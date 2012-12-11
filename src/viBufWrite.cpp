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

                // @todo return VI_ERROR_RSRC_LOCKED if object locked

                if(s->GetFmtWriteBufCnt() + count >= s->GetFmtWriteBufSiz())
                        return VI_ERROR_IO;

                memcpy(s->GetFmtWriteBuf() + s->GetFmtWriteBufCnt(), buf, count);
                s->SetFmtWriteBufCnt(s->GetFmtWriteBufCnt() + count);

                return VI_SUCCESS;
        }
        catch(exception &e)
        {
                return e.code;
        }
}
