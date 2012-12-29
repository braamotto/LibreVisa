#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viFindRsrc(ViSession vi, ViString expr, ViPFindList findList, ViPUInt32 retCount, ViRsrc instrDesc)
{
        try
        {
                return objects.get_session(vi)->FindRsrc(expr, findList, retCount, instrDesc);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
