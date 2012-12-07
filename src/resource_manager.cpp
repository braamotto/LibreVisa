#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_manager.h"

namespace freevisa {

resource_manager::resource_manager() :
	refcount(0)
{
	return;
}

ViStatus resource_manager::Open()
{
	++refcount;
	return VI_SUCCESS;
}

ViStatus resource_manager::Close()
{
	--refcount;
	return VI_SUCCESS;
}

ViStatus resource_manager::GetAttribute(ViAttr attr, void *attrState)
{
	switch(attr)
	{
	case VI_ATTR_RSRC_NAME:
		*reinterpret_cast<ViRsrc *>(attrState) = "";
		return VI_SUCCESS;

	case VI_ATTR_RM_SESSION:
		*reinterpret_cast<ViSession *>(attrState) = VI_NULL;
		return VI_SUCCESS;

	default:
		return GetObjectAttribute(attr, attrState);
	}
}

ViStatus resource_manager::SetAttribute(ViAttr attr, ViAttrState attrState)
{
	return SetObjectAttribute(attr, attrState);
}

resource_manager default_resource_manager;

}
