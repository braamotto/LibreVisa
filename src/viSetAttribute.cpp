#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

using namespace freevisa;

ViStatus viSetAttribute(ViObject vi, ViAttr attribute, ViAttrState attrState)
{
	try
	{
		return objects.get_object(vi)->SetAttribute(attribute, attrState);
	}
	catch(exception &e)
	{
		return e.code;
	}
}
