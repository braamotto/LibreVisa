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

#include "vxi_resource_creator.h"

#include "resource_manager.h"

#include "util.h"

namespace freevisa {
namespace vxi {

vxi_resource::creator::creator()
{
        default_resource_manager.register_creator(*this);
}

vxi_resource::creator::~creator() throw()
{
        default_resource_manager.unregister_creator(*this);
}

vxi_resource *vxi_resource::creator::create(std::vector<std::string> const &components) const
{
        if(components.size() < 2)
                return 0;

        // Expect "TCPIP"
        std::string const &transp = components[0];
        if(transp.size() < 5)
                return 0;

        if((transp[0] | 0x20) != 't' ||
                (transp[1] | 0x20) != 'c' ||
                (transp[2] | 0x20) != 'p' ||
                (transp[3] | 0x20) != 'i' ||
                (transp[4] | 0x20) != 'p')
        {
                return 0;
        }

        // Expect optional board number
        char const *cursor = transp.data() + 5;
        /*unsigned int board = */(void)parse_optional_int(cursor);

        std::string const &hostname = components[1];

        if(components.size() > 2)
        {
                // Expect "INSTR"
                char const *const type = components[2].data();

                if((type[0] | 0x20) != 'i' ||
                        (type[1] | 0x20) != 'n' ||
                        (type[2] | 0x20) != 's' ||
                        (type[3] | 0x20) != 't' ||
                        (type[4] | 0x20) != 'r')
                {
                        return 0;
                }

                cursor = type + 5;

                // Expect optional instrument instance number
                /*unsigned int instance = */(void)parse_optional_int(cursor);

        }

        if(components.size() > 3)
                return 0;

        return new vxi_resource(hostname);
}

vxi_resource::creator vxi_resource::creator::instance;

}
}
