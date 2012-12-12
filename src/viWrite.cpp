#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"

#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viWrite(ViSession vi, ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        try
        {
                return objects.get_session(vi)->Write(buf, count, retCount);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
