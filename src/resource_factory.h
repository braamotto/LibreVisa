#ifndef freevisa_resource_factory_h_
#define freevisa_resource_factory_h_ 1

#include "resource_creator.h"

#include <map>

namespace freevisa {

class resource_factory
{
public:
        resource *create(ViRsrc rsrc);

        void register_creator(resource_creator const &);
        void unregister_creator(resource_creator const &);

private:
        std::map<char const *, resource_creator const *> creators;
};

}

#endif
