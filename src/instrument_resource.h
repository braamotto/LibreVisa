/* 
 * Copyright (C) 2013 Simon Richter
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

#ifndef librevisa_instrument_resource_h_
#define librevisa_instrument_resource_h_ 1

#include "resource.h"
#include "event_queue.h"

namespace librevisa {

class instrument_resource :
        public resource,
        protected event_queue
{
public:
        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus FindRsrc(ViString, ViFindList *, ViUInt32 *, ViRsrc)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus ParseRsrc(ViRsrc, ViUInt16 *, ViUInt16 *, ViString *, ViString *, ViString *)
        {
                return VI_ERROR_NSUP_OPER;
        }
};

}

#endif
