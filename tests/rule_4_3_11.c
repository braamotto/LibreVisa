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
        ViSession rm1, rm2;

        ViStatus rc = viOpenDefaultRM(&rm1);

        if(rc != VI_SUCCESS)
                return 1;

        if(rm1 == VI_NULL)
                return 1;

        rc = viOpenDefaultRM(&rm2);

        if(rc != VI_SUCCESS)
                return 1;

        if(rm2 == VI_NULL)
                return 1;

        if(rm1 == rm2)
                return 1;

        viClose(rm2);
        viClose(rm1);

        return 0;
}
