lib_LTLIBRARIES = libvisa.la

libvisa_la_SOURCES = \
	exception.h \
	local_session.cpp local_session.h \
	object.cpp object.h \
	object_cache.cpp object_cache.h \
	resource.h \
	resource_creator.h \
	resource_factory.cpp resource_factory.h \
	resource_manager.cpp resource_manager.h \
	resource_manager_session.cpp resource_manager_session.h \
	resource_session.cpp resource_session.h \
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
	viUnlock.cpp \
	viVPrintf.cpp \
	$(VXI_SOURCES)

if WITH_VXI
VXI_SOURCES = tcpip_resource.cpp tcpip_resource.h
else
VXI_SOURCES =
endif

INCLUDES = -I$(top_srcdir)/include
