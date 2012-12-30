#
#   Copyright (C) 2012 Simon Richter
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
	open \
	open_multi \
	print \
	read \
	write

check_LIBRARIES = libdummy.a

libdummy_a_SOURCES = dummy.cpp dummy.h
libdummy_a_CPPFLAGS = -I$(top_srcdir)/src

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)/include
LDADD = libdummy.a ../../src/libvisa.la
LDFLAGS = -no-install
