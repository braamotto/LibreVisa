#ifndef freevisa_resource_h_
#define freevisa_resource_h_ 1

#include "object.h"

namespace freevisa {

class resource :
        public object
{
public:
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *) = 0;
};

}

#endif