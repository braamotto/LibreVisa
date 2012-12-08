lib_LTLIBRARIES = libvisa.la

libvisa_la_SOURCES = \
	exception.h \
	object.cpp object.h \
	object_cache.cpp object_cache.h \
	resource_manager.cpp resource_manager.h \
	session.cpp session.h \
	session_creator.h \
	session_factory.cpp session_factory.h \
	viClose.cpp \
	viGetAttribute.cpp \
	viLock.cpp \
	viOpen.cpp \
	viOpenDefaultRM.cpp \
	viParseRsrc.cpp \
	viSetAttribute.cpp \
	viUnlock.cpp \
	viSetBuf.cpp \
	viFlush.cpp \
	$(VXI_SOURCES)

if WITH_VXI
VXI_SOURCES = tcpip_session.cpp tcpip_session.h
else
VXI_SOURCES =
endif

INCLUDES = -I$(top_srcdir)/include
