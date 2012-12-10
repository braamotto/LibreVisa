#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_session.h"

namespace freevisa {

ViStatus resource_session::GetAttribute(ViAttr attr, void *attrState)
{
        ViStatus rc = local_session::GetAttribute(attr, attrState);
        if(rc == VI_ERROR_NSUP_ATTR)
                rc = r.GetAttribute(attr, attrState);

        return rc;
}

ViStatus resource_session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        ViStatus rc = local_session::SetAttribute(attr, attrState);
        if(rc == VI_ERROR_NSUP_ATTR)
                rc = r.SetAttribute(attr, attrState);

        return rc;
}

}
