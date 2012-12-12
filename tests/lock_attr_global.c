#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
        ViSession rmgr1, rmgr2;

        if(viOpenDefaultRM(&rmgr1) != VI_SUCCESS)
                return 1;

        if(viOpenDefaultRM(&rmgr2) != VI_SUCCESS)
                return 1;

        ViAccessMode accessMode;

        if(viGetAttribute(rmgr1, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_NO_LOCK)
                return 1;

        if(viGetAttribute(rmgr2, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_NO_LOCK)
                return 1;

        if(viLock(rmgr1, VI_EXCLUSIVE_LOCK, 0, VI_NULL, VI_NULL) != VI_SUCCESS)
                return 1;

        if(viGetAttribute(rmgr1, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_EXCLUSIVE_LOCK)
                return 1;

        if(viGetAttribute(rmgr2, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_EXCLUSIVE_LOCK)
                return 1;

        if(viUnlock(rmgr1) != VI_SUCCESS)
                return 1;

        if(viGetAttribute(rmgr1, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_NO_LOCK)
                return 1;

        if(viGetAttribute(rmgr2, VI_ATTR_RSRC_LOCK_STATE, &accessMode) != VI_SUCCESS)
                return 1;

        if(accessMode != VI_NO_LOCK)
                return 1;

        if(viClose(rmgr2) != VI_SUCCESS)
                return 1;

        if(viClose(rmgr1) != VI_SUCCESS)
                return 1;

        return 0;
}
