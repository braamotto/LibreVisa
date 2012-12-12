#ifndef freevisa_resource_session_h_
#define freevisa_resource_session_h_ 1

#include "resource.h"
#include "session.h"

namespace freevisa {

class resource_session :
        public session
{
public:
        explicit resource_session(resource &r) : r(r) { }

        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
        
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);

        resource &get() const { return r; }

private:
        resource &r;
};

}

#endif
