#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>

#include "visa.h"
#include "object_cache.h"
#include "session.h"
#include "base_vprintf.h"

using namespace freevisa;

ViStatus viVSPrintf(ViSession vi, ViPBuf buf, ViString writeFmt, ViVAList params)
{
        try
        {
                va_list aq;
                va_copy(aq, params);
                ViStatus ret = base_vprintf(vi, buf, writeFmt, aq);
                va_end(aq);
                return ret;
        }
	catch(exception &e)
	{
		return e.code;
	}
}
