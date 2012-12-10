#ifndef freevisa_session_factory_h_
#define freevisa_session_factory_h_ 1

#include "session_creator.h"

#include <map>

namespace freevisa {

class session_factory
{
public:
        session *create(ViRsrc rsrc);

        void register_creator(session_creator const &);
        void unregister_creator(session_creator const &);

private:
        std::map<char const *, session_creator const *> creators;
};

}

#endif
