#ifndef freevisa_resource_manager_h_
#define freevisa_resource_manager_h_ 1

#include "resource.h"
#include "resource_factory.h"

namespace freevisa {

class resource_manager :
        public resource,
        public resource_factory
{
public:
        resource_manager();

        ViStatus Open();

        virtual ViStatus Open(ViRsrc, ViAccessMode, ViUInt32, ViSession *);
        virtual ViStatus Write(ViBuf, ViUInt32, ViUInt32 *) { return VI_ERROR_NSUP_OPER; }
        virtual ViStatus ParseRsrc(
                ViRsrc rsrc,
                ViUInt16 *intfType,
                ViUInt16 *intfNum,
                ViString *rsrcClass,
                ViString *unaliasedExpandedRsrcName,
                ViString *aliasIfExists);

        virtual ViStatus Close();
        virtual ViStatus GetAttribute(ViAttr, void *);
        virtual ViStatus SetAttribute(ViAttr, ViAttrState);

private:
        unsigned int refcount;
};

extern resource_manager default_resource_manager;

}

#endif
