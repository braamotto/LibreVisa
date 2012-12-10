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

	virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId) = 0;
	virtual ViStatus Unlock() = 0;

        virtual ViStatus Close();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

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
};

}

#endif
