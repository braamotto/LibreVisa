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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usb_resource_creator.h"

#include "util.h"

namespace freevisa {
namespace usb {

resource *usb_resource::creator::create(std::vector<std::string> const &components) const
{
        if(components.size() < 4)
                return 0;

        std::string const &transp = components[0];

        if(transp.size() < 3)
                return 0;
        if((transp[0] | 0x20) != 'u' ||
                (transp[1] | 0x20) != 's' ||
                (transp[2] | 0x20) != 'b')
        {
                return 0;
        }

        char const *cursor = transp.data() + 3;

        (void)parse_optional_int(cursor);

        cursor = components[1].data();
        unsigned int manufacturer = parse_hex(cursor);

        cursor = components[2].data();
        unsigned int device = parse_hex(cursor);

        usb_string serial(components[3].begin(), components[3].end());

        return new usb_resource(manufacturer, device, serial);
}

usb_resource::creator const usb_resource::creator::instance;

}
}
