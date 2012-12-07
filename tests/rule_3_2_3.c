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

	ViVersion version;

	rc = viGetAttribute(rmgr, VI_ATTR_RSRC_SPEC_VERSION, &version);

	if(rc != VI_SUCCESS)
		return 1;

	if(version != VI_SPEC_VERSION)
		return 1;

	return 0;
}
