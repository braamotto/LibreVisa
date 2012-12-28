#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "vxi_resource_creator.h"

#include "resource_manager.h"

#include "util.h"

namespace freevisa {
namespace vxi {

vxi_resource::creator::creator()
{
        default_resource_manager.register_creator(*this);
}

vxi_resource::creator::~creator() throw()
{
        default_resource_manager.unregister_creator(*this);
}

vxi_resource *vxi_resource::creator::create(ViRsrc rsrc) const
{
        // Expect "TCPIP"
        if((rsrc[0] | 0x20) != 't' ||
                (rsrc[1] | 0x20) != 'c' ||
                (rsrc[2] | 0x20) != 'p' ||
                (rsrc[3] | 0x20) != 'i' ||
                (rsrc[4] | 0x20) != 'p')
        {
                return 0;
        }

        // Expect optional board number
        char const *cursor = &rsrc[5];
        /*unsigned int board = */(void)parse_optional_int(cursor);

        // Expect two colons
        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        // Expect host name
        char const *const hostname = cursor;

        while(is_valid_in_hostname(*cursor))
                ++cursor;

        char const *const hostname_end = cursor;

        // Expect two colons
        if(*cursor++ != ':')
                return 0;
        if(*cursor++ != ':')
                return 0;

        // Expect "INSTR"
        char const *const type = cursor;

        if((type[0] | 0x20) != 'i' ||
                (type[1] | 0x20) != 'n' ||
                (type[2] | 0x20) != 's' ||
                (type[3] | 0x20) != 't' ||
                (type[4] | 0x20) != 'r')
        {
                return 0;
        }

        cursor += 5;

        // Expect optional instrument instance number
        /*unsigned int instance = */(void)parse_optional_int(cursor);

        // Expect NUL
        if(*cursor != '\0')
                return 0;

        return new vxi_resource(std::string(hostname, hostname_end));
}

vxi_resource::creator vxi_resource::creator::instance;

}
}
