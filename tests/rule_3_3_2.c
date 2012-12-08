#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main()
{
	ViStatus rc = viClose(VI_NULL);

	if(rc != VI_WARN_NULL_OBJECT)
		return 1;

	return 0;
}
