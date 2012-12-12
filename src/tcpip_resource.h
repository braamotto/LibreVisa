#ifndef tcpip_resource_h_
#define tcpip_resource_h_ 1

#include "resource.h"

#include <rpc/rpc.h>

#include <vxi.h>

namespace freevisa {

class tcpip_resource :
        public resource
{
public:
        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
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

}

#endif
