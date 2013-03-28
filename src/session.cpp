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

#include <sys/time.h>

namespace librevisa {

session::session(resource *res) :
        res(res),
        exclusive_lock_count(0),
        shared_lock_count(0)
{
        for(unsigned int i = 0; i < num_supported_events; ++i)
                queue_enabled[i] = false;
        res->add_session(this);
}

ViStatus session::Close()
{
        if(res->remove_session(this))
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

ViStatus session::EnableEvent(
        ViEventType eventType,
        ViUInt16 mechanism,
        ViEventFilter) // context)
{
        try
        {
                unsigned int ev = lookup_event(eventType);

                switch(mechanism)
                {
                case VI_QUEUE:
                        queue_enabled[ev] = true;
                        return VI_SUCCESS;

                default:
                        return VI_ERROR_NSUP_MECH;
                }
        }
        catch(exception &e)
        {
                return e.code;
        }
}

ViStatus session::DisableEvent(
        ViEventType eventType,
        ViUInt16 mechanism)
{
        try
        {
                unsigned int ev = lookup_event(eventType);

                switch(mechanism)
                {
                case VI_QUEUE:
                        queue_enabled[ev] = false;
                        return VI_SUCCESS;

                default:
                        return VI_ERROR_NSUP_MECH;
                }
        }
        catch(exception &e)
        {
                return e.code;
        }
}

ViStatus session::WaitOnEvent(
        ViEventType inEventType,
        ViUInt32 timeout_ms,
        ViPEventType outEventType,
        ViPEvent outContext)
{
        timespec timeout;
        if(timeout_ms != VI_TMO_IMMEDIATE)
        {
                timeval start;
                ::gettimeofday(&start, 0);

                timeout.tv_sec = start.tv_sec + timeout_ms / 1000;
                timeout.tv_nsec = (start.tv_usec + (timeout_ms % 1000) * 1000) * 1000;

                if(timeout.tv_nsec > 1000000000)
                {
                        timeout.tv_nsec -= 1000000000;
                        timeout.tv_sec += 1;
                }
        }

        bool const all_events = (inEventType == VI_ALL_ENABLED_EVENTS);

        event_queue::locked lk(queue);

        for(;;)
        {
                event_queue::size_type const count = lk.get_count();

                for(event_queue::size_type i = 0; i < count; ++i)
                {
                        event::data &data = lk.at(i);
                        if(data.consumed)
                                continue;

                        if(all_events || data.eventType == inEventType)
                        {
                                data.consumed = true;
                                *outEventType = data.eventType;
                                if(outContext)
                                        return VI_ERROR_NSUP_OPER;
                                data.freed = true;
                                return VI_SUCCESS;
                        }
                }

                if(all_events)
                {
                        bool have_enabled_events = true;
                        for(unsigned int i = 0; i < num_supported_events || (have_enabled_events = false); ++i)
                                if(queue_enabled[i])
                                        break;

                        if(!have_enabled_events)
                                return VI_ERROR_NENABLED;
                }
                else
                {
                        if(!queue_enabled[lookup_event(inEventType)])
                                return VI_ERROR_NENABLED;
                }

                if(timeout_ms == VI_TMO_INFINITE)
                {
                        lk.wait();
                }
                else if(timeout_ms == VI_TMO_IMMEDIATE || !lk.wait(timeout))
                {
                        if(outEventType)
                                *outEventType = 0;
                        if(outContext)
                                *outContext = VI_NULL;
                        return VI_ERROR_TMO;
                }
        }
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
                *reinterpret_cast<ViUInt32 *>(attrState) = event_queue::locked(queue).get_size();
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
                event_queue::locked(queue).set_size(attrState);
                return VI_SUCCESS;

        default:
                return res->SetAttribute(attr, attrState);
        }
}

unsigned int session::lookup_event(ViEventType eventType)
{
        unsigned int pos = 7;
        for(unsigned int stepsize = 4; stepsize && pos < num_supported_events; stepsize >>= 1)
        {
                if(supported_events[pos] < eventType)
                        pos += stepsize;
                else if(supported_events[pos] > eventType)
                        pos -= stepsize;

                if(supported_events[pos] == eventType)
                        return pos;
        }

        throw exception(VI_ERROR_INV_EVENT);
}

/** List of supported events. This list must be kept sorted. */
ViEventType session::supported_events[15] =
{
        VI_EVENT_IO_COMPLETION,    // 0x3FFF2009
        VI_EVENT_SERVICE_REQ,      // 0x3FFF200B
        VI_EVENT_CLEAR,            // 0x3FFF200D
        VI_EVENT_GPIB_CIC,         // 0x3FFF2012
        VI_EVENT_GPIB_TALK,        // 0x3FFF2013
        VI_EVENT_GPIB_LISTEN,      // 0x3FFF2014
        VI_EVENT_VXI_VME_SYSFAIL,  // 0x3FFF201D
        VI_EVENT_VXI_VME_SYSRESET, // 0x3FFF201E
        VI_EVENT_VXI_SIGP,         // 0x3FFF2020
        VI_EVENT_PXI_INTR,         // 0x3FFF2022
        VI_EVENT_TCPIP_CONNECT,    // 0x3FFF2036
        VI_EVENT_USB_INTR,         // 0x3FFF2037
        VI_EVENT_TRIG,             // 0xBFFF200A
        VI_EVENT_EXCEPTION,        // 0xBFFF200E
        VI_EVENT_VXI_VME_INTR      // 0xBFFF2021
};

}

