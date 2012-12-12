#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_session.h"

namespace freevisa {

ViStatus resource_session::GetAttribute(ViAttr attr, void *attrState)
{
        ViStatus rc = session::GetAttribute(attr, attrState);
        if(rc == VI_ERROR_NSUP_ATTR)
                rc = r.GetAttribute(attr, attrState);

        return rc;
}

ViStatus resource_session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        ViStatus rc = session::SetAttribute(attr, attrState);
        if(rc == VI_ERROR_NSUP_ATTR)
                rc = r.SetAttribute(attr, attrState);

        return rc;
}

ViStatus resource_session::Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
{
        return r.Write(buf, count, retCount);
}

}
