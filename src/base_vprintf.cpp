#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

namespace freevisa {

// @todo stuff strings into buffer in chunks instead of single-character-wise for speed
ViStatus buf_put(ViSession vi, ViPBuf &userstring, ViChar c)
{
        session *s = objects.get_session(vi);

        if(userstring)
                *userstring++ = c;
        else {
                *(s->GetFmtWriteBuf() + s->GetFmtWriteBufCnt()) = c;
                s->SetFmtWriteBufCnt(s->GetFmtWriteBufCnt() + 1);

                if(s->GetFmtWriteBufCnt() >= s->GetFmtWriteBufSiz()) {
                        ViStatus ret;
                        ret = viFlush(vi, VI_WRITE_BUF);
                        if(ret != VI_SUCCESS)
                                return ret;
                }
        }

        return VI_SUCCESS;
}

void lltostr(char *pnum, unsigned long long i, int base, bool ucase)
{
        char buf[23];
        char *bufp = buf;

        if(!i)
                *bufp++ = '0';

        while(i) {
                char c = i % base + '0';
                if(c > '9')
                        c += (ucase?'A':'a') - '9' - 1;
                *bufp++ = c;
                i /= base;
        }

        for(--bufp; bufp >= buf; bufp--)
                *pnum++ = *bufp;

        *pnum = '\0';
}


ViStatus process_percent(ViSession vi, ViPBuf &userstring, ViChar *&f, ViVAList arg_ptr)
{
        char numbuf[23]; // has to hold signchar+LLONG_MAX chars
        char *pnum = numbuf;
        char *s;
        char *endptr;
        ViUInt32 fwidth = 0;
        ViUInt32 prec = 0;
        ViUInt32 num = 0;
        ViUInt32 i;
        bool isprec =  0;
        bool sign = 1;
        bool ucase = 0;
        int base = 10;

        ViStatus ret;

        f++; // eat %

in_fmt:
        switch(*f) {
        case '%':
                ret = buf_put(vi, userstring, '%');
                if(ret != VI_SUCCESS)
                        return ret;
                break;

        case 'c':
                ret = buf_put(vi, userstring, (char)va_arg(arg_ptr, int));
                if(ret != VI_SUCCESS)
                        return ret;
                break;

        case 's':
                s = va_arg(arg_ptr, char *);

                if(fwidth && fwidth > strlen(pnum)) {
                        for(ViUInt32 n=0; n < fwidth - strlen(pnum); n++) {
                                ViStatus ret = buf_put(vi, userstring, ' ');
                                if(ret != VI_SUCCESS)
                                        return ret;
                        }
                }
                for(i=0; *s && (prec ? i<prec : 1); i++, s++) {
                        ViStatus ret = buf_put(vi, userstring, *s);
                        if(ret != VI_SUCCESS)
                                return ret;
                }
                break;

        case 'X':
                ucase = 1;
        case 'x':
                base = 16;
        case 'u':
                sign = 0;
        case 'i':
        case 'd':
                num = va_arg(arg_ptr, int);

                if((ViInt32)num < 0 && sign) {
                        *pnum++ = '-';
                        num = 0 - num;
                }

                lltostr(pnum, num, base, ucase);

                if(fwidth && fwidth > strlen(pnum)) {
                        for(ViUInt32 n=0; n < fwidth - strlen(pnum); n++) {
                                ViStatus ret = buf_put(vi, userstring, ' ');
                                if(ret != VI_SUCCESS)
                                        return ret;
                        }
                }

                for(pnum = numbuf; *pnum && (prec ? ViUInt32(pnum - numbuf) < prec : 1); pnum++) {
                        ViStatus ret = buf_put(vi, userstring, *pnum);
                        if(ret != VI_SUCCESS)
                                return ret;
                }

                break;
        case '.':
                isprec = 1;
                f++;
                goto in_fmt;

        case '0' ... '9':
                if(isprec)
                        prec = strtoul(f, &endptr, 10);
                else
                        fwidth = strtoul(f, &endptr, 10);
                f = endptr;
                goto in_fmt;
        default:
                // @todo lots of missing formats

                return VI_ERROR_NSUP_FMT;
        }

        return VI_SUCCESS;
}


ViStatus process_backslash(ViSession vi, ViPBuf &userstring, ViChar *&f)
{
        char c;

        switch(*++f) {
        case 'n':
                c = '\n';
                break;
        case 'r':
                c = '\r';
                break;
        case 't':
                c = '\t';
                break;
        case '"':
                c = '"';
                break;
        case '\\':
                c = '\\';
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
                c = n;
                break;
        }
        default:
                return VI_ERROR_INV_FMT;
        }

        return buf_put(vi, userstring, c);
}


ViStatus base_vprintf(ViSession vi, ViPBuf userstring, ViString writeFmt, ViVAList arg_ptr)
{
        try
        {
                // @todo return VI_ERROR_INV_SESSION if session object invalid
                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                ViChar *f = writeFmt;

                for(; *f; f++) {
                        if(*f == '\\') {
                                ViStatus ret = process_backslash(vi, userstring, f);
                                if(ret != VI_SUCCESS)
                                        return ret;
                        } else if(*f == '%') {
                                ViStatus ret = process_percent(vi, userstring, f, arg_ptr);
                                if(ret != VI_SUCCESS)
                                        return ret;
                        } else {
                                ViStatus ret = buf_put(vi, userstring, *f);
                                if(ret != VI_SUCCESS)
                                        return ret;
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
