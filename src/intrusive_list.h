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

#ifndef librevisa_intrusive_list_h_
#define librevisa_intrusive_list_h_ 1

namespace librevisa {

template<typename T>
class intrusive_list
{
public:
        class iterator
        {
        public:
                iterator(T *pos = 0) : pos(pos) { }

                T *operator->() const { return pos; }
                T &operator*() const { return *pos; }

                bool operator==(iterator const &rhs) const { return pos == rhs.pos; }
                bool operator!=(iterator const &rhs) const { return pos != rhs.pos; }

                iterator &operator++() { pos = pos->next; return *this; }

        private:
                T *pos;
        };

        iterator begin() { return first; }
        iterator end() { return 0; }

        void push_front(T &obj)
        {
                obj.next = first;
                obj.prev = 0;
                if(first)
                        first->prev = &obj;
                first = &obj;
        }

        iterator erase(iterator i)
        {
                iterator ret = i->next;
                if(i == first)
                        first = i->next;
                if(i->next)
                        i->next->prev = i->prev;
                if(i->prev)
                        i->prev->next = i->next;
                i->next = 0;
                i->prev = 0;
                return ret;
        }

private:
        T *first;
};

}

#endif
