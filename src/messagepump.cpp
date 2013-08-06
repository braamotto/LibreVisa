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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "messagepump.h"

#include "timeval_op.h"
#include "lock.h"

#include <sys/time.h>

namespace librevisa {

messagepump::messagepump() throw()
{
        return;
}

void messagepump::register_watch(watch &w)
{
        lock l(cs);
        watches.push_front(w);
        cv.signal();
}

void messagepump::unregister_watch(watch &w)
{
        lock l(cs);
        w.fd = -1;
        cv.signal();
}

void messagepump::update_watch(watch &w, fd_event event)
{
        lock l(cs);
        w.event = event;
        cv.signal();
}

messagepump::fd_event messagepump::get_events(watch &w)
{
        fd_event ret = none;
        if(FD_ISSET(w.fd, &readfds))
                ret |= read;
        if(FD_ISSET(w.fd, &writefds))
                ret |= write;
        if(FD_ISSET(w.fd, &exceptfds))
                ret |= except;
        return ret;
}

void messagepump::register_timeout(timeout &t)
{
        lock l(cs);
        timeouts.push_front(t);
        cv.signal();
}

void messagepump::unregister_timeout(timeout &t)
{
        lock l(cs);
        t.tv.tv_sec = -1;
        cv.signal();
}

void messagepump::update_timeout(timeout &t, timeval const *tv)
{
        lock l(cs);
        if(!tv)
                tv = &null_timeout;
        t.tv = *tv;
        cv.signal();
}

void messagepump::run(unsigned int stopafter)
{
        timeval now;
        ::gettimeofday(&now, 0);

        timeval const limit = now + stopafter * 1000;

        for(;;)
        {
                bool restart = false;
                bool have_timeout = false;
                timeval next = limit;

                {
                        lock l(cs);

                        for(timeout_iterator i = timeouts.begin(); i != timeouts.end(); ++i)
                        {
                                while(i != timeouts.end() && i->tv.tv_sec == -1)
                                {
                                        timeout &t = *i;
                                        i = timeouts.erase(i);
                                        t.cleanup();
                                }
                                if(i == timeouts.end())
                                        break;
                                if(i->tv == null_timeout)
                                        continue;
                                if(i->tv < now)
                                {
                                        i->tv = null_timeout;
                                        i->notify_timeout();
                                        restart = true;
                                        continue;
                                }
                                have_timeout = true;
                                if(i->tv < next)
                                        next = i->tv;
                        }
                }

                if(restart)
                        continue;

                if(have_timeout)
                        next -= now;

                FD_ZERO(&readfds);
                FD_ZERO(&writefds);
                FD_ZERO(&exceptfds);

                int maxfd = -1;

                {
                        lock l(cs);

                        for(watch_iterator i = watches.begin(); i != watches.end(); ++i)
                        {
                                while(i != watches.end() && i->fd == -1)
                                {
                                        watch &w = *i;
                                        i = watches.erase(i);
                                        w.cleanup();
                                };
                                if(i == watches.end())
                                        break;

                                if(i->event & read)
                                        FD_SET(i->fd, &readfds);
                                if(i->event & write)
                                        FD_SET(i->fd, &writefds);
                                if(i->event & except)
                                        FD_SET(i->fd, &exceptfds);
                                if(i->event && i->fd > maxfd)
                                        maxfd = i->fd;
                        }

                        if(!have_timeout && maxfd == -1)
                        {
                                cv.wait(cs);
                                continue;
                        }
                }

                int rc = ::select(maxfd + 1, &readfds, &writefds, &exceptfds, &next);
                if(rc == -1)
                        return;
                if(rc > 0)
                {
                        lock l(cs);

                        for(watch_iterator i = watches.begin(); i != watches.end(); ++i)
                        {
                                fd_event ev = get_events(*i);
                                if(ev)
                                        i->notify_fd_event(i->fd, ev);
                        }
                }

                ::gettimeofday(&now, 0);
                if(limit < now)
                        return;
        }
}

timeval const messagepump::null_timeout = { 0, 1000000 };

messagepump main;

}
