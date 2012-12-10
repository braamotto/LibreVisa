#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_factory.h"

namespace freevisa {

void resource_factory::register_creator(resource_creator const &cre)
{
        creators[cre.name()] = &cre;
}

}
