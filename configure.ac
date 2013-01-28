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

# Initialize pkg-config.
# We require at least 0.22, as "Requires.private" behaviour changed there.
PKG_PROG_PKG_CONFIG([0.22])

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile tests/io/Makefile
		 librevisa.pc])

# Checks for programs.
AC_PROG_CXX
AC_PROG_LIBTOOL

# Checks for libraries.
AC_CHECK_LIB([vxiclient], [create_link_1])

# This variable collects the pkg-config names of all detected libs.
# It is then used to construct the "Requires.private:" field in the
# librevisa.pc file.
LIBREVISA_PKGLIBS=""

PKG_CHECK_MODULES([libusb], [libusb-1.0 >= 1.0.8],
                  [CXXFLAGS="$CXXFLAGS $libusb_CFLAGS";
                  LIBS="$LIBS $libusb_LIBS";
                  LIBREVISA_PKGLIBS="$LIBREVISA_PKGLIBS libusb-1.0";
                  WITH_USB=yes], [WITH_USB=no])

PKG_CHECK_MODULES([libavahi_client], [avahi-client >= 0.6],
                  [CXXFLAGS="$CXXFLAGS $libavahi_client_CFLAGS";
                  LIBS="$LIBS $libavahi_client_LIBS";
                  LIBREVISA_PKGLIBS="$LIBREVISA_PKGLIBS libavahi-client";
                  WITH_AVAHI=yes;
                  AC_DEFINE([WITH_AVAHI], [1], [AVAHI support])], [WITH_AVAHI=no])

AC_SUBST(LIBREVISA_PKGLIBS)

# Checks for header files.
AC_CHECK_HEADER([vxi.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AM_CONDITIONAL([WITH_USB], [test "x$WITH_USB" = "xyes"])
AM_CONDITIONAL([WITH_VXI], [test $ac_cv_lib_vxiclient_create_link_1 = yes -a $ac_cv_header_vxi_h = yes])
AM_CONDITIONAL([WITH_AVAHI], [test "x$WITH_AVAHI" = "xyes"])

AC_OUTPUT
