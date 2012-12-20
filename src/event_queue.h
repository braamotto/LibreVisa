#ifndef freevisa_event_queue_h_
#define freevisa_event_queue_h_ 1

#include "event.h"

#include "mutex.h"

namespace freevisa {

class event_queue
{
public:
        event_queue(unsigned int size = 10);

        typedef unsigned int size_type;

        size_type get_size() { return size; }
        void set_size(size_type size);

private:
        mutex cs;

        size_type size;
        size_type readp;
        size_type writep;

        event::data *data;

        size_type count() const
        {
                return (writep>readp)?(writep-readp):(size-readp+writep);
        }
};

}

#endif
