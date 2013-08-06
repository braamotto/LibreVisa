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

#include "mutex.h"

#include <windows.h>

#ifndef librevisa_condvar_windows_h_
#define librevisa_condvar_windows_h_ 1

namespace librevisa {

class condvar
{
public:
        condvar() { InitializeConditionVariable(&impl); }

        void wait(mutex &cs) { SleepConditionVariableCS(&impl, &cs.impl, INFINITE); }
        bool wait(mutex &cs, DWORD timeout_ms)
        {
                return SleepConditionVariableCS(&impl, &cs.impl, timeout_ms) != 0;
        }

private:
        CONDITION_VARIABLE impl;
};

}

#endif
