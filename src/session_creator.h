#ifndef freevisa_session_creator_h_
#define freevisa_session_creator_h_ 1

#include "visa.h"

namespace freevisa {

class session;

class session_creator
{
public:
        virtual char const *name() const = 0;
        virtual session *create(ViRsrc) const = 0;
};

}

#endif
