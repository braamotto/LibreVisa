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

#ifndef librevisa_event_queue_h_
#define librevisa_event_queue_h_ 1

#include "event.h"

#include "mutex.h"
#include "condvar.h"
#include "lock.h"

#include <stdint.h>

namespace librevisa {

class event_queue
{
public:
        event_queue(unsigned int size = 10);
        ~event_queue() throw();

        typedef unsigned int size_type;

        class locked :
                public lock
        {
        public:
                locked(event_queue &queue) :
                        lock(queue.cs),
                        queue(queue) { }

                size_type get_size() { return queue.get_size(); }
                void set_size(size_type size) { queue.set_size(size); }

                size_type get_count() { return queue.get_count(); }

                event::data &at(size_type index) { return queue.at(index); }

                void wait() { queue.cv.wait(queue.cs); }
                bool wait(timespec const &ts) { return queue.cv.wait(queue.cs, ts); }

        private:
                event_queue &queue;
        };

private:
        mutex cs;
        condvar cv;

        size_type get_size() { return size; }
        void set_size(size_type size);

        event::data &at(size_type index)
        {
                return data[(index + readp) % size];
        }

        size_type size;
        size_type readp;
        size_type writep;

        event::data *data;

        size_type get_count() const
        {
                return (writep>=readp)?(writep-readp):(size-readp+writep);
        }
};

}

#endif
