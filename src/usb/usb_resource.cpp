#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "usb_resource.h"

#include "exception.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

namespace freevisa {
namespace usb {

enum
{
        // Class requests
        INITIATE_ABORT_BULK_OUT = 1,
        CHECK_ABORT_BULK_OUT_STATUS = 2,
        INITIATE_ABORT_BULK_IN = 3,
        CHECK_ABORT_BULK_IN_STATUS = 4,
        INITIATE_CLEAR = 5,
        CHECK_CLEAR_STATUS = 6,
        GET_CAPABILITIES = 7,

        // Subclass requests
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
        have_interrupt_endpoint(true),
        rx_buf_offset(0),
        rx_buf_bytes(0)
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

        openusb_ctrl_request req =
        {
                {
                        USB_REQ_DEV_TO_HOST|USB_REQ_TYPE_CLASS|USB_REQ_RECIP_INTERFACE,
                        GET_CAPABILITIES,
                        0,
                        interface,
                },
                capabilities,
                sizeof capabilities,
                io_timeout,
                0,
                { 0, 0 },
                0
        };

        if(openusb_ctrl_xfer(dev, interface, 0, &req) != OPENUSB_SUCCESS)
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

int usb_resource::Transfer(uint8_t endpoint, uint8_t *data, int len)
{
        struct openusb_bulk_request request;
        request.payload = data;
        request.length = len;
        request.timeout = 1000;
        request.flags = 0;
        request.next = 0;

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
        else
                data[9] = 0x0a;
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

ViStatus usb_resource::Read(ViBuf payload_buf, ViUInt32 payload_buf_size, ViUInt32 *result)
{
        if (Send(REQUEST_DEV_DEP_MSG_IN, 0, 256) < 0)
                return VI_ERROR_IO;

        int endpoint = 0x82;
        uint8_t header[12];
        int header_received = 0;

        // Some or all of the next header may be already received.
        int header_bytes = MIN(rx_buf_bytes, 12);
        memcpy(header, &rx_buf[rx_buf_offset], header_bytes);
        header_received += header_bytes;
        rx_buf_offset += header_bytes;
        rx_buf_bytes -= header_bytes;

        // Make further transfers to receive rest of header.
        while (header_received < 12)
        {
                rx_buf_bytes = Transfer(endpoint, rx_buf, sizeof(rx_buf));
                if (rx_buf_bytes < 0)
                        return VI_ERROR_IO;
                int header_bytes = MIN(rx_buf_bytes, 12 - header_received);
                memcpy(&header[header_received], rx_buf, header_bytes);
                header_received += header_bytes;
                rx_buf_offset = header_bytes;
                rx_buf_bytes -= header_bytes;
        }

        // Calculate payload sizing.
        int payload_size = header[4] | (header[5] << 8) | (header[6] << 16) | (header[7] << 24);
        int payload_received = 0;
        int payload_to_copy = MIN(payload_size, (int)payload_buf_size);
        int payload_copied = 0;

        // Some or all of the payload may be already received.
        int payload_bytes = MIN(rx_buf_bytes, payload_size);
        if (payload_copied < payload_to_copy)
        {
                int copy_bytes = MIN(payload_bytes, payload_to_copy);
                memcpy(&payload_buf[payload_copied], &rx_buf[rx_buf_offset], copy_bytes);
                payload_copied += copy_bytes;
        }
        payload_received += payload_bytes;
        rx_buf_offset += payload_bytes;
        rx_buf_bytes -= payload_bytes;

        // Make further transfers to receive rest of payload.
        while (payload_received < payload_size)
        {
                rx_buf_bytes = Transfer(endpoint, rx_buf, sizeof(rx_buf));
                if (rx_buf_bytes < 0)
                        return VI_ERROR_IO;
                int payload_bytes = MIN(rx_buf_bytes, payload_size - payload_received);
                if (payload_copied < payload_to_copy)
                {
                        int copy_bytes = MIN(payload_bytes, payload_to_copy - payload_copied);
                        memcpy(&payload_buf[payload_copied], &rx_buf[rx_buf_offset], copy_bytes);
                        payload_copied += copy_bytes;
                }
                payload_received += payload_bytes;
                rx_buf_offset = payload_bytes;
                rx_buf_bytes -= payload_bytes;
        }

        *result = payload_copied;
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
