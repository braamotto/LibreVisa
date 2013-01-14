/* 
 * Copyright (C) 2012 Simon Richter
 * Copyright (C) 2012 Martin Ling
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "usb_resource.h"

#include "exception.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

namespace librevisa {
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

/// @todo replace by libusb data type once they have one.
struct string_descriptor
{
        uint8_t bLength;
        uint8_t bDescriptorType;
        uint16_t bString[0];
};

usb_resource::usb_resource(unsigned int vendor, unsigned int product, usb_string const &serial) :
        interface(0),
        intr_in_ep(0x83),
        status_tag(0),
        tag(1),
        io_timeout(1000),
        have_interrupt_endpoint(true),
        rx_buf_offset(0),
        rx_buf_bytes(0)
{
        if(libusb_init(&libusb) != LIBUSB_SUCCESS)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        libusb_device **devices = 0;

        ssize_t num_devices = libusb_get_device_list(libusb, &devices);

        if(num_devices < 0)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(num_devices == 0)
                throw exception(VI_ERROR_RSRC_NFOUND);

        libusb_device_handle *selected_device = 0;

        for(ssize_t i = 0; !selected_device && i < num_devices; ++i)
        {
                bool acceptable = true;

                libusb_device_descriptor ddesc;

                if(acceptable && libusb_get_device_descriptor(devices[i], &ddesc) != LIBUSB_SUCCESS)
                        acceptable = false;

                if(acceptable && libusb_le16_to_cpu(ddesc.idVendor) != vendor)
                        acceptable = false;
                if(acceptable && libusb_le16_to_cpu(ddesc.idProduct) != product)
                        acceptable = false;

                if(libusb_open(devices[i], &selected_device) != LIBUSB_SUCCESS)
                        acceptable = false;

                if(acceptable)
                {
                        /// @todo may not be portable everywhere
                        union
                        {
                                string_descriptor str;
                                unsigned char bytes[64];
                        } serialno;

                        int serialno_len = libusb_get_string_descriptor(
                                selected_device,
                                ddesc.iSerialNumber,
                                0,
                                serialno.bytes,
                                sizeof serialno.bytes);
                        if(serialno_len < 0)
                                acceptable = false;
                        else if(serialno_len != serialno.str.bLength)
                                acceptable = false;
                        else if((serial.size()*2+2) != unsigned(serialno_len))
                                acceptable = false;
                        else if(serial.compare(0, serial.size(), serialno.str.bString))
                                acceptable = false;
                }

                bool valid_configuration = false;

                for(uint8_t c = 0; acceptable && c < ddesc.bNumConfigurations; ++c)
                {
                        libusb_config_descriptor *cdesc;
                        if(libusb_get_config_descriptor(devices[i], c, &cdesc) != LIBUSB_SUCCESS)
                        {
                                acceptable = false;
                                break;
                        }

                        for(uint8_t in = 0; (in < cdesc->bNumInterfaces); ++in)
                        {
                                libusb_interface const &intf = cdesc->interface[in];

                                for(uint8_t as = 0; as < intf.num_altsetting; ++as)
                                {
                                        libusb_interface_descriptor const &idesc = intf.altsetting[as];
                                        if(idesc.bInterfaceClass != LIBUSB_CLASS_APPLICATION)
                                                continue;

                                        if(idesc.bInterfaceSubClass != 0x03)
                                                continue;

                                        if(idesc.bInterfaceProtocol > 0x01)
                                                continue;

                                        uint8_t bulk_in_ep = 0;
                                        uint8_t bulk_out_ep = 0;
                                        uint8_t intr_in_ep = 0;

                                        for(uint8_t ep = 0; ep < idesc.bNumEndpoints; ++ep)
                                        {
                                                libusb_endpoint_descriptor const &edesc = idesc.endpoint[ep];
                                                if((edesc.bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN)
                                                {
                                                        switch(edesc.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
                                                        {
                                                        case LIBUSB_TRANSFER_TYPE_BULK:
                                                                if(bulk_in_ep)
                                                                        acceptable = false;
                                                                else
                                                                        bulk_in_ep = edesc.bEndpointAddress;
                                                                break;
                                                        case LIBUSB_TRANSFER_TYPE_INTERRUPT:
                                                                if(intr_in_ep)
                                                                        acceptable = false;
                                                                else
                                                                        intr_in_ep = edesc.bEndpointAddress;
                                                                break;
                                                        default:
                                                                acceptable = false;
                                                        }
                                                }
                                                else
                                                {
                                                        switch(edesc.bmAttributes & LIBUSB_TRANSFER_TYPE_MASK)
                                                        {
                                                        case LIBUSB_TRANSFER_TYPE_BULK:
                                                                if(bulk_out_ep)
                                                                        acceptable = false;
                                                                else
                                                                        bulk_out_ep = edesc.bEndpointAddress;
                                                                break;
                                                        default:
                                                                acceptable = false;
                                                        }
                                                }
                                        }

                                        if(!bulk_in_ep || !bulk_out_ep)
                                                acceptable = false;

                                        this->configuration = cdesc->bConfigurationValue;
                                        this->interface = idesc.bInterfaceNumber;
                                        this->altsetting = idesc.bAlternateSetting;

                                        this->bulk_in_ep = bulk_in_ep;
                                        this->bulk_out_ep = bulk_out_ep;
                                        this->intr_in_ep = intr_in_ep;

                                        valid_configuration = true;

                                }
                        }

                        libusb_free_config_descriptor(cdesc);
                }

                if(!valid_configuration)
                        acceptable = false;

                if(!acceptable)
                {
                        libusb_close(selected_device);
                        selected_device = 0;
                }

        }

        libusb_free_device_list(devices, 1);

        dev = selected_device;

        if(libusb_claim_interface(dev, interface) != LIBUSB_SUCCESS)
                throw exception(VI_ERROR_RSRC_BUSY);

        int rc = libusb_control_transfer(
                dev,
                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                GET_CAPABILITIES,
                interface,
                0,
                capabilities,
                sizeof capabilities,
                io_timeout);
        if(rc != sizeof capabilities)
                throw exception(VI_ERROR_SYSTEM_ERROR);
}

usb_resource::~usb_resource() throw()
{
        libusb_release_interface(dev, interface);
        libusb_close(dev);
        libusb_exit(libusb);
}

ViStatus usb_resource::Close()
{
        delete this;
        return VI_SUCCESS;
}

int usb_resource::Transfer(uint8_t endpoint, uint8_t *data, int len)
{
        int actual;
        int rc = libusb_bulk_transfer(dev, endpoint, data, len, &actual, io_timeout);
        if (rc < 0)
                return rc;
        else
                return actual;
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
        if (Send(REQUEST_DEV_DEP_MSG_IN, 0, payload_buf_size) < 0)
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
                int received = Transfer(endpoint, rx_buf, sizeof(rx_buf));
                if (received < 0)
                        return VI_ERROR_IO;
                rx_buf_offset = 0;
                rx_buf_bytes = received;
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
                int received = Transfer(endpoint, rx_buf, sizeof(rx_buf));
                if (received < 0)
                        return VI_ERROR_IO;
                rx_buf_offset = 0;
                rx_buf_bytes = received;
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

        int rc = libusb_control_transfer(
                dev,
                LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_CLASS | LIBUSB_RECIPIENT_INTERFACE,
                READ_STATUS_BYTE,
                status_tag,
                interface,
                buffer,
                sizeof buffer,
                io_timeout);

        if(rc != LIBUSB_SUCCESS)
                throw exception(VI_ERROR_IO);

        if(!have_interrupt_endpoint)
        {
                *retStatus = buffer[2];
                return VI_SUCCESS;
        }

        for(;;)
        {
                unsigned char ibuffer[2];

                int actual;
                rc = libusb_interrupt_transfer(
                        dev,
                        intr_in_ep,
                        ibuffer,
                        sizeof ibuffer,
                        &actual,
                        io_timeout);

                if(rc != LIBUSB_SUCCESS)
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
