#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
	ViSession rmgr;

	ViStatus rc = viGetDefaultRM(&rmgr);

	if(rc != VI_SUCCESS)
		return 1;

	ViChar rmgr_rsrc[256];

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_NAME, rmgr_rsrc);

	if(rc != VI_SUCCESS)
		return 1;

	if(rmgr_rsrc[0] != '\0')
		return 1;

	return 0;
}
