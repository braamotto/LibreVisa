#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dummy.h"

#include "resource.h"
#include "resource_creator.h"
#include "resource_factory.h"

#include <list>
#include <string>

namespace {

using namespace freevisa;

class dummy_resource :
        public resource
{
public:
        virtual ViStatus Close() { return VI_SUCCESS; }
        virtual ViStatus Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
        {
                log.push_back(log_line_type(buf, count));
                *retCount = count;
                return VI_SUCCESS;
        }

        typedef std::basic_string<ViByte> log_line_type;
        typedef std::list<log_line_type> log_type;

        static log_type log;
};

dummy_resource::log_type dummy_resource::log;

class dummy_creator :
        public resource_creator
{
public:
        virtual char const *name() const { return "DUMMY"; }
        virtual resource *create(ViRsrc) const { return new dummy_resource; }
        static dummy_creator instance;
};

dummy_creator dummy_creator::instance;

}

dummy_resource::log_type::const_iterator reader_iterator;

void dummy_reader_restart()
{
        reader_iterator = dummy_resource::log.begin();
}

bool dummy_reader_isempty()
{
        return reader_iterator == dummy_resource::log.end();
}

ViByte const *dummy_reader_read(ViUInt32 *count)
{
        if(count)
                *count = reader_iterator->size();
        return reader_iterator->c_str();
}

void dummy_reader_reset()
{
        dummy_resource::log.clear();
        dummy_reader_restart();
}

void using_dummy_resource()
{
        resource_factory::instance.register_creator(dummy_creator::instance);
        dummy_reader_reset();
}

