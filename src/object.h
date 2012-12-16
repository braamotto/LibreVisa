#ifndef freevisa_object_h_
#define freevisa_object_h_ 1

#include "visa.h"

namespace freevisa {

class object
{
public:
        object();

        virtual ~object() throw() { }

        virtual ViStatus Close() = 0;
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);
};

}

#endif
