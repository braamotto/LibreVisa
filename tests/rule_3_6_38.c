#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
        ViSession vi;

        ViStatus rc = viOpenDefaultRM(&vi);

        if(rc != VI_SUCCESS)
                return 1;

        rc = viUnlock(vi);

        if(rc != VI_ERROR_SESN_NLOCKED)
                return 1;

        return 0;
}
