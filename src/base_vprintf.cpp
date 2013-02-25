/* 
 * Copyright (C) 2012 A. Pic
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

namespace librevisa {

/// @todo stuff strings into buffer in chunks instead of single-character-wise for speed
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
        case '0': case '1': case '2': case '3':
        case '4': case '5': case '6': case '7':
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
                /// @todo return VI_ERROR_INV_SESSION if session object invalid
                /// @todo return VI_ERROR_RSRC_LOCKED if object locked

                ViChar *f = writeFmt;

                for(; *f; f++) {
                        if(*f == '\\') {
                                ViStatus ret = process_backslash(vi, userstring, f);
                                if(ret != VI_SUCCESS)
                                        return ret;
                        } else if(*f == '%') {
                                char numbuf[23];
                                char *pnum = numbuf;
                                char *s;
                                char *endptr;
                                ViUInt32 fwidth = 0;
                                ViInt32 prec = -1;
                                ViInt32 nprecnum;
                                long long num = 0;
                                bool isprec =  0;
                                bool sign = 1;
                                bool ucase = 0;
                                int base = 10;
                                int lng = 0;
                                bool shrt = 0;
                                bool padright = 0;

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

                                        for(; *s && (prec > -1 ? prec-- : 1); s++) {
                                                ViStatus ret = buf_put(vi, userstring, *s);
                                                if(ret != VI_SUCCESS)
                                                        return ret;
                                        }
                                        break;

                                case 'L':
                                case 'l':
                                        lng++;
                                        f++;
                                        if(lng > 2)
                                                return VI_ERROR_INV_FMT;
                                        goto in_fmt;
                                case 'h':
                                        if(shrt)
                                                return VI_ERROR_INV_FMT;
                                        shrt=1;
                                        f++;
                                        goto in_fmt;

                                case 'X':
                                        ucase = 1;
                                case 'x':
                                        base = 16;
                                case 'u':
                                        sign = 0;
                                case 'i':
                                case 'd':
                                do_num:
                                        if(lng == 1)
                                                num = va_arg(arg_ptr, long);
                                        else if(lng == 2)
                                                num = va_arg(arg_ptr, long long);
                                        else
                                                num = va_arg(arg_ptr, int);
                                        if(shrt)
                                                num = short(num);

                                        if(num < 0 && sign) {
                                                *pnum = '-';
                                                num = 0 - num;
                                                lltostr(pnum+1, num, base, ucase);
                                        }
                                        else
                                                lltostr(pnum, num, base, ucase);

                                        nprecnum = (prec==-1) ? strlen(pnum) : prec > (ViInt32)strlen(pnum) ? prec : strlen(pnum);
                                        if(!padright && fwidth && fwidth > strlen(pnum)) {
                                                for(ViUInt32 n=0; n < fwidth - nprecnum; n++) {
                                                        ViStatus ret = buf_put(vi, userstring, ' ');
                                                        if(ret != VI_SUCCESS)
                                                                return ret;
                                                }
                                        }

                                        for(ViInt32 i = nprecnum - strlen(pnum); i > 0; i--) {
                                                ViStatus ret = buf_put(vi, userstring, '0');
                                                if(ret != VI_SUCCESS)
                                                        return ret;
                                        }


                                        for(pnum = numbuf; *pnum; pnum++) {
                                                ViStatus ret = buf_put(vi, userstring, *pnum);
                                                if(ret != VI_SUCCESS)
                                                        return ret;
                                        }

                                        if(padright && fwidth && fwidth > strlen(numbuf)) {
                                                for(ViUInt32 n=0; n < fwidth - nprecnum; n++) {
                                                        ViStatus ret = buf_put(vi, userstring, ' ');
                                                        if(ret != VI_SUCCESS)
                                                                return ret;
                                                }
                                        }

                                        break;
                                case 'o':
                                        base = 8;
                                        goto do_num;
                                case 'f':
                                        if(lng) {
                                                long double ldnum = va_arg(arg_ptr, long double);
                                                int bufsiz;
                                                if(fwidth) {
                                                        if(prec>=0)
                                                                bufsiz = snprintf(0, 0, "%*.*Lf", (int)fwidth, (int)prec, ldnum);
                                                        else
                                                                bufsiz = snprintf(0, 0, "%*Lf", (int)fwidth, ldnum);
                                                }
                                                else bufsiz = snprintf(0, 0, "%Lf", ldnum);
                                                char buf[bufsiz];
                                                snprintf(buf, bufsiz, "%Lf", ldnum);
                                                for(pnum = buf; *pnum; pnum++) {
                                                        ViStatus ret = buf_put(vi, userstring, *pnum);
                                                        if(ret != VI_SUCCESS)
                                                                return ret;
                                                }
                                        } else {
                                                double dnum = va_arg(arg_ptr, double);
                                                int bufsiz;
                                                if(fwidth) {
                                                        if(prec>=0)
                                                                bufsiz = snprintf(0, 0, "%*.*f", (int)fwidth, (int)prec, dnum);
                                                        else
                                                                bufsiz = snprintf(0, 0, "%*f", (int)fwidth, dnum);
                                                }
                                                else bufsiz = snprintf(0, 0, "%f", dnum);
                                                char buf[bufsiz];
                                                snprintf(buf, bufsiz, "%f", dnum);
                                                for(pnum = buf; *pnum; pnum++) {
                                                        ViStatus ret = buf_put(vi, userstring, *pnum);
                                                        if(ret != VI_SUCCESS)
                                                                return ret;
                                                }
                                        }

                                        break;
                                case '.':
                                        isprec = 1;
                                        f++;
                                        goto in_fmt;
                                case '0': case '1': case '2': case '3': case '4':
                                case '5': case '6': case '7': case '8': case '9':
                                        if(isprec)
                                                prec = strtoul(f, &endptr, 10);
                                        else
                                                fwidth = strtoul(f, &endptr, 10);
                                        f = endptr;
                                        goto in_fmt;
                                case '*':
                                        if(isprec)
                                                prec = va_arg(arg_ptr, int);
                                        else
                                                fwidth = va_arg(arg_ptr, int);
                                        f++;
                                        goto in_fmt;
                                case '-':
                                        padright = 1;
                                        f++;
                                        goto in_fmt;

                                default:
                                        /// @todo lots of missing formats

                                        return VI_ERROR_NSUP_FMT;
                                }
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
