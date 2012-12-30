/* 
 * Copyright (C) 2012 Simon Richter
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

#ifndef freevisa_session_h_
#define freevisa_session_h_ 1

#include "object.h"
#include "resource.h"

namespace freevisa {

class session :
        public object
{
public:
        session(resource *);

        ~session() throw() { }

        ViStatus Close();
        ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        ViStatus Unlock();
        ViStatus GetAttribute(ViAttr, void *);
        ViStatus SetAttribute(ViAttr, ViAttrState);

        ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);
        ViStatus FindRsrc(ViString expr, ViFindList *findList, ViUInt32 *retCount, ViRsrc instrDesc);

        ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);
        ViStatus ReadSTB(ViUInt16 *);

        ViUInt32 GetFmtReadBufSiz() { return fmt_read_buf.size; }
        void SetFmtReadBufSiz(ViUInt32 i) { fmt_read_buf.size = i; }
        ViUInt32 GetFmtReadBufCnt() { return fmt_read_buf.count; }
        void SetFmtReadBufCnt(ViUInt32 i) { fmt_read_buf.count = i; }
        ViAByte GetFmtReadBuf() { return fmt_read_buf.buf; }
        void SetFmtReadBuf(ViAByte a) { fmt_read_buf.buf = a; }

        ViUInt32 GetFmtWriteBufSiz() { return fmt_write_buf.size; }
        void SetFmtWriteBufSiz(ViUInt32 i) { fmt_write_buf.size = i; }
        ViUInt32 GetFmtWriteBufCnt() { return fmt_write_buf.count; }
        void SetFmtWriteBufCnt(ViUInt32 i) { fmt_write_buf.count = i; }
        ViAByte GetFmtWriteBuf() { return fmt_write_buf.buf; }
        void SetFmtWriteBuf(ViAByte a) { fmt_write_buf.buf = a; }

        ViUInt32 GetIOInBufSiz() { return io_in_buf.size; }
        void SetIOInBufSiz(ViUInt32 i) { io_in_buf.size = i; }
        ViUInt32 GetIOInBufCnt() { return io_in_buf.count; }
        void SetIOInBufCnt(ViUInt32 i) { io_in_buf.count = i; }
        ViAByte GetIOInBuf() { return io_in_buf.buf; }
        void SetIOInBuf(ViAByte a) { io_in_buf.buf = a; }

        ViUInt32 GetIOOutBufSiz() { return io_out_buf.size; }
        void SetIOOutBufSiz(ViUInt32 i) { io_out_buf.size = i; }
        ViUInt32 GetIOOutBufCnt() { return io_out_buf.count; }
        void SetIOOutBufCnt(ViUInt32 i) { io_out_buf.count = i; }
        ViAByte GetIOOutBuf() { return io_out_buf.buf; }
        void SetIOOutBuf(ViAByte a) { io_out_buf.buf = a; }

private:
        resource *res;

        unsigned int exclusive_lock_count, shared_lock_count;

        struct buffer
        {
                buffer() : size(0), count(0), buf(0) { }

                ViUInt32 size;
                ViUInt32 count;
                ViAByte buf;
        };

        buffer fmt_read_buf;
        buffer fmt_write_buf;
        buffer io_in_buf;
        buffer io_out_buf;

        unsigned int event_queue_length;
};

}

#endif
