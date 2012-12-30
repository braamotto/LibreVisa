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

#include <visa.h>

int main()
{
        ViSession rmgr;

        ViStatus rc = viOpenDefaultRM(&rmgr);

        if(rc != VI_SUCCESS)
                return 1;

        ViAccessMode access;

        rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

        if(rc != VI_SUCCESS)
                return 1;

        if(access != VI_NO_LOCK)
                return 1;

        ViChar key[256];

        rc = viLock(rmgr, VI_SHARED_LOCK, 100, VI_NULL, key);

        if(rc != VI_SUCCESS)
                return 1;

        if(key == VI_NULL)
                return 1;

        rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

        if(rc != VI_SUCCESS)
                return 1;

        if(access != VI_SHARED_LOCK)
                return 1;

        rc = viUnlock(rmgr);

        if(rc != VI_SUCCESS)
                return 1;

        rc = viGetAttribute(rmgr, VI_ATTR_RSRC_LOCK_STATE, &access);

        if(rc != VI_SUCCESS)
                return 1;

        if(access != VI_NO_LOCK)
                return 1;

        return 0;
}
