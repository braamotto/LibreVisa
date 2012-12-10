#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"
#include "session.h"

using namespace freevisa;

extern "C" ViStatus viLock(ViSession vi, ViAccessMode lockType, ViUInt32 timeout, ViKeyId requestedKey, ViKeyId accessKey);

ViStatus viLock(ViSession vi, ViAccessMode lockType, ViUInt32 timeout, ViKeyId requestedKey, ViKeyId accessKey)
{
        try
        {
                return objects.get_session(vi)->Lock(lockType, timeout, requestedKey, accessKey);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
