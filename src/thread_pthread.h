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

#ifndef librevisa_thread_pthread_h_
#define librevisa_thread_pthread_h_ 1

#include "mutex.h"
#include "lock.h"

#include <pthread.h>

extern "C" void *thread_func(void *arg);

namespace librevisa {

class thread
{
public:
        class runnable
        {
        public:
                virtual void run() = 0;
        };

        thread(runnable &r) : sui(r) { }

        void start()
        {
                lock locked(sui.cs);
                if(sui.running)
                        return;

                pthread_create(&handle, 0, &thread_func, &sui);
        }

private:
        struct startupinfo
        {
                startupinfo(runnable &r) : r(r), running(false) { }

                runnable &r;
                mutex cs;
                bool running;
        };

        startupinfo sui;

        pthread_t handle;

        friend void *::thread_func(void *);
};

}

#endif
