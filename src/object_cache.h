/* 
 * Copyright (C) 2012 Simon Richter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
