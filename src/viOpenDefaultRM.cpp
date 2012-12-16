#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_manager.h"
#include "object_cache.h"

using namespace freevisa;

ViStatus viOpenDefaultRM(ViSession *sesn)
{
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
