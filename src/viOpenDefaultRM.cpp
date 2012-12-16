#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_manager.h"
#include "object_cache.h"

using namespace freevisa;

ViStatus viOpenDefaultRM(ViSession *sesn)
{
        ViStatus res = default_resource_manager.Open();
        if(res != VI_SUCCESS)
        {
                *sesn = VI_NULL;
                return res;
        }

        try
        {
                *sesn = objects.add(&default_resource_manager);
                return VI_SUCCESS;
        }
        catch(exception &e)
        {
                return e.code;
        }
}
