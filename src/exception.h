#ifndef freevisa_exception_h_
#define freevisa_exception_h_ 1

#include "visa.h"

namespace freevisa {

class exception
{
public:
        exception(ViStatus code) : code(code) { }

        ViStatus const code;
};

}

#endif
