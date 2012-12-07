#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"

#include "object_cache.h"

using namespace freevisa;

ViStatus viClose(ViObject vi)
{
	try
	{
		ViStatus ret = objects.get_object(vi)->Close();

		if(ret == VI_SUCCESS)
			objects.set(vi, 0);

		return ret;
	}
	catch(exception &e)
	{
		return e.code;
	}
}
