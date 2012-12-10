#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

ViStatus viBufWrite(ViSession vi, ViBuf buf, ViUInt32 count, ViUInt32 &/*retCount*/)
{        
	try
	{
                session *s = reinterpret_cast<session *>(objects.get_object(vi));

                // @todo return VI_ERROR_INV_SESSION if session object invalid
                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(s->fmt_write_buf_cnt + count >= s->fmt_write_buf_siz)
                        return VI_ERROR_IO;

                memcpy(s->fmt_write_buf + s->fmt_write_buf_cnt, buf, count);
                s->fmt_write_buf_cnt += count;

		return VI_SUCCESS;
	}
	catch(exception &e)
	{
		return e.code;
	}
}
