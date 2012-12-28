#ifndef freevisa_usb_resource_creator_h_
#define freevisa_usb_resource_creator_h_ 1

#include "usb_resource.h"

#include "resource_creator.h"
#include "resource_manager.h"

namespace freevisa {
namespace usb {

class usb_resource::creator :
        public resource_creator
{
private:
        creator()
        {
                default_resource_manager.register_creator(*this);
        }

        ~creator() throw()
        {
                default_resource_manager.unregister_creator(*this);
        }

        virtual resource *create(std::vector<std::string> const &) const;

        static creator const instance;
};

}
}

#endif
