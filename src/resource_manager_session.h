#ifndef freevisa_resource_manager_session_h_
#define freevisa_resource_manager_session_h_ 1

#include "resource_manager.h"
#include "local_session.h"

namespace freevisa {

class resource_manager_session :
	public local_session
{
public:
	explicit resource_manager_session(resource_manager &rm) : rm(rm) { }

	resource_manager &get() const { return rm; }

private:
	resource_manager &rm;
};

}

#endif
