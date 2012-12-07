#ifndef freevisa_object_h_
#define freevisa_object_h_ 1

#include "visa.h"

namespace freevisa {

class object
{
public:
	virtual ViStatus Lock(ViAccessMode, ViUInt32, ViKeyId, ViKeyId);
	virtual ViStatus Unlock();

	virtual ViStatus Close() = 0;
	virtual ViStatus GetAttribute(ViAttr, void *) = 0;
	virtual ViStatus SetAttribute(ViAttr, ViAttrState) = 0;

protected:
	object();

	ViStatus GetObjectAttribute(ViAttr, void *);
	ViStatus SetObjectAttribute(ViAttr, ViAttrState);

	ViAccessMode lockState;
};

}

#endif
