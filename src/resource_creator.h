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

#ifndef librevisa_resource_creator_h_
#define librevisa_resource_creator_h_ 1

#include <string>
#include <vector>

#include "visa.h"

namespace librevisa {

class resource;
class findlist;

class resource_creator
{
public:
        virtual resource *create(std::vector<std::string> const &) const = 0;
        virtual void find(findlist &) const = 0;
};

}

#endif
