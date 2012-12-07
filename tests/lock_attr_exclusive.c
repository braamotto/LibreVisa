#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
	ViSession rmgr;

	ViStatus rc = viOpenDefaultRM(&rmgr);

	if(rc != VI_SUCCESS)
		return 1;

	ViAccessMode access;

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

	if(rc != VI_SUCCESS)
		return 1;

	if(access != VI_NO_LOCK)
		return 1;

	ViKeyId key;

	rc = viLock(rmgr, VI_EXCLUSIVE_LOCK, 100, VI_NULL, &key);

	if(rc != VI_SUCCESS)
		return 1;

	if(key != VI_NULL)
		return 1;

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

	if(rc != VI_SUCCESS)
		return 1;

	if(access != VI_EXCLUSIVE_LOCK)
		return 1;

	rc = viUnlock(rmgr);

	if(rc != VI_SUCCESS)
		return 1;

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

	if(rc != VI_SUCCESS)
		return 1;

	if(access != VI_NO_LOCK)
		return 1;

	return 0;
}
