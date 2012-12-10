#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "local_session.h"

namespace freevisa {

local_session::local_session() :
        exclusive_lock_count(0),
        shared_lock_count(0)
{
        return;
}

ViStatus local_session::Close()
{
        // @todo

        return VI_SUCCESS;
}

ViStatus local_session::Lock(ViAccessMode accessMode, ViUInt32, ViKeyId, ViKeyId accessKey)
{
        if(accessMode != VI_SHARED_LOCK && accessMode != VI_EXCLUSIVE_LOCK)
                return VI_ERROR_INV_ACC_MODE;

        if(accessMode == VI_SHARED_LOCK)
        {
                ++shared_lock_count;
                return VI_SUCCESS;
        }

        if(accessMode == VI_EXCLUSIVE_LOCK)
        {
                ++exclusive_lock_count;
                if(accessKey)
                        *accessKey = '\0';
                return VI_SUCCESS;
        }

        // @todo

        return VI_SUCCESS;
}

ViStatus local_session::Unlock()
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


ViStatus local_session::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else if(shared_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_SHARED_LOCK;
                else
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_NO_LOCK;
                return VI_SUCCESS;

        default:
                return session::GetAttribute(attr, attrState);
        }
}

ViStatus local_session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                return VI_ERROR_ATTR_READONLY;

        default:
                return session::SetAttribute(attr, attrState);
        }
}

}
