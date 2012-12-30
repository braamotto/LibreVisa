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

#ifndef librevisa_util_h_
#define librevisa_util_h_ 1

#include <limits>

namespace librevisa {

inline bool is_valid_in_hostname(char c)
{
        return
                (c >= '0' && c <= '9') ||
                (c >= 'A' && c <= 'Z') ||
                (c >= 'a' && c <= 'z') ||
                (c == '-') || (c == '.');
}

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
