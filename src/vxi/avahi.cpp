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

#include "avahi.h"

#include "resource_manager.h"

#include "messagepump.h"

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>

namespace librevisa {
namespace vxi {

avahi::avahi()
{
        default_resource_manager.register_creator(*this);
}

avahi::~avahi() throw()
{
        default_resource_manager.unregister_creator(*this);
}

static void avahi_client_callback(
        AvahiClient *,
        AvahiClientState,
        void *)
{
}

static void avahi_service_browser_callback(
        AvahiServiceBrowser *,
        AvahiIfIndex,
        AvahiProtocol,
        AvahiBrowserEvent,
        const char *,
        const char *,
        const char *,
        AvahiLookupResultFlags,
        void *)
{
}

void avahi::find(findlist &list) const
{
        AvahiClient *client = avahi_client_new(main, AvahiClientFlags(0), &avahi_client_callback, 0, 0);
        AvahiServiceBrowser *browser = avahi_service_browser_new(
                client,
                AVAHI_IF_UNSPEC,
                AVAHI_PROTO_UNSPEC,
                "_lxi._tcp",
                0,
                AvahiLookupFlags(0),
                avahi_service_browser_callback,
                &list);

        main.run(5000);

        avahi_service_browser_free(browser);
        avahi_client_free(client);
}

avahi avahi::instance;

}
}
