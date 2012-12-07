#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <visa.h>

int main(int argc, char **argv)
{
	ViStatus rc = viClose(VI_NULL);

	if(rc != VI_WARN_NULL_OBJECT)
		return 1;

	return 0;
}
