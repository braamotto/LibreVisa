#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "usb_resource.h"

#include "exception.h"

namespace freevisa {
namespace usb {

enum
{
        READ_STATUS_BYTE = 128,
        REN_CONTROL = 160,
        GO_TO_LOCAL = 161,
        LOCAL_LOCKOUT = 162
};

usb_resource::usb_resource(unsigned int vendor, unsigned int product, usb_string const &serial) :
        interface(0),
        int_in_ep(0x83),
        status_tag(0),
        io_timeout(1000),
        have_interrupt_endpoint(true)
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

                if((serial.size()*2+2) != devdata->serialnumber->bLength)
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
        if(openusb_claim_interface(dev, interface, USB_INIT_DEFAULT) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);
}

usb_resource::~usb_resource() throw()
{
        openusb_release_interface(dev, interface);
        openusb_close_device(dev);
        openusb_fini(openusb);
}

ViStatus usb_resource::Close()
{
        delete this;
        return VI_SUCCESS;
}

ViStatus usb_resource::ReadSTB(ViUInt16 *retStatus)
{
        unsigned char buffer[3];

        openusb_ctrl_request creq =
        {
                {
                        USB_REQ_DEV_TO_HOST|USB_REQ_TYPE_CLASS|USB_REQ_RECIP_INTERFACE,
                        READ_STATUS_BYTE,
                        openusb_cpu_to_le16(status_tag),
                        openusb_cpu_to_le16(interface),
                },
                buffer,
                sizeof buffer,
                io_timeout,
                0,
                { 0, 0 },
                0
        };

        status_tag = (status_tag + 1) & 0x7f;

        uint32_t rc = openusb_ctrl_xfer(dev, 0, 0, &creq);

        if(rc != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_IO);

        if(!have_interrupt_endpoint)
        {
                *retStatus = buffer[2];
                return VI_SUCCESS;
        }

        unsigned char ibuffer[2];

        openusb_intr_request ireq =
        {
                128,
                ibuffer,
                sizeof ibuffer,
                io_timeout,
                0,
                { 0, 0 },
                0
        };

        rc = openusb_intr_xfer(dev, interface, int_in_ep, &ireq);

        if(rc != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_IO);

        *retStatus = ibuffer[1];
        return VI_SUCCESS;
}

}
}
