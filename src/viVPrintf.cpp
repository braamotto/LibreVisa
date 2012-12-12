#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>

#include "visa.h"
#include "base_vprintf.h"

using namespace freevisa;

ViStatus viVPrintf(ViSession vi, ViString writeFmt, ViVAList params)
{        
        return base_vprintf(vi, 0, writeFmt, params);
}
