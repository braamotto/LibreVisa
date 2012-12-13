#ifndef freevisa_resource_factory_h_
#define freevisa_resource_factory_h_ 1

#include "resource_creator.h"

#include <map>

#include <cstring>

namespace freevisa {

class resource_factory
{
public:
        ~resource_factory() throw();

        resource *create(ViRsrc rsrc);

        void register_creator(resource_creator const &);
        void unregister_creator(resource_creator const &);

        static resource_factory &instance;

private:
        struct case_insensitive_less
        {
                bool operator()(char const *lhs, char const *rhs) { return strcasecmp(lhs, rhs) < 0; }
        };

        typedef std::map<char const *, resource_creator const *, case_insensitive_less> creator_map;
        typedef creator_map::iterator creator_iterator;
        typedef creator_map::const_iterator creator_const_iterator;

        creator_map creators;
};

}

#endif
