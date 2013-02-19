#
#   Copyright (C) 2012,2013 Simon Richter
#   Copyright (C) 2012 A. Pic
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

check_PROGRAMS = \
	lock_attr_exclusive \
	lock_attr_global \
	lock_attr_shared \
	lock_invalid_mode \
	lock_recursive_exclusive \
	lock_recursive_shared \
	open \
	open_multi \
	print \
	read \
	rule_3_2_3 \
	rule_3_3_2 \
	rule_3_6_38 \
	rule_3_7_3 \
	rule_4_2_1 \
	rule_4_2_2 \
	rule_4_3_11 \
	wait_immediate_noevents \
	write

check_LIBRARIES = libdummy.a

libdummy_a_SOURCES = dummy.cpp dummy.h
libdummy_a_CPPFLAGS = -I$(top_srcdir)/src

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)/include
LDADD = libdummy.a ../src/libvisa.la
LDFLAGS = -no-install
