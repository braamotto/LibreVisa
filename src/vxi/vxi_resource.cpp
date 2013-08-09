/* 
 * Copyright (C) 2012 Simon Richter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vxi_resource.h"

#include "exception.h"

#include <vxi_intr.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>

#include <iostream>

extern "C" void device_intr_1(struct svc_req *rqstp, register SVCXPRT *transp);

namespace librevisa {
namespace vxi {

struct vxi_resource::action
{
        // completion notification
        mutex cs;
        condvar cv;

        enum {
                OPEN,
                READSTB,
                READ,
                WRITE
        } op;

        ViStatus status;

        union
        {
                struct { } open;
                struct
                { 
                        unsigned char stb;
                } readstb;
                struct
                {
                        unsigned char *buffer;
                        size_t size;
                        size_t count;
                } read;
                struct
                {
                        unsigned char const *buffer;
                        size_t size;
                        size_t count;
                } write;
        };
};

vxi_resource::vxi_resource(std::string const &hostname) :
        hostname(hostname),
        io_timeout(1000), lock_timeout(1000)
{
        main.register_timeout(*this);

        action ac;
        ac.op = ac.OPEN;
        perform(ac);

        /// @todo handle errors
        return;
}

vxi_resource::~vxi_resource() throw()
{
        return;
}

void vxi_resource::do_open(action &)
{
        int sock = RPC_ANYSOCK;

        addrinfo hints =
        {
                AI_ADDRCONFIG,
                AF_INET,
                SOCK_STREAM,
                IPPROTO_TCP,
                0,
                0,
                0,
                0
        };

        addrinfo *results;

        int rc = getaddrinfo(
                hostname.c_str(),
                0,
                &hints,
                &results);
        if(rc != 0)
                throw exception(VI_ERROR_RSRC_NFOUND);

        for(addrinfo *i = results; i; i = i->ai_next)
        {
                if(i->ai_addr->sa_family != AF_INET)
                        continue;

                sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(i->ai_addr);
                sin->sin_port = 0;
                client = clnttcp_create(sin, DEVICE_CORE, DEVICE_CORE_VERSION, &sock, 0, 0);
                if(client)
                        break;
        }

        if(!client)
                throw exception(VI_ERROR_RSRC_NFOUND);

        Create_LinkParms clp = { 1, false, lock_timeout, const_cast<char *>("inst0") };
        Create_LinkResp *resp = create_link_1(&clp, client);

        if(!resp)
                throw exception(VI_ERROR_RSRC_NFOUND);

        lid = resp->lid;

        SVCXPRT *intr_svc = svctcp_create(RPC_ANYSOCK, 0, 0);

        if(!intr_svc)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(!svc_register(intr_svc, DEVICE_INTR, DEVICE_INTR_VERSION, device_intr_1, 0))
                throw exception(VI_ERROR_SYSTEM_ERROR);

        sockaddr_storage ss;

        socklen_t ss_len = sizeof ss;
        rc = getsockname(sock, reinterpret_cast<sockaddr *>(&ss), &ss_len);
        if(rc == -1)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(ss.ss_family != AF_INET)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        sockaddr_in *sin = reinterpret_cast<sockaddr_in *>(&ss);

        Device_RemoteFunc rfn =
        {
                ntohl(sin->sin_addr.s_addr),
                intr_svc->xp_port,
                DEVICE_INTR,
                DEVICE_INTR_VERSION,
                DEVICE_TCP
        };

        Device_Error *error = create_intr_chan_1(&rfn, client);

        if(!error)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(error->error == 8)
                ; /// @todo handle case where interrupt channel is unsupported
        else if(error->error)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        fd = sock;
        event = messagepump::read;
        main.register_watch(*this);

        Device_EnableSrqParms esp =
        {
                lid,
                true,
                {
                        5,
                        "hello"
                }
        };

        error = device_enable_srq_1(&esp, client);
        if(!error)
                throw exception(VI_ERROR_SYSTEM_ERROR);
        if(error->error)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        /// @todo handle errors
        return;
}

ViStatus vxi_resource::Close()
{
        main.unregister_watch(*this);
        destroy_link_1(&lid, client);
        clnt_destroy(client);
        delete this;
        return VI_SUCCESS;
}

ViStatus vxi_resource::Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId)
{
        return VI_SUCCESS;
}

ViStatus vxi_resource::Unlock()
{
        return VI_SUCCESS;
}

ViStatus vxi_resource::GetAttribute(ViAttr attr, void *attrState)
{
        return resource::GetAttribute(attr, attrState);
}

ViStatus vxi_resource::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        return resource::SetAttribute(attr, attrState);
}

ViStatus vxi_resource::Read(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        action ac;
        ac.op = ac.READ;
        ac.read.buffer = buf;
        ac.read.size = count;
        perform(ac);
        *retCount = ac.read.count;
        return ac.status;
}

void vxi_resource::do_read(action &ac)
{
        Device_ReadParms read_parms =
        {
                lid,
                ac.read.size,
                io_timeout,
                lock_timeout,
                0,
                0
        };

        Device_ReadResp *read_resp = device_read_1(&read_parms, client);

        ::memcpy(ac.read.buffer, read_resp->data.data_val, read_resp->data.data_len);
        ac.read.count = read_resp->data.data_len;
        /// @todo handle errors
        ac.status = VI_SUCCESS;
}

ViStatus vxi_resource::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        action ac;
        ac.op = ac.WRITE;
        ac.write.buffer = buf;
        ac.write.size = count;
        perform(ac);
        *retCount = ac.write.count;
        return ac.status;
}

void vxi_resource::do_write(action &ac)
{
        Device_WriteParms write_parms =
        {
                lid,
                io_timeout,
                lock_timeout,
                8,
                {
                        static_cast<u_int>(ac.write.size),
                        reinterpret_cast<char *>(const_cast<unsigned char *>(ac.write.buffer))
                }
        };

        Device_WriteResp *write_resp = device_write_1(&write_parms, client);

        if(!write_resp)
        {
                ac.status = VI_ERROR_SYSTEM_ERROR;
                return;
        }
        
        ac.write.count = write_resp->size;

        /// @todo handle errors
        ac.status = VI_SUCCESS;
}

ViStatus vxi_resource::ReadSTB(ViUInt16 *retStatus)
{
        action ac;
        ac.op = ac.READSTB;
        perform(ac);
        *retStatus = ac.readstb.stb;
        return ac.status;
}

void vxi_resource::do_readstb(action &ac)
{
        Device_GenericParms dgp =
        {
                lid,
                0,
                lock_timeout,
                io_timeout
        };
        Device_ReadStbResp *resp = device_readstb_1(&dgp, client);

        /// @todo handle errors

        ac.readstb.stb = resp->stb;
        ac.status = VI_SUCCESS;
}

void vxi_resource::notify_fd_event(int fd, messagepump::fd_event)
{
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        svc_getreqset(&readfds);
}

void vxi_resource::notify_timeout()
{
        if(actions.empty())
                return;

        action &ac = *actions.front();
        
        {
                lock l(ac.cs);

                switch(ac.op)
                {
                case action::OPEN:              do_open(ac);    break;
                case action::READSTB:           do_readstb(ac); break;
                case action::READ:              do_read(ac);    break;
                case action::WRITE:             do_write(ac);   break;
                }

                ac.cv.signal();
        }

        actions.pop();
}

void vxi_resource::cleanup()
{
        return;
}

void vxi_resource::perform(action &ac)
{
        lock l(ac.cs);
        actions.push(&ac);

        timeval now;
        ::gettimeofday(&now, 0);
        main.update_timeout(*this, &now);

        ac.cv.wait(ac.cs);
}

}
}

void *device_intr_srq_1_svc(Device_SrqParms *, svc_req *)
{
        return 0;
}

