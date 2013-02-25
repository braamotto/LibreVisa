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

#include "event_queue.h"

#include <algorithm>

#ifdef min
#undef min
#endif

namespace librevisa {

event_queue::event_queue(size_type size) :
        size(size),
        readp(0),
        writep(0),
        data(new event::data[size])
{
        return;
}

event_queue::~event_queue() throw()
{
        delete[] data;
}

void event_queue::set_size(size_type newsize)
{
        if(size == newsize)
                return;

        event::data *newdata = new event::data[newsize];

        {
                // When shrinking, discard newest events
                size_type const newcount = std::min(get_count(), newsize);

                if(readp < writep)
                {
                        std::copy(data + readp, data + readp + newcount, newdata);
                }
                else if(readp > writep)
                {
                        size_type const length1 = std::min(newcount, size - readp);
                        event::data *cursor = 
                                std::copy(data + readp, data + readp + length1, newdata);
                        if(newcount > length1)
                        {
                                size_type const length2 = newcount - length1;
                                std::copy(data, data + length2, cursor);
                        }
                }

                data = newdata;
                size = newsize;
                readp = 0;
                writep = newcount;
        }
        return;
}

}
