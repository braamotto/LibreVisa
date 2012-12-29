#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

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

        if(openusb_claim_interface(dev, 0, USB_INIT_DEFAULT) != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_RSRC_BUSY);
}

usb_resource::~usb_resource() throw()
{
        openusb_release_interface(dev, 0);
        openusb_close_device(dev);
        openusb_fini(openusb);
}

ViStatus usb_resource::Close()
{
        delete this;
        return VI_SUCCESS;
}

int usb_resource::Transfer(int endpoint, uint8_t *data, int len)
{
        struct openusb_bulk_request request;
        request.payload = data;
        request.length = len;
        request.timeout = 1000;
        request.flags = 0;
        request.next = 0;
        int interface = 0;
        int rc = openusb_bulk_xfer(dev, interface, endpoint, &request);
        if (rc < 0)
                return rc;
        else
                return request.result.transferred_bytes;
}

int usb_resource::Send(msg_id_t msg_id, uint8_t *buf, int size)
{
        int len = (12 + size + 3) & ~3;
        uint8_t data[len];
        memset(data, 0, len);
        data[0] = msg_id;
        data[1] = tag;
        data[2] = ~tag;
        tag++;
        data[4] = size & 0xFF;
        data[5] = (size >> 8) & 0xFF;
        data[6] = (size >> 16) & 0xFF;
        data[7] = (size >> 24) & 0xFF;
        data[8] = 1;
        memcpy(&data[12], buf, size);
        int endpoint = 0x01;
        int total_sent = 0;
        while (total_sent < len)
        {
                int sent = Transfer(endpoint, &data[total_sent], len - total_sent);
                if (sent < 0)
                        return sent;
                total_sent += sent;
        }
        return size;
}

int usb_resource::Receive(uint8_t *data, int size)
{
        int endpoint = 0x82;
        int total_read = 0;
        while (total_read < size)
        {
                int read = Transfer(endpoint, &data[total_read], size - total_read);
                if (read < 0)
                        return read;
                total_read += read;
        }
        return size;
}

ViStatus usb_resource::Write(ViBuf buf, ViUInt32 size, ViUInt32 *result)
{
        if (Send(DEV_DEP_MSG_OUT, buf, size) < 0)
        {
                return VI_ERROR_IO;
        }
        else
        {
                *result = size;
                return VI_SUCCESS;
        }
}

ViStatus usb_resource::Read(ViBuf buf, ViUInt32 buf_size, ViUInt32 *result)
{
        if (buf_size > 64)
                buf_size = 64;
        if (Send(REQUEST_DEV_DEP_MSG_IN, 0, 0) < 0)
                return VI_ERROR_IO;
        int len = (12 + buf_size + 3) & ~3;
        uint8_t data[len];
        if (Receive(data, 12) < 0)
                return VI_ERROR_IO;
        int size = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
        if (size > buf_size)
                size = buf_size;
        if (Receive(&data[12], size) < 0)
                return VI_ERROR_IO;
        memcpy(buf, &data[12], size);
        *result = size;
        return VI_SUCCESS;
}

}
}
