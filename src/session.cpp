/* 
 * Copyright (C) 2012,2013 Simon Richter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "session.h"

namespace librevisa {

session::session(resource *res) :
        res(res),
        exclusive_lock_count(0),
        shared_lock_count(0),
        event_queue_length(0)
{
        res->add_ref();
}

ViStatus session::Close()
{
        if(res->release() != 0)
                return VI_SUCCESS;
        return res->Close();
}

ViStatus session::Open(ViRsrc rsrc, ViAccessMode accessMode, ViUInt32 timeout, ViSession *vi)
{
        return res->Open(rsrc, accessMode, timeout, vi);
}

ViStatus session::FindRsrc(ViString expr, ViFindList *findList, ViUInt32 *retCount, ViRsrc instrDesc)
{
        return res->FindRsrc(expr, findList, retCount, instrDesc);
}

ViStatus session::Read(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        return res->Read(buf, count, retCount);
}

ViStatus session::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        return res->Write(buf, count, retCount);
}

ViStatus session::ReadSTB(ViUInt16 *retStatus)
{
        return res->ReadSTB(retStatus);
}

ViStatus session::WaitOnEvent(ViEventType inEventType, ViUInt32 timeout, ViPEventType outEventType, ViPEvent outContext)
{
        return res->WaitOnEvent(inEventType, timeout, outEventType, outContext);
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

        /// @todo

        return VI_SUCCESS;
}

ViStatus session::Unlock()
{
        /// @todo

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

        case VI_ATTR_MAX_QUEUE_LENGTH:
                *reinterpret_cast<ViUInt32 *>(attrState) = event_queue_length;
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

        case VI_ATTR_MAX_QUEUE_LENGTH:
                if(attrState > 0xffffffff)
                        return VI_ERROR_NSUP_ATTR_STATE;
                event_queue_length = attrState;
                return VI_SUCCESS;

        default:
                return res->SetAttribute(attr, attrState);
        }
}

}

