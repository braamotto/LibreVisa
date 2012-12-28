#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "resource.h"
#include "resource_creator.h"
#include "resource_manager.h"

#include "util.h"

#include "exception.h"

#include <openusb.h>

namespace freevisa {
namespace {

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

usb_resource::usb_resource(unsigned int /*manufacturer*/, unsigned int /*device*/, unsigned int /*serial*/)
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

        virtual resource *create(ViRsrc) const;

        static creator const instance;
};

resource *usb_resource::creator::create(ViRsrc rsrc) const
{
        if((rsrc[0] | 0x20) != 'u' ||
                (rsrc[1] | 0x20) != 's' ||
                (rsrc[2] | 0x20) != 'b')
        {
                return 0;
        }

        char const *cursor = rsrc + 3;

        (void)parse_optional_int(cursor);

        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        unsigned int manufacturer = parse_hex(cursor);

        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        unsigned int device = parse_hex(cursor);

        if(*cursor != ':')
                return 0;
        if(*cursor != ':')
                return 0;

        unsigned int serial = parse_hex(cursor);

        if(*cursor != ':')
                return 0;
        if(*cursor != ':')
                return 0;

        return new usb_resource(manufacturer, device, serial);
}

usb_resource::creator const usb_resource::creator::instance;

}
}
