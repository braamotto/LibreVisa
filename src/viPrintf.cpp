#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdarg>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

extern "C" ViStatus viPrintf(ViSession vi, ViString writeFmt, ...)
{        
	try
	{
                va_list arg_ptr;
                va_start(arg_ptr, writeFmt);
                ViStatus ret = viVPrintf(vi, writeFmt, arg_ptr);
                va_end (arg_ptr);
                return ret;
        }
	catch(exception &e)
	{
		return e.code;
	}
}
