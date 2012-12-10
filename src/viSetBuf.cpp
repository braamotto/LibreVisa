#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viSetBuf(ViSession vi, ViUInt16 mask, ViUInt32 size)
{
        try
        {
                if(!(mask & VI_READ_BUF)  && !(mask & VI_WRITE_BUF) &&
                   !(mask & VI_IO_IN_BUF) && !(mask & VI_IO_OUT_BUF))
                        return VI_WARN_NSUP_BUF;

                // @todo viFlush(vi, mask);

                session *s = reinterpret_cast<session *>(objects.get_object(vi));

                // @todo return VI_ERROR_INV_SESSION if session object invalid
                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(mask & VI_READ_BUF) {
                        if(s->fmt_read_buf_siz)
                                delete[](s->fmt_read_buf);
                        s->fmt_read_buf = new ViByte[size];
                        s->fmt_read_buf_siz = size;
                        s->fmt_read_buf_cnt = 0;
                }
                if(mask & VI_WRITE_BUF) {
                        if(s->fmt_write_buf_siz)
                                delete[](s->fmt_write_buf);
                        s->fmt_write_buf = new ViByte[size];
                        s->fmt_write_buf_siz = size;
                        s->fmt_write_buf_cnt = 0;
                }
                if(mask & VI_IO_IN_BUF) {
                        if(s->io_in_buf_siz)
                                delete[](s->io_in_buf);
                        s->io_in_buf = new ViByte[size];
                        s->io_in_buf_siz = size;
                        s->io_in_buf_cnt = 0;
                }
                if(mask & VI_IO_OUT_BUF) {
                        if(s->io_out_buf_siz)
                                delete[](s->io_out_buf);
                        s->io_out_buf = new ViByte[size];
                        s->io_out_buf_siz = size;
                        s->io_out_buf_cnt = 0;
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
