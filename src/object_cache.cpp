#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

#include "resource_manager.h"

#include <cassert>

namespace freevisa {

object *object_cache::get_object(ViObject vi) const throw(exception)
{
	if(vi == VI_NULL)
		throw exception(VI_WARN_NULL_OBJECT);

	{
		rmmap::const_iterator i = resource_managers.find(vi);
		if(i != resource_managers.end())
			return i->second;
	}

	{
		smap::const_iterator i = sessions.find(vi);
		if(i != sessions.end())
			return i->second;
	}

	throw exception(VI_ERROR_INV_OBJECT);
}

void object_cache::remove(ViObject vi) throw(exception)
{
	assert(vi != VI_NULL);

	rmmap::iterator ri = resource_managers.find(vi);
	if(ri != resource_managers.end())
	{
		resource_managers.erase(ri);
		return;
	}

	smap::iterator si = sessions.find(vi);
	if(si != sessions.end())
	{
		sessions.erase(si);
		return;
	}

	throw(VI_ERROR_INV_OBJECT);
}

ViSession object_cache::add(resource_manager *obj) throw(exception)
{
	assert(obj);
	return resource_managers.insert(std::make_pair(find_id(), obj)).first->first;
}

ViObject object_cache::find_id() throw(exception)
{
	unsigned int id = 0;
	if(!resource_managers.empty())
	{
		unsigned int maxrm = resource_managers.rbegin()->first;
		if(maxrm > id)
			id = maxrm;
	}
	if(!sessions.empty())
	{
		unsigned int maxses = sessions.rbegin()->first;
		if(maxses > id)
			id = maxses;
	}
	++id;
	if(!id)
		throw exception(VI_ERROR_ALLOC);
	
	return id;
}

object_cache objects;

}
