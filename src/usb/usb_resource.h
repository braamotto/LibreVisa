#ifndef freevisa_usb_resource_h_
#define freevisa_usb_resource_h_ 1

#include "resource.h"

#include <openusb.h>

namespace freevisa {
namespace usb {

class usb_resource :
        public resource
{
private:
        usb_resource(unsigned int, unsigned int, unsigned int);
        ~usb_resource() throw();

        virtual ViStatus Close();

        openusb_handle_t openusb;

        class creator;
};

}
}

#endif
