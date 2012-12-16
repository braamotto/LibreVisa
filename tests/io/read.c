#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dummy.h"

#include <visa.h>

int main()
{
        using_dummy_resource();

        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        ViSession vi;

        if(viOpen(rmgr, "DUMMY", VI_NO_LOCK, 0, &vi) != VI_SUCCESS)
                return 1;

        dummy_writer_append("test", 4, VI_SUCCESS);

        ViByte buf[10];
        ViUInt32 count;

        if(viRead(vi, buf, sizeof buf, &count) != VI_SUCCESS)
                return 1;

        if(count != 4)
                return 1;

        if(viClose(vi) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr) != VI_SUCCESS)
                return 1;

        return 0;
}
