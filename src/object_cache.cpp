#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

#include "exception.h"

#include <cassert>

namespace freevisa {

object *object_cache::get_object(ViObject vi) const throw(exception)
{
	if(vi == VI_NULL)
		throw exception(VI_WARN_NULL_OBJECT);
	
	if(vi >= max_objects)
		throw exception(VI_ERROR_INV_OBJECT);

	object *ret = objects[vi];

	if(!ret)
		throw exception(VI_ERROR_INV_OBJECT);

	return ret;
}

void object_cache::set(ViObject vi, object *obj) throw()
{
	assert(vi != VI_NULL);
	assert(vi < max_objects);

	objects[vi] = obj;

	return;
}

ViObject object_cache::add(object *obj) throw(exception)
{
	assert(obj);
	for(ViObject i = last; i < max_objects; ++i)
	{
		if(!objects[i])
		{
			set(i, obj);
			last = i;
			return i;
		}
	}
	for(ViObject i = VI_NULL + 1; i < last; ++i)
	{
		if(!objects[i])
		{
			set(i, obj);
			last = i;
			return i;
		}
	}
	throw exception(VI_ERROR_ALLOC);
}

object_cache objects;

}
