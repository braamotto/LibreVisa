#ifndef freevisa_usb_resource_h_
#define freevisa_usb_resource_h_ 1

#include "resource.h"

#include <openusb.h>

#include "usb_string.h"

namespace freevisa {
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

        virtual int Transfer(int, uint8_t *, int);
        virtual int Send(msg_id_t, uint8_t *, int);
        virtual int Receive(uint8_t *, int);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *);
        virtual ViStatus Close();
        virtual ViStatus ReadSTB(ViUInt16 *);

        openusb_handle_t openusb;
        openusb_dev_handle_t dev;
        uint8_t interface;
        uint8_t int_in_ep;
        uint8_t status_tag;
        uint8_t tag;
        unsigned int io_timeout;
        bool have_interrupt_endpoint;

        class creator;
};

}
}

#endif
