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

        ViSession dummySession1;

        rc = viOpen(rm, "DUMMY", VI_NO_LOCK, 0, &dummySession1);

        if(rc != VI_SUCCESS)
                return 1;

        if(dummySession1 == VI_NULL)
                return 1;

        if(dummySession1 == rm)
                return 1;

        ViSession dummySession2;

        rc = viOpen(rm, "DUMMY", VI_NO_LOCK, 0, &dummySession2);

        if(rc != VI_SUCCESS)
                return 1;

        if(dummySession2 == VI_NULL)
                return 1;

        if(dummySession2 == dummySession1)
                return 1;

        if(dummySession2 == rm)
                return 1;

        rc = viClose(dummySession2);

        if(rc != VI_SUCCESS)
                return 1;

        rc = viClose(dummySession1);

        if(rc != VI_SUCCESS)
                return 1;

        rc = viClose(rm);

        if(rc != VI_SUCCESS)
                return 1;

        return 0;
}
