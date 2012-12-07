#ifndef freevisa_object_cache_h_
#define freevisa_object_cache_h_ 1

#include "object.h"

#include "exception.h"

namespace freevisa {

class object_cache
{
public:
	object_cache() : last(1) { }
	object *get_object(ViObject) const throw(exception);

	void set(ViObject, object *) throw();

	ViObject add(object *) throw(exception);

	static unsigned int const max_objects = 1024;

private:
	object *objects[max_objects];
	unsigned int last;
};

extern object_cache objects;

}

#endif
