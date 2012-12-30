#
#   Copyright (C) 2012 Simon Richter
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

SUBDIRS = . io

check_PROGRAMS = \
	lock_attr_exclusive \
	lock_attr_global \
	lock_attr_shared \
	lock_invalid_mode \
	lock_recursive_exclusive \
	lock_recursive_shared \
	rule_3_2_3 \
	rule_3_3_2 \
	rule_3_6_38 \
	rule_3_7_3 \
	rule_4_2_1 \
	rule_4_2_2 \
	rule_4_3_11

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)/include
LDADD = ../src/libvisa.la
LDFLAGS = -no-install
