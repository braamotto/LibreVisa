#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource.h"

namespace freevisa {

resource::resource() :
        refcount(0), exclusive_lock_holder(0)
{
        return;
}

ViStatus resource::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_holder)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_NO_LOCK;
                return VI_SUCCESS;

        default:
                return object::GetAttribute(attr, attrState);
        }
}

bool resource::lock_exclusive(session const *ses)
{
        if(exclusive_lock_holder)
                return false;
        exclusive_lock_holder = ses;
        return true;
}

void resource::unlock_exclusive()
{
        exclusive_lock_holder = 0;
        return;
}

}
