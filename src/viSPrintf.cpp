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

#include <cstdarg>

#include "visa.h"
#include "object_cache.h"
#include "session.h"

using namespace freevisa;

extern "C" ViStatus viSPrintf(ViSession vi, ViPBuf buf, ViString writeFmt, ...)
{
	try
	{
                va_list arg_ptr;
                va_start(arg_ptr, writeFmt);
                ViStatus ret = viVSPrintf(vi, buf, writeFmt, arg_ptr);
                va_end (arg_ptr);
                return ret;
        }
	catch(exception &e)
	{
		return e.code;
	}
}
