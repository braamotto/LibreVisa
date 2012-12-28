lib_LTLIBRARIES = libvisa.la

libvisa_la_SOURCES = \
	base_vprintf.cpp base_vprintf.h \
	event.h \
	event_queue.cpp event_queue.h \
	exception.h \
	lock.h \
	mutex.h \
	object.cpp object.h \
	object_cache.cpp object_cache.h \
	resource.cpp resource.h \
	resource_creator.h \
	resource_manager.cpp resource_manager.h \
	session.cpp session.h \
	util.h \
	viBufWrite.cpp \
	viClose.cpp \
	viFlush.cpp \
	viGetAttribute.cpp \
	viLock.cpp \
	viOpen.cpp \
	viOpenDefaultRM.cpp \
	viParseRsrc.cpp \
	viPrintf.cpp \
	viRead.cpp \
	viSetAttribute.cpp \
	viSetBuf.cpp \
	viSPrintf.cpp \
	viUnlock.cpp \
	viVPrintf.cpp \
	viVSPrintf.cpp \
	viWrite.cpp \
	$(USB_SOURCES) \
	$(VXI_SOURCES)
libvisa_la_LDFLAGS = -export-symbols $(srcdir)/libvisa.sym

dist_noinst_DATA = libvisa.sym

if WITH_USB
USB_SOURCES = \
	usb/usb_resource.cpp usb/usb_resource.h \
	usb/usb_resource_creator.cpp usb/usb_resource_creator.h
else
USB_SOURCES =
endif

if WITH_VXI
VXI_SOURCES = vxi/vxi.cpp
else
VXI_SOURCES =
endif

INCLUDES = -I$(top_srcdir)/include
