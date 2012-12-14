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

        char *fmt = "%o%*.*d%1X%5d%.2s\\42foo\\rbar\\n\\\\baz\\05\\2342";

        if(viSetBuf(vi, VI_WRITE_BUF, 42) != VI_SUCCESS)
                return 1;
        if(viPrintf(vi, fmt, 9, 5, 3, 0, 0xbeef, 1234, "Fnord" ) != VI_SUCCESS)
                return 1;
        if(viFlush(vi, VI_WRITE_BUF) != VI_SUCCESS)
                return 1;

        dummy_reader_restart();

        if(dummy_reader_isempty())
                return 1;

        ViUInt32 count;
        ViByte const *data = dummy_reader_read(&count);

        char *testdata = "11  000BEEF 1234Fn\42foo\rbar\n\\baz\05\2342";

        if(count != strlen(testdata))
                return 1;

        if(memcmp(data, testdata, strlen(testdata)))
                return 1;

        unsigned char buf[42];
        if(viSPrintf(vi, buf, fmt, 9, 5, 3, 0, 0xbeef, 1234, "Fnord") != VI_SUCCESS)
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
