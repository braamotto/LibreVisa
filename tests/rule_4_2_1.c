#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main(int argc, char **argv)
{
	ViSession rmgr;

	ViStatus rc = viGetDefaultRM(&rmgr);

	if(rc != VI_SUCCESS)
		return 1;

	ViRsrc rmgr_rsrc;

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_NAME, &rmgr_rsrc);

	if(rc != VI_SUCCESS)
		return 1;

	if(rmgr_rsrc[0] != '\0')
		return 1;

	return 0;
}
