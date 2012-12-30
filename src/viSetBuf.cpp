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

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace librevisa;

ViStatus viSetBuf(ViSession vi, ViUInt16 mask, ViUInt32 size)
{
        try
        {
                if(!(mask & VI_READ_BUF)  && !(mask & VI_WRITE_BUF) &&
                   !(mask & VI_IO_IN_BUF) && !(mask & VI_IO_OUT_BUF))
                        return VI_WARN_NSUP_BUF;

                /// @todo viFlush(vi, mask);

                session *s = objects.get_session(vi);

                /// @todo return VI_ERROR_INV_SESSION if session object invalid
                /// @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(mask & VI_READ_BUF) {
                        if(s->GetFmtReadBufSiz())
                                delete[](s->GetFmtReadBuf());
                        s->SetFmtReadBuf(new ViByte[size]);
                        s->SetFmtReadBufSiz(size);
                        s->SetFmtReadBufCnt(0);
                }
                if(mask & VI_WRITE_BUF) {
                        if(s->GetFmtWriteBufSiz())
                                delete[](s->GetFmtWriteBuf());
                        s->SetFmtWriteBuf(new ViByte[size]);
                        s->SetFmtWriteBufSiz(size);
                        s->SetFmtWriteBufCnt(0);
                }
                if(mask & VI_IO_IN_BUF) {
                        if(s->GetIOInBufSiz())
                                delete[](s->GetIOInBuf());
                        s->SetIOInBuf(new ViByte[size]);
                        s->SetIOInBufSiz(size);
                        s->SetIOInBufCnt(0);
                }
                if(mask & VI_IO_OUT_BUF) {
                        if(s->GetIOOutBufSiz())
                                delete[](s->GetIOOutBuf());
                        s->SetIOOutBuf(new ViByte[size]);
                        s->SetIOOutBufSiz(size);
                        s->SetIOOutBufCnt(0);
                }
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
