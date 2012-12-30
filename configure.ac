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

# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([librevisa], [0.0.20121230], [Simon.Richter@hogyros.de])
AM_INIT_AUTOMAKE([1.10 foreign])
LT_INIT

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile tests/io/Makefile])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([usb-1.0], [libusb_init])
AC_CHECK_LIB([vxiclient], [create_link_1])

# Checks for header files.
AC_CHECK_HEADER([libusb-1.0/libusb.h])
AC_CHECK_HEADER([vxi.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_CONDITIONAL([WITH_USB], [test $ac_cv_lib_usb_1_0_libusb_init = yes -a $ac_cv_header_libusb_1_0_libusb_h = yes])
AM_CONDITIONAL([WITH_VXI], [test $ac_cv_lib_vxiclient_create_link_1 = yes -a $ac_cv_header_vxi_h = yes])

AC_OUTPUT
