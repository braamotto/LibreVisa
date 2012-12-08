#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "visa.h"

#include "object_cache.h"
#include "object.h"

using namespace freevisa;

ViStatus viClose(ViObject vi)
{
	try
	{
		ViStatus ret = objects.get_object(vi)->Close();

		if(ret == VI_SUCCESS)
			objects.remove(vi);

		return ret;
	}
	catch(exception &e)
	{
		return e.code;
	}
}
