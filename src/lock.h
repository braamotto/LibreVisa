#ifndef freevisa_lock_h_
#define freevisa_lock_h_ 1

#include "mutex.h"

namespace freevisa {

class lock
{
public:
        lock(mutex &m) throw() : m(m) { m.lock(); }
        ~lock() throw() { m.unlock(); }

private:
        mutex &m;
};

}

#endif
