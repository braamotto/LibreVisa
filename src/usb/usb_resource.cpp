#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usb_resource.h"

#include "exception.h"

namespace freevisa {
namespace usb {

usb_resource::usb_resource(unsigned int /*manufacturer*/, unsigned int /*device*/, usb_string const &/*serial*/)
{
        if(openusb_init(0, &openusb) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);
}

usb_resource::~usb_resource() throw()
{
        openusb_fini(openusb);
}

ViStatus usb_resource::Close()
{
        delete this;
        return VI_SUCCESS;
}

}
}
