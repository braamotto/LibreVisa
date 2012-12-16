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

        virtual ~session() throw() { }

        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);

        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);

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
};

}

#endif
