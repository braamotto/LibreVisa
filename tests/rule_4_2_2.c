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

        ViStatus rc = viGetDefaultRM(&rmgr);

        if(rc != VI_SUCCESS)
                return 1;

        ViSession rmgr_session;

        rc = viGetAttribute(rmgr, VI_ATTR_RM_SESSION, &rmgr_session);

        if(rc != VI_SUCCESS)
                return 1;

        if(rmgr_session != VI_NULL)
                return 1;

        return 0;
}
