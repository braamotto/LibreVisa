#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"
#include "session.h"

#include <visa.h>

using namespace freevisa;

ViStatus viReadSTB(ViSession vi, ViPUInt16 retStatus)
{
        try
        {
                return objects.get_session(vi)->ReadSTB(retStatus);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
