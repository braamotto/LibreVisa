#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object.h"

namespace freevisa {

ViStatus object::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                *reinterpret_cast<ViVersion *>(attrState) = 0x00500100;
                return VI_SUCCESS;

        default:
                return VI_ERROR_NSUP_ATTR;
        }
}

ViStatus object::SetAttribute(ViAttr attr, ViAttrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                return VI_ERROR_ATTR_READONLY;

        default:
                return VI_ERROR_NSUP_ATTR;
        }
}

}
