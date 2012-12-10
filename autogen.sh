#!/bin/sh -e

aclocal
autoheader
libtoolize
automake --add-missing
autoconf
