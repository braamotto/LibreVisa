#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object.h"

namespace freevisa {

object::object() :
        exclusive_lock_count(0),
        shared_lock_count(0)
{
        return;
}

ViStatus object::Lock(ViAccessMode accessMode, ViUInt32, ViKeyId, ViKeyId accessKey)
{
        if(accessMode != VI_SHARED_LOCK && accessMode != VI_EXCLUSIVE_LOCK)
                return VI_ERROR_INV_ACC_MODE;

        if(accessMode == VI_SHARED_LOCK)
        {
                bool const nested = (shared_lock_count);
                ++shared_lock_count;
                return nested ? VI_SUCCESS_NESTED_SHARED : VI_SUCCESS;
        }

        if(accessMode == VI_EXCLUSIVE_LOCK)
        {
                bool const nested = (exclusive_lock_count);
                ++exclusive_lock_count;
                if(accessKey)
                        *accessKey = '\0';
                return nested ? VI_SUCCESS_NESTED_EXCLUSIVE : VI_SUCCESS;
        }

        // @todo

        return VI_SUCCESS;
}

ViStatus object::Unlock()
{
        // @todo

        if(exclusive_lock_count)
                --exclusive_lock_count;
        else if(shared_lock_count)
                --shared_lock_count;
        else
                return VI_ERROR_SESN_NLOCKED;

        if(exclusive_lock_count)
                return VI_SUCCESS_NESTED_EXCLUSIVE;
        else if(shared_lock_count)
                return VI_SUCCESS_NESTED_SHARED;
        else
                return VI_SUCCESS;
}

ViStatus object::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                *reinterpret_cast<ViVersion *>(attrState) = 0x00500100;
                return VI_SUCCESS;

        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else if(shared_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_SHARED_LOCK;
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
        case VI_ATTR_RSRC_LOCK_STATE:
                return VI_ERROR_ATTR_READONLY;

        default:
                return VI_ERROR_NSUP_ATTR;
        }
}

}
