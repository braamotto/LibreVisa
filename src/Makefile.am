lib_LTLIBRARIES = libvisa.la

libvisa_la_SOURCES = \
	base_vprintf.cpp base_vprintf.h \
	exception.h \
	object.cpp object.h \
	object_cache.cpp object_cache.h \
	resource.cpp resource.h \
	resource_creator.h \
	resource_manager.cpp resource_manager.h \
	session.cpp session.h \
	viBufWrite.cpp \
	viClose.cpp \
	viFlush.cpp \
	viGetAttribute.cpp \
	viLock.cpp \
	viOpen.cpp \
	viOpenDefaultRM.cpp \
	viParseRsrc.cpp \
	viPrintf.cpp \
	viSetAttribute.cpp \
	viSetBuf.cpp \
	viSPrintf.cpp \
	viUnlock.cpp \
	viVPrintf.cpp \
	viVSPrintf.cpp \
        viWrite.cpp \
	$(VXI_SOURCES)
libvisa_la_LDFLAGS = -export-symbols libvisa.sym

if WITH_VXI
VXI_SOURCES = tcpip.cpp
else
VXI_SOURCES =
endif

INCLUDES = -I$(top_srcdir)/include
