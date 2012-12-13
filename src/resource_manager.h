#ifndef freevisa_resource_manager_h_
#define freevisa_resource_manager_h_ 1

#include "resource.h"

#include "resource_creator.h"

#include <map>
#include <cstring>

namespace freevisa {

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

        struct case_insensitive_less
        {
                bool operator()(char const *lhs, char const *rhs) { return strcasecmp(lhs, rhs) < 0; }
        };

        typedef std::map<char const *, resource_creator const *, case_insensitive_less> creator_map;
        typedef creator_map::iterator creator_iterator;
        typedef creator_map::const_iterator creator_const_iterator;

        creator_map creators;
};

extern resource_manager default_resource_manager;

}

#endif
