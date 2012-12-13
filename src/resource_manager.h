#ifndef freevisa_resource_manager_h_
#define freevisa_resource_manager_h_ 1

#include "resource.h"

#include <list>

namespace freevisa {

class resource_creator;

class resource_manager :
        public resource
{
public:
        resource_manager();
        ~resource_manager() throw() { }

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

        resource *create(ViRsrc rsrc);

        void register_creator(resource_creator const &);
        void unregister_creator(resource_creator const &);

private:
        unsigned int refcount;

        typedef std::list<resource_creator const *> creator_list;
        typedef creator_list::iterator creator_iterator;
        typedef creator_list::const_iterator creator_const_iterator;

        creator_list creators;
};

extern resource_manager default_resource_manager;

}

#endif
