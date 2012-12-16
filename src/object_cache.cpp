#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "object_cache.h"

#include "session.h"

#include <cassert>

namespace freevisa {

object *object_cache::get_object(ViObject vi) throw(exception)
{
        if(vi == VI_NULL)
                throw exception(VI_WARN_NULL_OBJECT);

        {
                smap::iterator i = sessions.find(vi);
                if(i != sessions.end())
                        return &i->second;
        }

        throw exception(VI_ERROR_INV_OBJECT);
}

session *object_cache::get_session(ViSession vi) throw(exception)
{
        if(vi == VI_NULL)
                throw exception(VI_WARN_NULL_OBJECT);

        {
                smap::iterator i = sessions.find(vi);
                if(i != sessions.end())
                        return &i->second;
        }

        throw exception(VI_ERROR_INV_OBJECT);
}

void object_cache::remove(ViObject vi) throw(exception)
{
        assert(vi != VI_NULL);

        smap::iterator si = sessions.find(vi);
        if(si != sessions.end())
        {
                sessions.erase(si);
                return;
        }

        throw(VI_ERROR_INV_OBJECT);
}

ViSession object_cache::add(resource *res) throw(exception)
{
        return sessions.insert(std::make_pair(find_id(), session(res))).first->first;
}

ViObject object_cache::find_id() throw(exception)
{
        unsigned int id = 0;
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
