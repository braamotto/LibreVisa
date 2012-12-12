#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tcpip_resource.h"

#include "resource_creator.h"

namespace freevisa {

ViStatus tcpip_resource::Close()
{
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

ViStatus tcpip_resource::Write(ViBuf, ViUInt32, ViUInt32 *)
{
        return VI_SUCCESS;
}

class tcpip_resource::creator :
        public resource_creator
{
public:
        virtual char const *name() const;
        virtual tcpip_resource *create(ViRsrc) const;
};

char const *tcpip_resource::creator::name() const
{
        return "TCPIP";
}

tcpip_resource *tcpip_resource::creator::create(ViRsrc) const
{
        return new tcpip_resource;
}

}
