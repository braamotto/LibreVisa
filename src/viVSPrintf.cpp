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
                return base_vprintf(vi, buf, writeFmt, params);
        }
	catch(exception &e)
	{
		return e.code;
	}
}
