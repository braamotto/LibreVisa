#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        if(viLock(rmgr, VI_EXCLUSIVE_LOCK, 0, VI_NULL, VI_NULL) != VI_SUCCESS)
                return 1;

        if(viLock(rmgr, VI_EXCLUSIVE_LOCK, 0, VI_NULL, VI_NULL) != VI_SUCCESS_NESTED_EXCLUSIVE)
                return 1;

        if(viUnlock(rmgr) != VI_SUCCESS_NESTED_EXCLUSIVE)
                return 1;

        if(viUnlock(rmgr) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr) != VI_SUCCESS)
                return 1;

        return 0;
}
