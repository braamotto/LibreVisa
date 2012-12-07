#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "session_factory.h"

namespace freevisa {

void session_factory::register_creator(session_creator const &cre)
{
	creators[cre.name()] = &cre;
}

}
