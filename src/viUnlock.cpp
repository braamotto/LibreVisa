#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viUnlock(ViSession vi)
{
        try
        {
                return objects.get_session(vi)->Unlock();
        }
        catch(exception &e)
        {
                return e.code;
        }
}
