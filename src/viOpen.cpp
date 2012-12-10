#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"

#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viOpen(ViSession sesn, ViRsrc rsrcName, ViAccessMode accessMode, ViUInt32 timeout, ViSession *vi)
{
        try
        {
                return objects.get_session(sesn)->Open(rsrcName, accessMode, timeout, vi);
        }
        catch(exception &e)
        {
                return e.code;
        }
}
