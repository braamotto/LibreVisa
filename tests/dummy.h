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

#ifndef librevisa_tests_dummy_h_
#define librevisa_tests_dummy_h_

#include <visa.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void using_dummy_resource();

void dummy_reader_restart();
bool dummy_reader_isempty();
ViByte const *dummy_reader_read(ViUInt32 *count);
void dummy_reader_reset();

void dummy_writer_append(char const *data, unsigned int count, ViStatus ret);

#ifdef __cplusplus
}
#endif

#endif
