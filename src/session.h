#ifndef freevisa_session_h_
#define freevisa_session_h_ 1

#include "object.h"

namespace freevisa {

class session
        : public object
{
public:
        session();

        virtual ~session() throw() { }

        virtual ViStatus Close();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);

        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);

protected:
        ViUInt32 fmt_read_buf_siz;
        ViUInt32 fmt_read_buf_cnt;
        ViAByte fmt_read_buf;

        ViUInt32 fmt_write_buf_siz;
        ViUInt32 fmt_write_buf_cnt;
        ViAByte fmt_write_buf;

        ViUInt32 io_in_buf_siz;
        ViUInt32 io_in_buf_cnt;
        ViAByte io_in_buf;

        ViUInt32 io_out_buf_siz;
        ViUInt32 io_out_buf_cnt;
        ViAByte io_out_buf;

public:
        ViUInt32 GetFmtReadBufSiz() {return fmt_read_buf_siz;}
        void SetFmtReadBufSiz(ViUInt32 i) {fmt_read_buf_siz=i;}
        ViUInt32 GetFmtReadBufCnt() {return fmt_read_buf_cnt;}
        void SetFmtReadBufCnt(ViUInt32 i) {fmt_read_buf_cnt=i;}
        ViAByte GetFmtReadBuf() {return fmt_read_buf;}
        void SetFmtReadBuf(ViAByte a) {fmt_read_buf=a;}

        ViUInt32 GetFmtWriteBufSiz() {return fmt_write_buf_siz;}
        void SetFmtWriteBufSiz(ViUInt32 i) {fmt_write_buf_siz=i;}
        ViUInt32 GetFmtWriteBufCnt() {return fmt_write_buf_cnt;}
        void SetFmtWriteBufCnt(ViUInt32 i) {fmt_write_buf_cnt=i;}
        ViAByte GetFmtWriteBuf() {return fmt_write_buf;}
        void SetFmtWriteBuf(ViAByte a) {fmt_write_buf=a;}

        ViUInt32 GetIOInBufSiz() {return io_in_buf_siz;}
        void SetIOInBufSiz(ViUInt32 i) {io_in_buf_siz=i;}
        ViUInt32 GetIOInBufCnt() {return io_in_buf_cnt;}
        void SetIOInBufCnt(ViUInt32 i) {io_in_buf_cnt=i;}
        ViAByte GetIOInBuf() {return io_in_buf;}
        void SetIOInBuf(ViAByte a) {io_in_buf=a;}

        ViUInt32 GetIOOutBufSiz() {return io_out_buf_siz;}
        void SetIOOutBufSiz(ViUInt32 i) {io_out_buf_siz=i;}
        ViUInt32 GetIOOutBufCnt() {return io_out_buf_cnt;}
        void SetIOOutBufCnt(ViUInt32 i) {io_out_buf_cnt=i;}
        ViAByte GetIOOutBuf() {return io_out_buf;}
        void SetIOOutBuf(ViAByte a) {io_out_buf=a;}
};

}

#endif
