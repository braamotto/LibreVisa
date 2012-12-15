#ifndef freevisa_resource_creator_h_
#define freevisa_resource_creator_h_ 1

#include "visa.h"

namespace freevisa {

class resource;

class resource_creator
{
public:
        virtual resource *create(ViRsrc) const = 0;
};

}

#endif
