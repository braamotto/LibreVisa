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

        ViAccessMode access;

        rc = viGetAttribute(vi, VI_ATTR_RSRC_LOCK_STATE, &access);

        if(rc != VI_SUCCESS)
                return 1;

        if(access != VI_NO_LOCK)
                return 1;

        ViChar key[256];

        rc = viLock(vi, VI_NO_LOCK, 0, VI_NULL, key);

        if(rc != VI_ERROR_INV_ACC_MODE)
                return 1;

        return 0;
}
