#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
	ViSession rm1, rm2;

	ViStatus rc = viOpenDefaultRM(&rm1);

	if(rc != VI_SUCCESS)
		return 1;
	
	if(rm1 == VI_NULL)
		return 1;

	rc = viOpenDefaultRM(&rm2);
	
	if(rc != VI_SUCCESS)
		return 1;

	if(rm2 == VI_NULL)
		return 1;

	if(rm1 == rm2)
		return 1;
	
	viClose(rm2);
	viClose(rm1);

	return 0;
}
