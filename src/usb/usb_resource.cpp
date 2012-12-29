#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

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
        tag(1),
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
                throw exception(VI_ERROR_RSRC_BUSY);
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
        int len = buf?((12 + size + 3) & ~3):12;
        uint8_t data[len];
        memset(data, 0, len);
        data[0] = msg_id;
        data[1] = tag;
        data[2] = ~tag;
        if(!++tag)
                ++tag;
        data[4] = size & 0xFF;
        data[5] = (size >> 8) & 0xFF;
        data[6] = (size >> 16) & 0xFF;
        data[7] = (size >> 24) & 0xFF;
        if(msg_id == DEV_DEP_MSG_OUT)
                data[8] = 1;
        if(buf)
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
        if (Send(REQUEST_DEV_DEP_MSG_IN, 0, buf_size) < 0)
                return VI_ERROR_IO;
        int len = (12 + buf_size + 3) & ~3;
        uint8_t data[len];
        if (Receive(data, 12) < 0)
                return VI_ERROR_IO;
        uint32_t size = data[4] | (data[5] << 8) | (data[6] << 16) | (data[7] << 24);
        if (size > buf_size)
                size = buf_size;
        if (Receive(&data[12], size) < 0)
                return VI_ERROR_IO;
        memcpy(buf, &data[12], size);
        *result = size;
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
                        status_tag,
                        interface,
                },
                buffer,
                sizeof buffer,
                io_timeout,
                0,
                { 0, 0 },
                0
        };

        uint32_t rc = openusb_ctrl_xfer(dev, 0, 0, &creq);

        if(rc != OPENUSB_SUCCESS)
                throw exception(VI_ERROR_IO);

        if(!have_interrupt_endpoint)
        {
                *retStatus = buffer[2];
                return VI_SUCCESS;
        }

        for(;;)
        {
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
                        return VI_ERROR_IO;

                if(ibuffer[0] == (status_tag | 0x80))
                {
                        *retStatus = ibuffer[1];
                        break;
                }
        }

        status_tag = (status_tag + 1) & 0x7f;

        return VI_SUCCESS;
}

}
}
