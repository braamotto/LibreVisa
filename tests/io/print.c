/* 
 * Copyright (C) 2012 A. Pic
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

#include "dummy.h"

#include <visa.h>
#include <string.h>
#include <stdio.h>

int main()
{
        using_dummy_resource();

        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        ViSession vi;

        if(viOpen(rmgr, "DUMMY", VI_NO_LOCK, 0, &vi) != VI_SUCCESS)
                return 1;

        char *fmt = "%f%-5d%o%*.*d%1X%.2s\\42foo\\rbar\\n\\\\baz\\05\\2342";

        if(viSetBuf(vi, VI_WRITE_BUF, 42) != VI_SUCCESS)
                return 1;
        if(viPrintf(vi, fmt, 23.42, 1234, 9, 5, 3, 0, 0xbeef, "Fnord" ) != VI_SUCCESS)
                return 1;
        if(viFlush(vi, VI_WRITE_BUF) != VI_SUCCESS)
                return 1;

        dummy_reader_restart();

        if(dummy_reader_isempty())
                return 1;

        ViUInt32 count;
        ViByte const *data = dummy_reader_read(&count);

        char *testdata = "23.420001234 11  000BEEFFn\42foo\rbar\n\\baz\05\2342";

        if(count != strlen(testdata))
                return 1;

        if(memcmp(data, testdata, strlen(testdata)))
                return 1;

        unsigned char buf[42];
        if(viSPrintf(vi, buf, fmt, 23.42, 1234, 9, 5, 3, 0, 0xbeef, "Fnord") != VI_SUCCESS)
                return 1;

        if(memcmp(buf, testdata, strlen(testdata)))
                return 1;

        if(*(buf + strlen(testdata)) != '\0')
                return 1;

        if(viClose(vi) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr) != VI_SUCCESS)
                return 1;

        return 0;
}
