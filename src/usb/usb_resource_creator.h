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

#ifndef librevisa_usb_resource_creator_h_
#define librevisa_usb_resource_creator_h_ 1

#include "usb_resource.h"

#include "resource_creator.h"
#include "resource_manager.h"

namespace librevisa {
namespace usb {

class usb_resource::creator :
        public resource_creator
{
private:
        creator()
        {
                default_resource_manager.register_creator(*this);
        }

        ~creator() throw()
        {
                default_resource_manager.unregister_creator(*this);
        }

        virtual resource *create(std::vector<std::string> const &) const;

        static creator const instance;
};

}
}

#endif
