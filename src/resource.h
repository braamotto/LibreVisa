#ifndef freevisa_resource_h_
#define freevisa_resource_h_ 1

#include "object.h"

namespace freevisa {

class session;

class resource :
        public object
{
public:
        resource();

protected:
        ~resource() throw() { }

public:
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *) = 0;
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *) = 0;
        virtual ViStatus ParseRsrc(
                ViRsrc rsrc,
                ViUInt16 *intfType,
                ViUInt16 *intfNum,
                ViString *rsrcClass = 0,
                ViString *unaliasedExpandedRsrcName = 0,
                ViString *aliasIfExists = 0) = 0;

        bool lock_exclusive(session const *);
        void unlock_exclusive();

private:
        session const *exclusive_lock_holder;
};

}

#endif
