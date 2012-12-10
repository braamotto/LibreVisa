#ifndef freevisa_resource_session_h_
#define freevisa_resource_session_h_ 1

#include "resource.h"
#include "local_session.h"

namespace freevisa {

class resource_session :
        public local_session
{
public:
        explicit resource_session(resource &r) : r(r) { }

        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

        resource &get() const { return r; }

private:
        resource &r;
};

}

#endif
