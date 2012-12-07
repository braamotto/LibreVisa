#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

using namespace freevisa;

ViStatus viUnlock(ViSession vi)
{
	try
	{
		return objects.get_object(vi)->Unlock();
	}
	catch(exception &e)
	{
		return e.code;
	}
}
