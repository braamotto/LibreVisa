#ifndef freevisa_session_h_
#define freevisa_session_h_ 1

#include "object.h"

namespace freevisa {

class session :
	public object
{
public:
	virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId) = 0;
	virtual ViStatus Unlock() = 0;
};

}

#endif
