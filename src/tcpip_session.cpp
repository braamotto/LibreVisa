#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tcpip_session.h"

#include "session_creator.h"

namespace freevisa {

ViStatus tcpip_session::Close()
{
        delete this;
        return VI_SUCCESS;
}

ViStatus tcpip_session::Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId)
{
        return VI_SUCCESS;
}

ViStatus tcpip_session::Unlock()
{
        return VI_SUCCESS;
}

ViStatus tcpip_session::GetAttribute(ViAttr attr, void *attrState)
{
        return session::GetAttribute(attr, attrState);
}

ViStatus tcpip_session::SetAttribute(ViAttr attr, ViAttrState attrState)
{
        return session::SetAttribute(attr, attrState);
}

class tcpip_session::creator :
        public session_creator
{
public:
        virtual char const *name() const;
        virtual tcpip_session *create(ViRsrc) const;
};

char const *tcpip_session::creator::name() const
{
        return "TCPIP";
}

tcpip_session *tcpip_session::creator::create(ViRsrc) const
{
        return new tcpip_session;
}

}
