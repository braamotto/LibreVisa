#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

using namespace freevisa;

ViStatus viGetAttribute(ViObject vi, ViAttr attribute, void *attrState)
{
	try
	{
		return objects.get_object(vi)->GetAttribute(attribute, attrState);
	}
	catch(exception &e)
	{
		return e.code;
	}
}
