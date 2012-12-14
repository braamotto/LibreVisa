#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>
#include <cstdarg>

#include "visa.h"
#include "base_vprintf.h"

using namespace freevisa;

ViStatus viVPrintf(ViSession vi, ViString writeFmt, ViVAList params)
{

        va_list aq;
        va_copy(aq, params);
        ViStatus ret = base_vprintf(vi, 0, writeFmt, aq);
        va_end(aq);
        return ret;
}
