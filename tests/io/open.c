#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

#include "dummy.h"

int main()
{
        using_dummy_resource();

        ViSession rm;

        ViStatus rc = viOpenDefaultRM(&rm);

        if(rc != VI_SUCCESS)
                return 1;

        ViSession dummySession;

        rc = viOpen(rm, "DUMMY", VI_NO_LOCK, 0, &dummySession);

        if(rc != VI_SUCCESS)
                return 1;

        if(dummySession == VI_NULL)
                return 1;

        if(dummySession == rm)
                return 1;

        rc = viClose(dummySession);

        if(rc != VI_SUCCESS)
                return 1;

        rc = viClose(rm);

        if(rc != VI_SUCCESS)
                return 1;

        return 0;
}
