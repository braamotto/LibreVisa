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

#ifndef librevisa_usb_resource_h_
#define librevisa_usb_resource_h_ 1

#include "resource.h"

#include <libusb.h>

#include "usb_string.h"

namespace librevisa {
namespace usb {

class usb_resource :
        public resource
{
private:
        usb_resource(unsigned int, unsigned int, usb_string const &);
        ~usb_resource() throw();

        typedef enum {
                DEV_DEP_MSG_OUT = 1,
                REQUEST_DEV_DEP_MSG_IN = 2
        } msg_id_t;

        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Close();
        virtual ViStatus ReadSTB(ViUInt16 *);

        int Transfer(uint8_t, uint8_t *, int);
        int Send(msg_id_t, uint8_t *, int);
        int Receive(uint8_t *, int);

        libusb_context *libusb;
        libusb_device_handle *dev;

        uint8_t interface;

        uint8_t bulk_in_ep;
        uint8_t bulk_out_ep;
        uint8_t intr_in_ep;

        uint8_t status_tag;
        uint8_t tag;
        uint8_t capabilities[24];
        unsigned int io_timeout;
        bool have_interrupt_endpoint;
        uint8_t rx_buf[64];
        unsigned int rx_buf_offset;
        int rx_buf_bytes;

        class creator;
};

}
}

#endif
