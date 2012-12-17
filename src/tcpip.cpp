#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource.h"

#include "resource_creator.h"
#include "resource_manager.h"

#include "exception.h"

#include <limits>
#include <string>

#include <rpc/rpc.h>

#include <vxi.h>

namespace {

bool is_valid_in_hostname(char c)
{
        return
                (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c == '-') || (c == '.');
}

unsigned int parse_optional_int(char const *&cursor)
{
        unsigned int ret = 0;
        while(*cursor >= '0' && *cursor <= '9')
        {
                if(ret > std::numeric_limits<unsigned int>::max() / 10)
                        return 0;
                ret *= 10;
                ret += *cursor - '0';
        }
        return ret;
}

}

namespace freevisa {

class tcpip_resource :
        public resource
{
public:
        tcpip_resource(std::string const &hostname);

        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);

private:
        // RPC
        CLIENT *client;

        Device_Link lid;

        u_long io_timeout;
        u_long lock_timeout;

        class creator;
        static creator const creator_inst;
};

tcpip_resource::tcpip_resource(std::string const &hostname) :
        io_timeout(10), lock_timeout(10)
{
        client = clnt_create(hostname.c_str(), DEVICE_ASYNC, DEVICE_ASYNC_VERSION, "tcp");

        if(!client)
                throw exception(VI_ERROR_RSRC_NFOUND);

        Create_LinkParms clp = { 1, false, lock_timeout, const_cast<char *>("INSTR") };
        Create_LinkResp *resp = create_link_1(&clp, client);

        /// @todo handle errors
        lid = resp->lid;
        return;
}

ViStatus tcpip_resource::Close()
{
        destroy_link_1(&lid, client);
        clnt_destroy(client);
        delete this;
        return VI_SUCCESS;
}

ViStatus tcpip_resource::Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId)
{
        return VI_SUCCESS;
}

ViStatus tcpip_resource::Unlock()
{
        return VI_SUCCESS;
}

ViStatus tcpip_resource::GetAttribute(ViAttr attr, void *attrState)
{
        return resource::GetAttribute(attr, attrState);
}

ViStatus tcpip_resource::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        return resource::SetAttribute(attr, attrState);
}

ViStatus tcpip_resource::Read(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
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

ViStatus tcpip_resource::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        Device_WriteParms write_parms =
        {
                lid,
                io_timeout,
                lock_timeout,
                0,
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

class tcpip_resource::creator :
        public resource_creator
{
public:
        creator();

        virtual tcpip_resource *create(ViRsrc) const;

        static creator instance;
};

tcpip_resource::creator::creator()
{
        default_resource_manager.register_creator(*this);
}

tcpip_resource *tcpip_resource::creator::create(ViRsrc rsrc) const
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

        return new tcpip_resource(std::string(hostname, hostname_end));
}

tcpip_resource::creator tcpip_resource::creator::instance;

}
