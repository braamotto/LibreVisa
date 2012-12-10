#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dummy.h"

#include "resource.h"
#include "resource_creator.h"
#include "resource_factory.h"

namespace {

using namespace freevisa;

class dummy_resource :
        public resource
{
public:
        virtual ViStatus Close() { return VI_SUCCESS; }
};

class dummy_creator :
        public resource_creator
{
public:
        virtual char const *name() const { return "DUMMY"; }
        virtual resource *create(ViRsrc) const { return new dummy_resource; }
        static dummy_creator instance;
};

dummy_creator dummy_creator::instance;

}

void using_dummy_resource()
{
        resource_factory::instance.register_creator(dummy_creator::instance);
}
