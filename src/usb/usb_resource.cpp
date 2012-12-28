#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usb_resource.h"

#include "exception.h"

namespace freevisa {
namespace usb {

usb_resource::usb_resource(unsigned int vendor, unsigned int product, usb_string const &serial)
{
        if(openusb_init(0, &openusb) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        openusb_devid_t *devids = 0;
        uint32_t num_devids = 0;

        if(openusb_get_devids_by_vendor(openusb, vendor, product, &devids, &num_devids) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(!num_devids)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        openusb_devid_t devid;
        bool found = false;

        for(uint32_t i = 0; !found && i < num_devids; ++i)
        {
                openusb_dev_data_t *devdata;

                if(openusb_get_device_data(openusb, devids[i], 0, &devdata) != OPENUSB_SUCCESS)
                        continue;

                found = true;

                if(serial.size() != devdata->serialnumber->bLength)
                        found = false;

                if(found && serial.compare(devdata->serialnumber->bString))
                        found = false;

                if(found)
                        devid = devids[i];

                openusb_free_device_data(devdata);
        }

        openusb_free_devid_list(devids);

        if(openusb_open_device(openusb, devid, USB_INIT_DEFAULT, &dev) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);
}

usb_resource::~usb_resource() throw()
{
        openusb_close_device(dev);
        openusb_fini(openusb);
}

ViStatus usb_resource::Close()
{
        delete this;
        return VI_SUCCESS;
}

}
}
