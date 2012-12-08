#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viFlush(ViSession vi, ViUInt16 mask)
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

                session *s = reinterpret_cast<session *>(objects.get_session(vi));

                // @todo return VI_ERROR_INV_SESSION if session object invalid
                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(mask & VI_READ_BUF_DISCARD) {
                        s->fmt_read_buf_cnt = 0;
                }
                if(mask & VI_READ_BUF) {
                        // @todo viRead
                        s->fmt_read_buf_cnt = 0;
                }
                if(mask & VI_WRITE_BUF) {
                        // @todo viWrite
                        s->fmt_write_buf_cnt = 0;
                }
                if(mask & VI_WRITE_BUF_DISCARD) {
                        s->fmt_write_buf_cnt = 0;
                }
                if((mask & VI_IO_IN_BUF) || (mask & VI_IO_IN_BUF_DISCARD)) {
                        s->io_in_buf_cnt = 0;
                }
                if(mask & VI_IO_OUT_BUF) {
                        // @todo viWrite
                        s->io_out_buf_cnt = 0;
                }
                if(mask & VI_IO_OUT_BUF_DISCARD) {
                        s->io_out_buf_cnt = 0;
                }
		return VI_SUCCESS;
	}
	catch(exception &e)
        {
		return e.code;
	}
}
