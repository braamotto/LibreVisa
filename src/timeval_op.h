/* 
 * Copyright (C) 2013 Simon Richter
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

#ifndef librevisa_timeval_op_h_
#define librevisa_timeval_op_h_ 1

#include <sys/time.h>

inline timeval operator+(timeval const &lhs, unsigned int microseconds)
{
        timeval ret = lhs;
        ret.tv_usec += microseconds % 1000000;
        ret.tv_sec += microseconds / 1000000;
        if(ret.tv_usec > 1000000)
        {
                ret.tv_usec -= 1000000;
                ret.tv_sec += 1;
        }
        return ret;
}

inline timeval &operator-=(timeval &lhs, timeval const &rhs)
{
        if(lhs.tv_usec > rhs.tv_usec)
        {
                lhs.tv_sec -= rhs.tv_sec;
                lhs.tv_usec -= rhs.tv_usec;
        }
        else
        {
                lhs.tv_sec = lhs.tv_sec - 1 - rhs.tv_sec;
                lhs.tv_usec = lhs.tv_usec + 1000000 - rhs.tv_usec;
        }
        return lhs;
}

inline bool operator==(timeval const &lhs, timeval const &rhs)
{
        return lhs.tv_sec == rhs.tv_sec && lhs.tv_usec == rhs.tv_usec;
}

inline bool operator<(timeval const &lhs, timeval const &rhs)
{
        return lhs.tv_sec < rhs.tv_sec ||
                (lhs.tv_sec == rhs.tv_sec && lhs.tv_usec < rhs.tv_usec);
}

#endif
