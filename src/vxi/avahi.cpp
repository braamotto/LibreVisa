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
#include "findlist.h"

#include "messagepump_avahi.h"
#include "messagepump.h"

#include <sstream>
#include <algorithm>

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>

struct find_context
{
        librevisa::findlist *list;
        AvahiClient *client;
        std::list<AvahiServiceBrowser *> browsers;
        std::list<AvahiServiceResolver *> resolvers;
};

extern "C" {
static void avahi_client_callback(AvahiClient *, AvahiClientState, void *);
static void avahi_service_browser_callback(AvahiServiceBrowser *, AvahiIfIndex, AvahiProtocol, AvahiBrowserEvent, const char *, const char *, const char *, AvahiLookupResultFlags, void *);
static void avahi_service_resolver_callback(AvahiServiceResolver *r, AvahiIfIndex interface, AvahiProtocol protocol, AvahiResolverEvent event, const char *name, const char *type, const char *domain, const char *host_name, const AvahiAddress *a, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags, void *userdata);
}

static void avahi_client_callback(
        AvahiClient *client,
        AvahiClientState state,
        void *userdata)
{
        find_context *ctx = reinterpret_cast<find_context *>(userdata);
        switch(state)
        {
        case AVAHI_CLIENT_S_REGISTERING:
        case AVAHI_CLIENT_S_RUNNING:
        case AVAHI_CLIENT_S_COLLISION:
                ctx->browsers.push_back(
                        avahi_service_browser_new(
                                client,
                                AVAHI_IF_UNSPEC,
                                AVAHI_PROTO_UNSPEC,
                                "_vxi-11._tcp",
                                0,
                                AvahiLookupFlags(0),
                                &avahi_service_browser_callback,
                                userdata));
                break;
        case AVAHI_CLIENT_FAILURE:
        case AVAHI_CLIENT_CONNECTING:
                break;
        }
}

static void avahi_service_browser_callback(
        AvahiServiceBrowser *browser,
        AvahiIfIndex interface,
        AvahiProtocol protocol,
        AvahiBrowserEvent event,
        const char *name,
        const char *type,
        const char *domain,
        AvahiLookupResultFlags,
        void *userdata)
{
        find_context *ctx = reinterpret_cast<find_context *>(userdata);
        switch(event)
        {
        case AVAHI_BROWSER_NEW:
                ctx->resolvers.push_back(
                        avahi_service_resolver_new(
                                ctx->client,
                                interface,
                                protocol,
                                name,
                                type,
                                domain,
                                AVAHI_PROTO_UNSPEC,
                                AvahiLookupFlags(0),
                                &avahi_service_resolver_callback,
                                userdata));
                break;
        case AVAHI_BROWSER_REMOVE:
                break;
        case AVAHI_BROWSER_CACHE_EXHAUSTED:
        case AVAHI_BROWSER_ALL_FOR_NOW:
        case AVAHI_BROWSER_FAILURE:
                avahi_service_browser_free(browser);
                ctx->browsers.remove(browser);
                if(ctx->browsers.empty() && ctx->resolvers.empty())
                {
                        avahi_client_free(ctx->client);
                        ctx->client = 0;
                }
                break;
        }
}

static void avahi_service_resolver_callback(
        AvahiServiceResolver *r,
        AvahiIfIndex interface,
        AvahiProtocol,
        AvahiResolverEvent event,
        const char *,
        const char *,
        const char *,
        const char *,
        const AvahiAddress *a,
        uint16_t,
        AvahiStringList *,
        AvahiLookupResultFlags,
        void *userdata)
{
        find_context *ctx = reinterpret_cast<find_context *>(userdata);
        switch(event)
        {
        case AVAHI_RESOLVER_FAILURE:
                break;

        case AVAHI_RESOLVER_FOUND:
                std::ostringstream os;
                os << "TCPIP" << interface << "::";
                char buffer[AVAHI_ADDRESS_STR_MAX];
                avahi_address_snprint(buffer, sizeof buffer, a);
                os << buffer << "::INSTR";
                ctx->list->add(os.str().c_str());
                break;
        }

        avahi_service_resolver_free(r);
        ctx->resolvers.remove(r);
        if(ctx->browsers.empty() && ctx->resolvers.empty())
        {
                avahi_client_free(ctx->client);
                ctx->client = 0;
        }
}

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

void avahi::find(findlist &list) const
{
        find_context ctx;
        ctx.list = &list;
        ctx.client = avahi_client_new(&avahi_main, AvahiClientFlags(0), &avahi_client_callback, &ctx, 0);
        if(!ctx.client)
                return;


        //std::for_each(ctx.resolvers.begin(), ctx.resolvers.end(), &avahi_service_resolver_free);
        std::for_each(ctx.browsers.begin(), ctx.browsers.end(), &avahi_service_browser_free);
        if(ctx.client)
                avahi_client_free(ctx.client);
}

avahi avahi::instance;

}
}
