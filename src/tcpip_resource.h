#ifndef tcpip_resource_h_
#define tcpip_resource_h_ 1

#include "resource.h"

namespace freevisa {

class tcpip_resource :
        public resource
{
public:
        virtual ViStatus Close();
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

private:
        class creator;
        static creator const creator_inst;
};

}

#endif
