#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "session.h"

namespace freevisa {

session::session()
{
        fmt_read_buf_siz = 0;
        fmt_read_buf_cnt = 0;
        fmt_read_buf = 0;

        fmt_write_buf_siz = 0;
        fmt_write_buf_cnt = 0;
        fmt_write_buf = 0;

        io_in_buf_siz = 0;
        io_in_buf_cnt = 0;
        io_in_buf = 0;

        io_out_buf_siz = 0;
        io_out_buf_cnt = 0;
        io_out_buf = 0;

}

ViStatus session::Close()
{
        delete this;
        return VI_SUCCESS;
}

ViStatus session::Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *)
{
        return VI_ERROR_NSUP_OPER;
}

ViStatus session::GetAttribute(ViAttr attr, void *attrState)
{
        return object::GetAttribute(attr, attrState);
}

ViStatus session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        return object::SetAttribute(attr, attrState);
}

}

