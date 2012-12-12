#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dummy.h"

#include <visa.h>

#include <string.h>

int main()
{
        using_dummy_resource();

        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        ViSession vi;

        if(viOpen(rmgr, "DUMMY", VI_NO_LOCK, 0, &vi) != VI_SUCCESS)
                return 1;

        ViByte testdata[] = { 't', 'e', 's', 't' };

        ViUInt32 written;

        if(viWrite(vi, testdata, sizeof testdata, &written) != VI_SUCCESS)
                return 1;

        if(written != sizeof testdata)
                return 1;

        dummy_reader_restart();

        if(dummy_reader_isempty())
                return 1;

        ViUInt32 count;
        ViByte const *data = dummy_reader_read(&count);

        if(count != sizeof testdata)
                return 1;

        if(memcmp(data, testdata, sizeof testdata))
                return 1;

        if(viClose(vi) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr) != VI_SUCCESS)
                return 1;

        return 0;
}
