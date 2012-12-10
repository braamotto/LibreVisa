#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>
#include <stdio.h>

int main()
{
        ViSession rm;
        ViStatus rc = viOpenDefaultRM(&rm);

        if(rc != VI_SUCCESS)
                return 1;
        if(rm == VI_NULL)
                return 1;

        viSetBuf(rm, VI_IO_OUT_BUF, 42);
        viPrintf(rm, "foobar\\nbaz");

        // @todo Do a debug output of buffer contents

        viClose(rm);

        return 0;
}
