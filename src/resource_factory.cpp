#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource_factory.h"

#include "exception.h"

namespace freevisa {

void resource_factory::register_creator(resource_creator const &cre)
{
        creators[cre.name()] = &cre;
}

resource *resource_factory::create(ViRsrc rsrcName)
{
        // @todo use viParseRsrc
        creator_iterator i = creators.find(rsrcName);
        if(i == creators.end())
                throw exception(VI_ERROR_RSRC_NFOUND);
        return i->second->create(rsrcName);
}

resource_factory resource_factory::instance;

}
