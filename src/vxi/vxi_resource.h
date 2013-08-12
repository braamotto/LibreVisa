/* 
 * Copyright (C) 2012 Simon Richter
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

#ifndef librevisa_vxi_resource_h_
#define librevisa_vxi_resource_h_ 1

#include "instrument_resource.h"

#include "messagepump.h"

#include <string>
#include <queue>
#include <map>

#include <rpc/rpc.h>
#include <vxi.h>

namespace librevisa {
namespace vxi {

class vxi_resource :
        public instrument_resource,
        private messagepump::timeout
{
private:
        vxi_resource(std::string const &hostname);
        ~vxi_resource() throw();

        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus ReadSTB(ViUInt16 *);

        // messagepump::timeout
        virtual void notify_timeout();
        // watch/timeout common
        virtual void cleanup();

        struct rpc_watch_proxy;
        std::list<rpc_watch_proxy> rpc_watch_proxies;
        void update_rpc_watches();
        void notify_rpc_watch(int fd);

        // RPC
        CLIENT *client;

        Device_Link lid;

        std::string const hostname;
        u_long io_timeout;
        u_long lock_timeout;

        struct action;
        std::queue<action *> actions;
        void perform(action &);

        void do_open(action &);
        void do_readstb(action &);
        void do_read(action &);
        void do_write(action &);

        using instrument_resource::emit;
        static std::map<std::string, vxi_resource *> instances;

        class creator;

public:
        static void emit(std::string const &cookie, ViEventType event);
};

}
}

#endif
