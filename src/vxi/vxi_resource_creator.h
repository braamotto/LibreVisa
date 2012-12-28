#ifndef freevisa_vxi_resource_creator_h_
#define freevisa_vxi_resource_creator_h_ 1

#include "vxi_resource.h"

#include "resource_creator.h"

namespace freevisa {
namespace vxi {

class vxi_resource::creator :
        public resource_creator
{
private:
        creator();
        ~creator() throw();

        virtual vxi_resource *create(std::vector<std::string> const &) const;

        static creator instance;
};

}
}

#endif
