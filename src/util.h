#ifndef freevisa_util_h_
#define freevisa_util_h_ 1

#include <limits>

namespace freevisa {

inline unsigned int parse_optional_int(char const *&cursor)
{
        unsigned int ret = 0;
        while(*cursor >= '0' && *cursor <= '9')
        {
                if(ret > std::numeric_limits<unsigned int>::max() / 10)
                        return 0;
                ret *= 10;
                ret += *cursor++ - '0';
        }
        return ret;
}

}

#endif
