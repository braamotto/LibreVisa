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

ViStatus _VI_FUNC viFlush(ViSession vi, ViUInt16 mask)
{
        try
        {

                if(!(mask & VI_READ_BUF)  && !(mask & VI_WRITE_BUF) &&
                   !(mask & VI_IO_IN_BUF) && !(mask & VI_IO_OUT_BUF) &&
                   !(mask & VI_READ_BUF_DISCARD) &&
                   !(mask & VI_WRITE_BUF_DISCARD) &&
                   !(mask & VI_IO_IN_BUF_DISCARD) &&
                   !(mask & VI_IO_OUT_BUF_DISCARD))
                        return VI_ERROR_INV_MASK;

                session *s = objects.get_session(vi);

                /// @todo return VI_ERROR_INV_SESSION if session object invalid
                /// @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(mask & VI_READ_BUF_DISCARD) {
                        s->SetFmtReadBufCnt(0);
                }
                if(mask & VI_READ_BUF) {
                        /// @todo viRead
                        s->SetFmtReadBufCnt(0);
                }
                if(mask & VI_WRITE_BUF) {
                        ViStatus ret;
                        ViUInt32 written = 42;
                        ret = viWrite(vi, s->GetFmtWriteBuf(), s->GetFmtWriteBufCnt(), &written);
                        if(ret != VI_SUCCESS) {
                                if(ret == VI_ERROR_TMO)
                                        return ret;
                                return VI_ERROR_IO;
                        }

                        s->SetFmtWriteBufCnt(0);
                }
                if(mask & VI_WRITE_BUF_DISCARD) {
                        s->SetFmtWriteBufCnt(0);
                }
                if((mask & VI_IO_IN_BUF) || (mask & VI_IO_IN_BUF_DISCARD)) {
                        s->SetIOInBufCnt(0);
                }
                if(mask & VI_IO_OUT_BUF) {
                        ViStatus ret;
                        ViUInt32 written = 42;
                        ret = viWrite(vi, s->GetIOOutBuf(), s->GetIOOutBufCnt(), &written);
                        if(ret != VI_SUCCESS) {
                                if(ret == VI_ERROR_TMO)
                                        return ret;
                                return VI_ERROR_IO;
                        }
                        s->SetIOOutBufCnt(0);
                }
                if(mask & VI_IO_OUT_BUF_DISCARD) {
                        s->SetIOOutBufCnt(0);
                }
                return VI_SUCCESS;
        }
        catch(exception &e)
        {
                return e.code;
        }
}
