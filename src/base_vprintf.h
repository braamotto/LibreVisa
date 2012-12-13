#ifndef freevisa_base_vprintf_h_
#define freevisa_base_vprintf_h_ 1

namespace freevisa {
        // buf -> viVSPrintf, !buf -> viVPrintf
        ViStatus base_vprintf(ViSession vi, ViPBuf buf, ViString writeFmt, ViVAList params);
}

#endif
