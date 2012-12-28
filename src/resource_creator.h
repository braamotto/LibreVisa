#ifndef freevisa_resource_creator_h_
#define freevisa_resource_creator_h_ 1

#include <string>
#include <vector>

#include "visa.h"

namespace freevisa {

class resource;

class resource_creator
{
public:
        virtual resource *create(std::vector<std::string> const &) const = 0;
};

}

#endif
