#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "session.h"

namespace freevisa {

session::session(resource *res) :
        res(res),
        exclusive_lock_count(0),
        shared_lock_count(0)
{
        fmt_read_buf_siz = 0;
        fmt_read_buf_cnt = 0;
        fmt_read_buf = 0;

        fmt_write_buf_siz = 0;
        fmt_write_buf_cnt = 0;
        fmt_write_buf = 0;

        io_in_buf_siz = 0;
        io_in_buf_cnt = 0;
        io_in_buf = 0;

        io_out_buf_siz = 0;
        io_out_buf_cnt = 0;
        io_out_buf = 0;

}

ViStatus session::Close()
{
        return VI_SUCCESS;
}

ViStatus session::Open(ViRsrc rsrc, ViAccessMode accessMode, ViUInt32 timeout, ViSession *vi)
{
        return res->Open(rsrc, accessMode, timeout, vi);
}

ViStatus session::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        return res->Write(buf, count, retCount);
}

ViStatus session::Lock(ViAccessMode accessMode, ViUInt32, ViKeyId, ViKeyId accessKey)
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
                if(!nested && !res->lock_exclusive(this))
                        return VI_ERROR_RSRC_LOCKED;
                ++exclusive_lock_count;
                if(accessKey)
                        *accessKey = '\0';
                return nested ? VI_SUCCESS_NESTED_EXCLUSIVE : VI_SUCCESS;
        }

        // @todo

        return VI_SUCCESS;
}

ViStatus session::Unlock()
{
        // @todo

        if(exclusive_lock_count)
        {
                --exclusive_lock_count;
                if(!exclusive_lock_count)
                        res->unlock_exclusive();
        }
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

ViStatus session::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else if(shared_lock_count)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_SHARED_LOCK;
                else
                        return res->GetAttribute(VI_ATTR_RSRC_LOCK_STATE, attrState);
                return VI_SUCCESS;

        default:
                return res->GetAttribute(attr, attrState);
        }
}

ViStatus session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                return VI_ERROR_ATTR_READONLY;

        default:
                return res->SetAttribute(attr, attrState);
        }
}

}

