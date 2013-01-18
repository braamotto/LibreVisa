/* 
 * Copyright (C) 2012 Simon Richter
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

#include "usb_resource_creator.h"

#include "resource_manager.h"

#include "findlist.h"

#include "util.h"
#include "exception.h"

#include <sstream>
#include <iomanip>

namespace librevisa {
namespace usb {

usb_resource::creator::creator()
{
        if(libusb_init(&libusb) != LIBUSB_SUCCESS)
                libusb = 0;
        default_resource_manager.register_creator(*this);
}

usb_resource::creator::~creator() throw()
{
        default_resource_manager.unregister_creator(*this);
        libusb_exit(libusb);
}

resource *usb_resource::creator::create(std::vector<std::string> const &components) const
{
        create_or_find_args args;
        args.create = &components;
        return create_or_find(CREATE, args);
}

void usb_resource::creator::find(findlist &list) const
{
        create_or_find_args args;
        args.find = &list;
        create_or_find(FIND, args);
}

resource *usb_resource::creator::create_or_find(
        create_or_find_mode mode,
        create_or_find_args args) const
{
        unsigned int vendor, product;
        usb_string serial;

        if(mode == CREATE)
        {
                std::vector<std::string> const &components = *args.create;
                if(components.size() < 4)
                        return 0;

                std::string const &transp = components[0];

                if(transp.size() < 3)
                        return 0;
                if((transp[0] | 0x20) != 'u' ||
                        (transp[1] | 0x20) != 's' ||
                        (transp[2] | 0x20) != 'b')
                {
                        return 0;
                }

                char const *cursor = transp.data() + 3;

                (void)parse_optional_int(cursor);

                cursor = components[1].data();
                vendor = parse_hex(cursor);

                cursor = components[2].data();
                product = parse_hex(cursor);

                serial.assign(components[3].begin(), components[3].end());
        }

        if(!libusb)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        libusb_device **devices = 0;

        ssize_t num_devices = libusb_get_device_list(libusb, &devices);

        if(num_devices < 0)
                throw exception(VI_ERROR_SYSTEM_ERROR);

        if(num_devices == 0)
                throw exception(VI_ERROR_RSRC_NFOUND);

        libusb_device_handle *dev = 0;
        interface_info info;

        for(ssize_t i = 0; !dev && i < num_devices; ++i)
        {
                bool acceptable = true;

                libusb_device_descriptor ddesc;

                if(acceptable && libusb_get_device_descriptor(devices[i], &ddesc) != LIBUSB_SUCCESS)
                        acceptable = false;

                if(mode == CREATE)
                {
                        if(acceptable && libusb_le16_to_cpu(ddesc.idVendor) != vendor)
                                acceptable = false;
                        if(acceptable && libusb_le16_to_cpu(ddesc.idProduct) != product)
                                acceptable = false;

                        usb_string dev_serial;

                        if(acceptable && !open_device_and_get_serial(devices[i], ddesc.iSerialNumber, dev, dev_serial))
                                acceptable = false;

                        if(acceptable && dev_serial != serial)
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

                                        info.configuration = cdesc->bConfigurationValue;
                                        info.interface = idesc.bInterfaceNumber;
                                        info.altsetting = idesc.bAlternateSetting;

                                        info.bulk_in_ep = bulk_in_ep;
                                        info.bulk_out_ep = bulk_out_ep;
                                        info.intr_in_ep = intr_in_ep;

                                        valid_configuration = true;

                                }
                        }

                        libusb_free_config_descriptor(cdesc);
                }

                if(!valid_configuration)
                        acceptable = false;

                if(acceptable && mode == FIND)
                {
                        usb_string dev_serial;

                        if(!open_device_and_get_serial(devices[i], ddesc.iSerialNumber, dev, dev_serial))
                                acceptable = false;

                        if(acceptable)
                        {
                                std::string dev_serial_ascii(dev_serial.begin(), dev_serial.end());

                                std::ostringstream ss;
                                ss << "USB";
                                ss << unsigned(libusb_get_bus_number(devices[i]));
                                ss << "::";
                                ss << "0x" << std::hex << std::setfill('0') << std::setw(4) << ddesc.idVendor;
                                ss << "::";
                                ss << "0x" << std::hex << std::setfill('0') << std::setw(4) << ddesc.idProduct;
                                ss << "::";
                                ss << dev_serial_ascii;

                                args.find->add(ss.str());
                        }
                }

                if((mode == FIND || !acceptable) && dev)
                {
                        libusb_close(dev);
                        dev = 0;
                }

        }

        libusb_free_device_list(devices, 1);

        if(mode == FIND)
                return 0;

        if(!dev)
                throw exception(VI_ERROR_RSRC_NFOUND);

        return new usb_resource(dev, info);
}

bool usb_resource::creator::open_device_and_get_serial(
        libusb_device *devinfo,
        uint16_t iSerialNumber,
        libusb_device_handle *&dev,
        usb_string &serial) const
{
        if(libusb_open(devinfo, &dev) != LIBUSB_SUCCESS)
                return false;

        /// @todo replace by libusb data type once they have one.
        struct string_descriptor
        {
                uint8_t bLength;
                uint8_t bDescriptorType;
                uint16_t bString[0];
        };

        /// @todo may not be portable everywhere
        union
        {
                string_descriptor str;
                unsigned char bytes[64];
        } serialno;

        int serialno_len = libusb_get_string_descriptor(
                dev,
                iSerialNumber,
                0,
                serialno.bytes,
                sizeof serialno.bytes);
        if(serialno_len < 0)
                return false;
        else if(serialno_len != serialno.str.bLength)
                return false;
        else if(serialno_len & 1)
                return false;
        serial.assign(serialno.str.bString, serialno_len/2-1);
        return true;
}

usb_resource::creator const usb_resource::creator::instance;

}
}
