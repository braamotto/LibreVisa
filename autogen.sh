#!/bin/sh

aclocal && autoheader && libtoolize && autoconf && automake --add-missing && automake
