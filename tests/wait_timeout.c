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

#include <visa.h>

#include <sys/types.h>
#include <sys/time.h>

#include "dummy.h"

int main()
{
        using_dummy_resource();

        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        ViSession vi;
        if(viOpen(rmgr, "DUMMY", VI_NO_LOCK, 0, &vi) != VI_SUCCESS)
                return 1;

        if(viEnableEvent(vi, VI_EVENT_IO_COMPLETION, VI_QUEUE, 0) != VI_SUCCESS)
                return 1;

        struct timeval start;
        if(gettimeofday(&start, 0) != 0)
                return 1;

        if(viWaitOnEvent(vi, VI_ALL_ENABLED_EVENTS, 1000, VI_NULL, VI_NULL) != VI_ERROR_TMO)
                return 1;

        struct timeval end;
        if(gettimeofday(&end, 0) != 0)
                return 1;

        unsigned int elapsed =
                (end.tv_sec - start.tv_sec) * 1000000 +
                end.tv_usec - start.tv_usec;

        if(elapsed < 1000000 || elapsed > 1100000)
                return 1;

        if(viClose(vi) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr) != VI_SUCCESS)
                return 1;

        return 0;
}
