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

inline unsigned int parse_hex(char const *&cursor)
{
        unsigned int ret = 0;
        if(cursor[0] != '0' || cursor[1] != 'x')
                return 0;

        cursor += 2;

        for(;;)
        {
                char const c = *cursor;

                if(c >= '0' && c <= '9')
                {
                        ret <<= 4;
                        ret += c - '0';
                }
                else if(c >= 'A' && c <= 'F')
                {
                        ret <<= 4;
                        ret += c - 'A' + 10;
                }
                else if(c >= 'a' && c <= 'f')
                {
                        ret <<= 4;
                        ret += c - 'a' + 10;
                }
                else
                        return ret;

                ++cursor;
        }
}

}

#endif
