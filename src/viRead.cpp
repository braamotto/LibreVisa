#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"
#include "session.h"

#include <visa.h>

using namespace freevisa;

ViStatus viRead(ViSession vi, ViPBuf buf, ViUInt32 count, ViPUInt32 retCount)
{
        try
        {
                return objects.get_session(vi)->Read(buf, count, retCount);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
