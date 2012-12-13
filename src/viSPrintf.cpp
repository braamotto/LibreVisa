#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdarg>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

extern "C" ViStatus viSPrintf(ViSession vi, ViPBuf buf, ViString writeFmt, ...)
{
	try
	{
                va_list arg_ptr;
                va_start(arg_ptr, writeFmt);
                ViStatus ret = viVSPrintf(vi, buf, writeFmt, arg_ptr);
                va_end (arg_ptr);
                return ret;
        }
	catch(exception &e)
	{
		return e.code;
	}
}
