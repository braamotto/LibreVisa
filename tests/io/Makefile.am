check_PROGRAMS = \
        open \
        open_multi \
	print \
        write

check_LIBRARIES = libdummy.a

libdummy_a_SOURCES = dummy.cpp dummy.h
libdummy_a_CPPFLAGS = -I$(top_srcdir)/src

TESTS = $(check_PROGRAMS)

INCLUDES = -I$(top_srcdir)/include
LDADD = libdummy.a ../../src/libvisa.la
LDFLAGS = -no-install
