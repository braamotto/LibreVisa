#ifndef freevisa_mutex_h_
#define freevisa_mutex_h_ 1

#include <pthread.h>

namespace freevisa {

class mutex
{
public:
        mutex() throw() { pthread_mutex_init(&impl, 0); }
        ~mutex() throw() { pthread_mutex_destroy(&impl); }

        void lock() throw() { pthread_mutex_lock(&impl); }
        void unlock() throw() { pthread_mutex_unlock(&impl); }

private:
        pthread_mutex_t impl;
};

}

#endif
