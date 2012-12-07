#ifndef freevisa_resource_manager_h_
#define freevisa_resource_manager_h_ 1

#include "session.h"

namespace freevisa {

class resource_manager :
	public session
{
public:
	resource_manager();

	ViStatus Open();

	virtual ViStatus Close();
	virtual ViStatus GetAttribute(ViAttr, void *);
	virtual ViStatus SetAttribute(ViAttr, ViAttrState);

private:
	unsigned int refcount;
};

extern resource_manager default_resource_manager;

}

#endif
