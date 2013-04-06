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

#ifndef librevisa_resource_manager_h_
#define librevisa_resource_manager_h_ 1

#include "resource.h"

namespace librevisa {

class resource_creator;

class resource_manager :
        public resource
{
public:
        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);

        virtual ViStatus FindRsrc(ViString, ViFindList *, ViUInt32 *, ViRsrc);

        virtual ViStatus ParseRsrc(
                ViRsrc rsrc,
                ViUInt16 *intfType,
                ViUInt16 *intfNum,
                ViString *rsrcClass,
                ViString *unaliasedExpandedRsrcName,
                ViString *aliasIfExists);

        virtual ViStatus Close();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus ReadSTB(ViUInt16 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus WaitOnEvent(ViEventType, ViUInt32, ViPEventType, ViPEvent)
        {
                return VI_ERROR_NSUP_OPER;
        }
        resource *create(ViRsrc rsrc);

        void register_creator(resource_creator const &);
        void unregister_creator(resource_creator const &) { } /// @todo

private:
        unsigned int refcount;

        static unsigned int const num_creators = 20;
        resource_creator const *creators[num_creators];
};

extern resource_manager default_resource_manager;

}

#endif
