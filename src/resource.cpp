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

#include "resource.h"

namespace librevisa {

resource::resource() :
        refcount(0), exclusive_lock_holder(0)
{
        return;
}

ViStatus resource::GetAttribute(ViAttr attr, void *attrState)
{
        switch(attr)
        {
        case VI_ATTR_RSRC_LOCK_STATE:
                if(exclusive_lock_holder)
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_EXCLUSIVE_LOCK;
                else
                        *reinterpret_cast<ViAccessMode *>(attrState) = VI_NO_LOCK;
                return VI_SUCCESS;

        default:
                return object::GetAttribute(attr, attrState);
        }
}

bool resource::lock_exclusive(session const *ses)
{
        if(exclusive_lock_holder)
                return false;
        exclusive_lock_holder = ses;
        return true;
}

void resource::unlock_exclusive()
{
        exclusive_lock_holder = 0;
        return;
}

}
