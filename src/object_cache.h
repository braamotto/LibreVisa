#ifndef freevisa_object_cache_h_
#define freevisa_object_cache_h_ 1

#include "exception.h"

#include <map>

namespace freevisa {

class object;
class resource;
class resource_manager;
class session;

class object_cache
{
public:
        object *get_object(ViObject) throw(exception);
        session *get_session(ViSession) throw(exception);

        void remove(ViObject) throw(exception);

        ViSession add(resource *) throw(exception);

private:
        typedef std::map<ViSession, session> smap;
        smap sessions;

        ViObject find_id() throw(exception);
};

extern object_cache objects;

}

#endif
