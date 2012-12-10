#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
        ViSession rmgr;

        ViStatus rc = viGetDefaultRM(&rmgr);

        if(rc != VI_SUCCESS)
                return 1;

        ViSession rmgr_session;

        rc = viGetAttribute(rmgr, VI_ATTR_RM_SESSION, &rmgr_session);

        if(rc != VI_SUCCESS)
                return 1;

        if(rmgr_session != VI_NULL)
                return 1;

        return 0;
}
