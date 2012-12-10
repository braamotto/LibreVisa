#ifndef freevisa_local_session_h_
#define freevisa_local_session_h_ 1

#include "session.h"

namespace freevisa {

class local_session :
	public session
{
public:
	virtual ViStatus Close();
	virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
	virtual ViStatus Unlock();
	virtual ViStatus GetAttribute(ViAttr, void *);
	virtual ViStatus SetAttribute(ViAttr, ViAttrState);

private:
	unsigned int exclusive_lock_count, shared_lock_count;
};

}

#endif
