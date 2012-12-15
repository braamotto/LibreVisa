#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tcpip_resource.h"

#include "resource_creator.h"

namespace freevisa {

ViStatus tcpip_resource::Close()
{
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

        // @todo handle errors

        return VI_SUCCESS;
}

class tcpip_resource::creator :
        public resource_creator
{
public:
        virtual tcpip_resource *create(ViRsrc) const;
};

tcpip_resource *tcpip_resource::creator::create(ViRsrc) const
{
        return new tcpip_resource;
}

}
