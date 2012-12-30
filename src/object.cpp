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

#include "object.h"

namespace librevisa {

object::object()
{
        return;
}

ViStatus object::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                *reinterpret_cast<ViVersion *>(attrState) = 0x00500100;
                return VI_SUCCESS;

        default:
                return VI_ERROR_NSUP_ATTR;
        }
}

ViStatus object::SetAttribute(ViAttr attr, ViAttrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_SPEC_VERSION:
                return VI_ERROR_ATTR_READONLY;

        default:
                return VI_ERROR_NSUP_ATTR;
        }
}

}
