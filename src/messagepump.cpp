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

#include <queue>

#include <sys/time.h>
#include <sys/signal.h>

namespace librevisa {

messagepump::messagepump() throw() :
        worker(*this)
{
        return;
}

void messagepump::register_watch(watch &w)
{
        worker.start();
        lock l(cs);
        watches.push_front(w);
        cv.signal();
        worker.kill(SIGUSR1);
}

void messagepump::unregister_watch(watch &w)
{
        lock l(cs);
        w.fd = -1;
        cv.signal();
        worker.kill(SIGUSR1);
}

void messagepump::update_watch(watch &w, fd_event event)
{
        lock l(cs);
        w.event = event;
        cv.signal();
        worker.kill(SIGUSR1);
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
        worker.start();
        lock l(cs);
        timeouts.push_front(t);
        cv.signal();
        worker.kill(SIGUSR1);
}

void messagepump::unregister_timeout(timeout &t)
{
        lock l(cs);
        t.tv.tv_sec = -1;
        cv.signal();
        worker.kill(SIGUSR1);
}

void messagepump::update_timeout(timeout &t, timeval const *tv)
{
        lock l(cs);
        if(!tv)
                tv = &null_timeout;
        t.tv = *tv;
        cv.signal();
        worker.kill(SIGUSR1);
}

void messagepump::init()
{
        struct sigaction sa;
        sa.sa_handler = &ignore;
        sa.sa_flags = 0;
        sigemptyset(&sa.sa_mask);
        sigaction(SIGUSR1, &sa, 0);

        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        pthread_sigmask(SIG_BLOCK, &mask, 0);
}

void messagepump::run()
{
        timeval now;
        ::gettimeofday(&now, 0);

        for(;;)
        {
                timeout *expired = 0;
                bool have_timeout = false;
                timeval next;

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
                                        expired = &*i;
                                        break;
                                }
                                if(!have_timeout || i->tv < next)
                                {
                                        next = i->tv;
                                        have_timeout = true;
                                }
                        }
                }

                if(expired)
                {
                        expired->notify_timeout();
                        ::gettimeofday(&now, 0);
                        continue;
                }

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

                sigset_t mask;
                sigemptyset(&mask);

                timespec next_as_ts;
                if(have_timeout)
                {
                        next_as_ts.tv_sec = next.tv_sec;
                        next_as_ts.tv_nsec = next.tv_usec * 1000;
                }

                int rc = ::pselect(maxfd + 1, &readfds, &writefds, &exceptfds, have_timeout? &next_as_ts : 0, &mask);
                if(rc == -1)
                {
                        if(errno == EINTR)
                        {
                                ::gettimeofday(&now, 0);
                                continue;
                        }
                        return;
                }

                if(rc > 0)
                {
                        std::queue<watch *> notify_list;

                        {
                                lock l(cs);

                                for(watch_iterator i = watches.begin(); i != watches.end(); ++i)
                                {
                                        fd_event ev = get_events(*i);
                                        if(ev)
                                                notify_list.push(&*i);
                                }
                        }

                        while(!notify_list.empty())
                        {
                                for(watch_iterator i = watches.begin(); i != watches.end(); ++i)
                                {
                                        if(notify_list.front() == &*i)
                                        {
                                                i->notify_fd_event(i->fd, get_events(*i));
                                                break;
                                        }
                                }
                                notify_list.pop();
                        }
                }

                ::gettimeofday(&now, 0);
        }
}

void messagepump::ignore(int)
{
        return;
}

timeval const messagepump::null_timeout = { 0, 1000000 };

messagepump main;

}
