#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "local_session.h"

namespace freevisa {

ViStatus local_session::Close()
{
	// @todo

	return VI_SUCCESS;
}

ViStatus local_session::Lock(ViAccessMode accessMode, ViUInt32, ViKeyId, ViKeyId)
{
	if(accessMode != VI_SHARED_LOCK && accessMode != VI_EXCLUSIVE_LOCK)
		return VI_ERROR_INV_ACC_MODE;

	// @todo

	return VI_SUCCESS;
}

ViStatus local_session::Unlock()
{
	// @todo

	return VI_SUCCESS;
}


ViStatus local_session::GetAttribute(ViAttr attr, void *attrState)
{
	switch(attr)
	{
	case VI_ATTR_RSRC_LOCK_STATE:
		if(exclusive_lock_count)
			*reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
		else if(shared_lock_count)
			*reinterpret_cast<ViAccessMode *>(attrState) = VI_SHARED_LOCK;
		else
			*reinterpret_cast<ViAccessMode *>(attrState) = VI_NO_LOCK;
		return VI_SUCCESS;
	
	default:
		return session::GetAttribute(attr, attrState);
	}
}

ViStatus local_session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
	switch(attr)
	{
	case VI_ATTR_RSRC_LOCK_STATE:
		return VI_ERROR_ATTR_READONLY;

	default:
		return session::SetAttribute(attr, attrState);
	}
}

}
