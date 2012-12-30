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

#ifndef freevisa_vxi_resource_h_
#define freevisa_vxi_resource_h_ 1

#include "resource.h"

#include <string>

#include <rpc/rpc.h>
#include <vxi.h>

namespace freevisa {
namespace vxi {

class vxi_resource :
        public resource
{
private:
        vxi_resource(std::string const &hostname);

        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus ReadSTB(ViUInt16 *);

        // RPC
        CLIENT *client;

        Device_Link lid;

        u_long io_timeout;
        u_long lock_timeout;

        class creator;
};

}
}

#endif
