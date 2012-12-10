#!/bin/sh -ex

aclocal
autoheader
libtoolize
automake --add-missing
autoconf
