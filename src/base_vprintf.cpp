#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

namespace freevisa {

ViStatus base_vprintf(ViSession vi, ViPBuf userstring, ViString writeFmt, ViVAList)
{
	try
	{
                session *s = objects.get_session(vi);

                // @todo return VI_ERROR_INV_SESSION if session object invalid
                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(!userstring && strlen(writeFmt) + s->GetFmtWriteBufCnt() >= s->GetFmtWriteBufSiz())
                        return VI_ERROR_IO;

                ViByte *p = userstring ? userstring : s->GetFmtWriteBuf() + s->GetFmtWriteBufCnt();
                ViChar *f = writeFmt;

                for(; *f; f++) {
                        if(*f == '\\') {
                                switch(*++f) {
                                case 'n':
                                        *p++ = '\n';
                                        break;
                                case 'r':
                                        *p++ = '\r';
                                        break;
                                case 't':
                                        *p++ = '\t';
                                        break;
                                case '"':
                                        *p++ = '"';
                                        break;
                                case '\\':
                                        *p++ = '\\';
                                        break;
                                case '0' ... '7':
                                {
                                        char oct[3+1] = {0,0,0,0};
                                        if((*f >= '0' && *f <= '7')) {
                                                oct[0] = *f++;
                                                if((*f >= '0' && *f <= '7')) {
                                                        oct[1] = *f++;
                                                        if((*f >= '0' && *f <= '7'))
                                                                oct[2] = *f++;
                                                }
                                        }
                                        f--;
                                        ViByte n = strtoul(oct, 0, 8);
                                        *p++ = n;
                                        break;
                                }
                                default:
                                        return VI_ERROR_INV_FMT;
                                }
                        } else {
                                *p++ = *f;
                        }

                        // @todo Actually process format string

                        if(userstring)
                                userstring++;
                        else {
                                s->SetFmtWriteBufCnt(s->GetFmtWriteBufCnt()+1);

                                if(s->GetFmtWriteBufCnt() >= s->GetFmtWriteBufSiz()) {
                                        ViStatus ret;
                                        ret = viFlush(vi, VI_WRITE_BUF);
                                        if(ret != VI_SUCCESS)
                                                return ret;
                                }
                        }
                }

                if(userstring)
                        *userstring = '\0';

                return VI_SUCCESS;
	}
        catch(std::bad_alloc &e)
        {
                return VI_ERROR_ALLOC;
        }
	catch(exception &e)
	{
		return e.code;
	}
}

}
