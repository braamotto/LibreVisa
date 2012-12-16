#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
        ViSession rmgr;

        if(viOpenDefaultRM(&rmgr) != VI_SUCCESS)
                return 1;

        if(viSetAttribute(rmgr, VI_ATTR_MAX_QUEUE_LENGTH, 10) != VI_SUCCESS)
                return 1;

        ViUInt32 max_queue_length;

        if(viGetAttribute(rmgr, VI_ATTR_MAX_QUEUE_LENGTH, &max_queue_length) != VI_SUCCESS)
                return 1;

        if(max_queue_length != 10)
                return 1;

        return 0;
}
