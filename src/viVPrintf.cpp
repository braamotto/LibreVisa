/* 
 * Copyright (C) 2012 A. Pic
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

#include <cstring>
#include <cstdlib>
#include <cstdarg>

#include "visa.h"
#include "base_vprintf.h"

using namespace freevisa;

ViStatus viVPrintf(ViSession vi, ViString writeFmt, ViVAList params)
{

        va_list aq;
        va_copy(aq, params);
        ViStatus ret = base_vprintf(vi, 0, writeFmt, aq);
        va_end(aq);
        return ret;
}
