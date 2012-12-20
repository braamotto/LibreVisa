#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "event_queue.h"

#include "lock.h"

#include <algorithm>

namespace freevisa {

event_queue::event_queue(size_type size) :
        size(size),
        readp(0),
        writep(0),
        data(new event::data[size])
{
        return;
}

void event_queue::set_size(size_type newsize)
{
        if(size == newsize)
                return;

        event::data *newdata = new event::data[newsize];

        {
                lock cs_lock(cs);

                // When shrinking, discard newest events
                size_type const newcount = std::min(count(), newsize);

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
