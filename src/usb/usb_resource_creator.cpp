#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usb_resource_creator.h"

#include "util.h"

namespace freevisa {
namespace usb {

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
