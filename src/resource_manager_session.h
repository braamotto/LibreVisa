#ifndef freevisa_resource_manager_session_h_
#define freevisa_resource_manager_session_h_ 1

#include "resource_manager.h"
#include "session.h"

namespace freevisa {

class resource_manager_session :
        public session
{
public:
        explicit resource_manager_session(resource_manager &rm) : rm(rm) { }

        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);

        resource_manager &get() const { return rm; }

private:
        resource_manager &rm;
};

}

#endif
