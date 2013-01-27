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

#include <avahi-common/watch.h>

#include <list>

namespace librevisa {

class messagepump :
        private AvahiPoll
{
public:
        messagepump() throw();

        void run(unsigned int timeout);

        operator AvahiPoll const *(void) const throw() { return this; }

        AvahiWatch* watch_new(int fd, AvahiWatchEvent event, AvahiWatchCallback callback, void *userdata);
        void watch_update(AvahiWatch *w, AvahiWatchEvent event);
        AvahiWatchEvent watch_get_events(AvahiWatch *w);
        void watch_free(AvahiWatch *w);
        AvahiTimeout *timeout_new(timeval const *tv, AvahiTimeoutCallback callback, void *userdata);
        void timeout_update(AvahiTimeout *t, timeval const *tv);
        void timeout_free(AvahiTimeout *t);

private:
        struct watch;
        std::list<watch> watches;

        struct timeout;
        std::list<timeout> timeouts;

        fd_set readfds;
        fd_set writefds;
        fd_set exceptfds;

        static timeval const null_timeout;
};

extern messagepump main;

}

#endif
