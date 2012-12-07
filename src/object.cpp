#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object.h"

namespace freevisa {

object::object() : lockState(VI_NO_LOCK)
{
	return;
}

ViStatus object::Lock(ViAccessMode accessMode, ViUInt32 timeout, ViKeyId requestedKey, ViKeyId accessKey)
{
	if(accessMode != VI_SHARED_LOCK && accessMode != VI_EXCLUSIVE_LOCK)
		return VI_ERROR_INV_ACC_MODE;

	// @todo

	return VI_SUCCESS;
}

ViStatus object::Unlock()
{
	// @todo

	return VI_SUCCESS;
}

ViStatus object::GetObjectAttribute(ViAttr attr, void *attrState)
{
	switch(attr)
	{
	case VI_ATTR_RSRC_SPEC_VERSION:
		*reinterpret_cast<ViVersion *>(attrState) = 0x00500100;
		return VI_SUCCESS;

	case VI_ATTR_RSRC_LOCK_STATE:
		*reinterpret_cast<ViAccessMode *>(attrState) = lockState;
		return VI_SUCCESS;

	default:
		return VI_ERROR_NSUP_ATTR;
	}
}

ViStatus object::SetObjectAttribute(ViAttr attr, ViAttrState)
{
	switch(attr)
	{
	case VI_ATTR_RSRC_SPEC_VERSION:
	case VI_ATTR_RSRC_LOCK_STATE:
		return VI_ERROR_ATTR_READONLY;
	
	default:
		return VI_ERROR_NSUP_ATTR;
	}
}

}
