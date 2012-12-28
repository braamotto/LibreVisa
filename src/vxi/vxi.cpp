#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource.h"

#include "resource_creator.h"
#include "resource_manager.h"

#include "util.h"

#include "exception.h"

#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <rpc/rpc.h>

#include <vxi.h>
#include <vxi_intr.h>

extern "C" void device_intr_1(struct svc_req *rqstp, register SVCXPRT *transp);

namespace {

bool is_valid_in_hostname(char c)
{
        return
                (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c == '-') || (c == '.');
}

}

void *device_intr_srq_1_svc(Device_SrqParms *, svc_req *)
{
        return 0;
}

namespace freevisa {

class vxi_resource :
        public resource
{
private:
        vxi_resource(std::string const &hostname);

        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);

        // RPC
        CLIENT *client;

        Device_Link lid;

        u_long io_timeout;
        u_long lock_timeout;

        class creator;
};

vxi_resource::vxi_resource(std::string const &hostname) :
        io_timeout(10), lock_timeout(10)
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

        SVCXPRT *intr_svc = svcudp_create(RPC_ANYSOCK);

        if(!intr_svc)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(!svc_register(intr_svc, DEVICE_ASYNC, DEVICE_ASYNC_VERSION, device_intr_1, IPPROTO_UDP))
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
                ntohs(sin->sin_port),
                DEVICE_ASYNC,
                DEVICE_ASYNC_VERSION,
                DEVICE_UDP
        };

        Device_Error *error = create_intr_chan_1(&rfn, client);

        if(!error)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(error->error)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        /// @todo handle errors
        lid = resp->lid;
        return;
}

ViStatus vxi_resource::Close()
{
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
        Device_ReadParms read_parms =
        {
                lid,
                count,
                io_timeout,
                lock_timeout,
                0,
                0
        };

        Device_ReadResp *read_resp = device_read_1(&read_parms, client);

        ::memcpy(buf, read_resp->data.data_val, read_resp->data.data_len);
        *retCount = read_resp->data.data_len;

        /// @todo handle errors

        return VI_SUCCESS;
}

ViStatus vxi_resource::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        Device_WriteParms write_parms =
        {
                lid,
                io_timeout,
                lock_timeout,
                8,
                {
                        static_cast<u_int>(count),
                        reinterpret_cast<char *>(buf)
                }
        };

        Device_WriteResp *write_resp = device_write_1(&write_parms, client);

        *retCount = write_resp->size;

        /// @todo handle errors

        return VI_SUCCESS;
}

class vxi_resource::creator :
        public resource_creator
{
private:
        creator();
        ~creator() throw();

        virtual vxi_resource *create(ViRsrc) const;

        static creator instance;
};

vxi_resource::creator::creator()
{
        default_resource_manager.register_creator(*this);
}

vxi_resource::creator::~creator() throw()
{
        default_resource_manager.unregister_creator(*this);
}

vxi_resource *vxi_resource::creator::create(ViRsrc rsrc) const
{
        // Expect "TCPIP"
        if((rsrc[0] | 0x20) != 't' ||
                (rsrc[1] | 0x20) != 'c' ||
                (rsrc[2] | 0x20) != 'p' ||
                (rsrc[3] | 0x20) != 'i' ||
                (rsrc[4] | 0x20) != 'p')
        {
                return 0;
        }

        // Expect optional board number
        char const *cursor = &rsrc[5];
        /*unsigned int board = */(void)parse_optional_int(cursor);

        // Expect two colons
        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        // Expect host name
        char const *const hostname = cursor;

        while(is_valid_in_hostname(*cursor))
                ++cursor;

        char const *const hostname_end = cursor;

        // Expect two colons
        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        // Expect "INSTR"
        char const *const type = cursor;

        if((type[0] | 0x20) != 'i' ||
                (type[1] | 0x20) != 'n' ||
                (type[2] | 0x20) != 's' ||
                (type[3] | 0x20) != 't' ||
                (type[4] | 0x20) != 'r')
        {
                return 0;
        }

        cursor += 5;

        // Expect optional instrument instance number
        /*unsigned int instance = */(void)parse_optional_int(cursor);

        // Expect NUL
        if(*cursor != '\0')
                return 0;

        return new vxi_resource(std::string(hostname, hostname_end));
}

vxi_resource::creator vxi_resource::creator::instance;

}
