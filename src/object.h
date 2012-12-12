#ifndef freevisa_object_h_
#define freevisa_object_h_ 1

#include "visa.h"

namespace freevisa {

class object
{
public:
        object();

        virtual ~object() throw() { }

        virtual ViStatus Close() = 0;
        virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
        virtual ViStatus Unlock();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

private:
        unsigned int exclusive_lock_count, shared_lock_count;
};

}

#endif
