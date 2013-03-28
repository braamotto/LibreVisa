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

#ifndef librevisa_messagepump_h_
#define librevisa_messagepump_h_ 1

#include "intrusive_list.h"

#include <sys/select.h>

namespace librevisa {

class messagepump
{
public:
        messagepump() throw();

        void run(unsigned int timeout);

        enum fd_event
        {
                none = 0,
                read = 1 << 0,
                write = 1 << 1,
                except = 1 << 2
        };

        struct watch
        {
                watch *next;
                watch *prev;

                int fd;
                fd_event event;

                virtual void notify_fd_event(int fd, fd_event event) = 0;
                virtual void cleanup() = 0;

        protected:
                ~watch() throw() { }
        };

        void register_watch(watch &);
        void unregister_watch(watch &);
        void update_watch(watch &, fd_event);
        fd_event get_events(watch &);

        struct timeout
        {
                timeout *next;
                timeout *prev;

                timeval tv;
                virtual void notify_timeout() = 0;
                virtual void cleanup() = 0;
                
        protected:
                ~timeout() throw() { }
        };

        void register_timeout(timeout &);
        void unregister_timeout(timeout &);
        void update_timeout(timeout &, timeval const *);

private:
        typedef intrusive_list<watch> watch_list;
        typedef watch_list::iterator watch_iterator;
        watch_list watches;

        typedef intrusive_list<timeout> timeout_list;
        typedef timeout_list::iterator timeout_iterator;
        timeout_list timeouts;

        fd_set readfds;
        fd_set writefds;
        fd_set exceptfds;

        static timeval const null_timeout;
};

inline messagepump::fd_event &operator|=(
        messagepump::fd_event &lhs,
        messagepump::fd_event const &rhs)
{
        return lhs = static_cast<messagepump::fd_event>(
                static_cast<unsigned int>(lhs) |
                static_cast<unsigned int>(rhs));
}

extern messagepump main;

}

#endif
