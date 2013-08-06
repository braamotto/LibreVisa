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

#include "mutex.h"

#include <cerrno>

#ifndef librevisa_condvar_pthread_h_
#define librevisa_condvar_pthread_h_ 1

namespace librevisa {

class condvar
{
public:
        condvar() { pthread_cond_init(&impl, 0); }
        ~condvar() throw() { pthread_cond_destroy(&impl); }

        void wait(mutex &cs) { pthread_cond_wait(&impl, &cs.impl); }
        bool wait(mutex &cs, timespec const &ts)
        {
                return pthread_cond_timedwait(&impl, &cs.impl, &ts) != ETIMEDOUT;
        }

        void signal()
        {
                pthread_cond_signal(&impl);
        }

private:
        pthread_cond_t impl;
};

}

#endif
