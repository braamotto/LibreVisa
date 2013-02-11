/* 
 * Copyright (C) 2013 Simon Richter
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

#ifndef librevisa_findlist_h_
#define librevisa_findlist_h_ 1

#include "object.h"

#include <list>
#include <string>

namespace librevisa {

class findlist :
        public object
{
public:
        findlist() { }
        virtual ~findlist() throw() { }

        virtual ViStatus Close();

        ViStatus FindNext(ViChar rsrc[]);

        ViUInt32 size() { return resources.size(); }
        void add(std::string const &rsrc) { resources.push_back(rsrc); }

private:
        findlist(findlist const &);
        findlist &operator=(findlist const &);

        std::list<std::string> resources;
};

}

#endif
