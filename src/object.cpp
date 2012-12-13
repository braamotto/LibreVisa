#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object.h"

namespace freevisa {

object::object() :
        exclusive_lock_holder(0)
{
        return;
}

ViStatus object::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                *reinterpret_cast<ViVersion *>(attrState) = 0x00500100;
                return VI_SUCCESS;

        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_holder)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_NO_LOCK;
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

bool object::lock_exclusive(session const *ses)
{
        if(exclusive_lock_holder)
                return false;
        exclusive_lock_holder = ses;
        return true;
}

void object::unlock_exclusive()
{
        exclusive_lock_holder = 0;
        return;
}

}
