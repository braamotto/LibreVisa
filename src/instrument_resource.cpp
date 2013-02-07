/* 
 * Copyright (C) 2013 Simon Richter
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

#include "instrument_resource.h"

#include "lock.h"
#include "timeval_op.h"

#include <sys/time.h>

namespace librevisa {

ViStatus instrument_resource::WaitOnEvent(
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

                timeout.tv_sec = start.tv_sec;
                timeout.tv_nsec = start.tv_usec * 1000;
        }

        locked lk(*this);

        for(;;)
        {
                size_type const count = lk.get_count();

                for(size_type i = 0; i < count; ++i)
                {
                        event::data &data = lk.at(i);
                        if(data.consumed)
                                continue;

                        if(data.eventType & inEventType)
                        {
                                data.consumed = true;
                                *outEventType = data.eventType;
                                if(outContext)
                                        return VI_ERROR_NSUP_OPER;
                                data.freed = true;
                                return VI_SUCCESS;
                        }
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

}
