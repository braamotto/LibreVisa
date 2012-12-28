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

public:
        unsigned int add_ref() { return ++refcount; }
        unsigned int release() { return --refcount; }

        virtual ViStatus GetAttribute(ViAttr, void *);

        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus Read(ViBuf, ViUInt32, ViUInt32 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus ReadSTB(ViUInt16 *)
        {
                return VI_ERROR_NSUP_OPER;
        }

        virtual ViStatus ParseRsrc(
                ViRsrc,
                ViUInt16 *,
                ViUInt16 *,
                ViString * = 0,
                ViString * = 0,
                ViString * = 0)
        {
                return VI_ERROR_NSUP_OPER;
        }

        bool lock_exclusive(session const *);
        void unlock_exclusive();

private:
        unsigned int refcount;
        session const *exclusive_lock_holder;
};

}

#endif
