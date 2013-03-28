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

#include "messagepump_avahi.h"
#include "messagepump.h"

#include <sys/time.h>

extern "C" {
        static AvahiWatch *avahi_watch_new(AvahiPoll const *api, int fd, AvahiWatchEvent event, AvahiWatchCallback callback, void *userdata);
        static void avahi_watch_update(AvahiWatch *w, AvahiWatchEvent event);
        static AvahiWatchEvent avahi_watch_get_events(AvahiWatch *w);
        static void avahi_watch_free(AvahiWatch *w);
        static AvahiTimeout* avahi_timeout_new(const AvahiPoll *api, const struct timeval *tv, AvahiTimeoutCallback callback, void *userdata);
        static void avahi_timeout_update(AvahiTimeout *, const struct timeval *tv);
        static void avahi_timeout_free(AvahiTimeout *t);
}

using namespace librevisa;

struct AvahiWatch :
        public messagepump::watch
{
        AvahiWatchCallback callback;
        void *userdata;

        virtual void notify_fd_event(int fd, messagepump::fd_event event);
        virtual void cleanup() { delete this; }

        // Avoid warning
        virtual ~AvahiWatch() throw() { }
};

struct AvahiTimeout :
        public librevisa::messagepump::timeout
{
        AvahiTimeoutCallback callback;
        void *userdata;

        virtual void notify_timeout();
        virtual void cleanup() { delete this; }

        // Avoid watning
        virtual ~AvahiTimeout() throw() { }
};

inline AvahiWatchEvent &operator|=(AvahiWatchEvent &lhs, AvahiWatchEvent rhs)
{
        return (lhs = AvahiWatchEvent(unsigned(lhs) | unsigned(rhs)));
}

inline AvahiWatchEvent to_avahi(messagepump::fd_event event)
{
        AvahiWatchEvent ret = AvahiWatchEvent(0);
        if(event & main.read)
                ret |= AVAHI_WATCH_IN;
        if(event & main.write)
                ret |= AVAHI_WATCH_OUT;
        return ret;
}

inline messagepump::fd_event to_librevisa(AvahiWatchEvent event)
{
        messagepump::fd_event ret = messagepump::fd_event(0);
        if(event & AVAHI_WATCH_IN)
                ret |= main.read;
        if(event & AVAHI_WATCH_OUT)
                ret |= main.write;
        return ret;
}

void AvahiWatch::notify_fd_event(int fd, messagepump::fd_event event)
{
        callback(this, fd, to_avahi(event), userdata);
}

void AvahiTimeout::notify_timeout()
{
        callback(this, userdata);
}

static AvahiWatch *avahi_watch_new(
        AvahiPoll const *,
        int fd,
        AvahiWatchEvent event,
        AvahiWatchCallback callback,
        void *userdata)
{
        AvahiWatch *n = new AvahiWatch;
        n->fd = fd;
        n->event = to_librevisa(event);
        n->callback = callback;
        n->userdata = userdata;

        main.register_watch(*n);
        return n;
}

static void avahi_watch_update(AvahiWatch *w, AvahiWatchEvent event)
{
        main.update_watch(*w, to_librevisa(event));
}

static AvahiWatchEvent avahi_watch_get_events(AvahiWatch *w)
{
        return to_avahi(main.get_events(*w));
}

static void avahi_watch_free(AvahiWatch *w)
{
        main.unregister_watch(*w);
}

static AvahiTimeout* avahi_timeout_new(
        AvahiPoll const *,
        struct timeval const *tv,
        AvahiTimeoutCallback callback,
        void *userdata)
{
        AvahiTimeout *n = new AvahiTimeout;
        if(tv)
                n->tv = *tv;
        n->callback = callback;
        n->userdata = userdata;
        main.register_timeout(*n);
        return n;
}

static void avahi_timeout_update(AvahiTimeout *t, const struct timeval *tv)
{
        main.update_timeout(*t, tv);
}

static void avahi_timeout_free(AvahiTimeout *t)
{
        main.unregister_timeout(*t);
}

struct AvahiPoll const avahi_main =
{
        0,
        &avahi_watch_new,
        &avahi_watch_update,
        &avahi_watch_get_events,
        &avahi_watch_free,
        &avahi_timeout_new,
        &avahi_timeout_update,
        &avahi_timeout_free
};
