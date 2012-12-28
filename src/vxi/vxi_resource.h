#ifndef freevisa_vxi_resource_h_
#define freevisa_vxi_resource_h_ 1

#include "resource.h"

#include <string>

#include <rpc/rpc.h>
#include <vxi.h>

namespace freevisa {
namespace vxi {

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

}
}

#endif
