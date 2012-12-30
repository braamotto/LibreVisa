/* 
 * Copyright (C) 2012 Simon Richter
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dummy.h"

#include "resource.h"
#include "resource_creator.h"
#include "resource_manager.h"

#include <list>
#include <queue>
#include <string>
#include <cstring>

namespace {

using namespace freevisa;

class dummy_resource :
        public resource
{
public:
        virtual ViStatus Close() { delete this; return VI_SUCCESS; }
        virtual ViStatus Read(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
        {
                if(reads.empty())
                        return VI_ERROR_IO;
                read_op &op = reads.front();
                if(op.count > count)
                        return VI_ERROR_RAW_RD_PROT_VIOL;
                memcpy(buf, op.data, op.count);
                if(retCount)
                        *retCount = op.count;
                ViStatus ret = op.ret;
                reads.pop();
                return ret;
        }

        virtual ViStatus Write(ViBuf buf, ViUInt32 count, ViUInt32 *retCount)
        {
                log.push_back(log_line_type(buf, count));
                *retCount = count;
                return VI_SUCCESS;
        }

        struct read_op
        {
                char const *data;
                unsigned int count;
                ViStatus ret;
        };

        static std::queue<read_op> reads;

        typedef std::basic_string<ViByte> log_line_type;
        typedef std::list<log_line_type> log_type;

        static log_type log;
};

std::queue<dummy_resource::read_op> dummy_resource::reads;
dummy_resource::log_type dummy_resource::log;

class dummy_creator :
        public resource_creator
{
public:
        virtual resource *create(std::vector<std::string> const &) const { return new dummy_resource; }
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

void dummy_writer_append(char const *data, unsigned int count, ViStatus ret)
{
        dummy_resource::read_op op = { data, count, ret };
        dummy_resource::reads.push(op);
}

void using_dummy_resource()
{
        default_resource_manager.register_creator(dummy_creator::instance);
        dummy_reader_reset();
}
