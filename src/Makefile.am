#
#   Copyright (C) 2012,2013 Simon Richter
#   Copyright (C) 2012 A. Pic
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

lib_LTLIBRARIES = libvisa.la

libvisa_la_SOURCES = \
	base_vprintf.cpp base_vprintf.h \
	condvar.h \
	event.h \
	event_queue.cpp event_queue.h \
	exception.h \
	findlist.cpp findlist.h \
	instrument_resource.h \
	lock.h \
	messagepump.cpp messagepump.h \
	mutex.h \
	object.cpp object.h \
	object_cache.cpp object_cache.h \
	resource.cpp resource.h \
	resource_creator.h \
	resource_manager.cpp resource_manager.h \
	session.cpp session.h \
	timeval_op.h \
	util.h \
	viBufWrite.cpp \
	viClose.cpp \
	viDisableEvent.cpp \
	viEnableEvent.cpp \
	viFindNext.cpp \
	viFindRsrc.cpp \
	viFlush.cpp \
	viGetAttribute.cpp \
	viLock.cpp \
	viOpen.cpp \
	viOpenDefaultRM.cpp \
	viParseRsrc.cpp \
	viPrintf.cpp \
	viRead.cpp \
	viReadSTB.cpp \
	viSetAttribute.cpp \
	viSetBuf.cpp \
	viSPrintf.cpp \
	viUnlock.cpp \
	viVPrintf.cpp \
	viVSPrintf.cpp \
	viWaitOnEvent.cpp \
	viWrite.cpp \
	$(USB_SOURCES) \
	$(VXI_SOURCES)
libvisa_la_LDFLAGS = -export-symbols $(srcdir)/libvisa.sym

dist_noinst_DATA = libvisa.sym

if WITH_USB
USB_SOURCES = \
	usb/usb_resource.cpp usb/usb_resource.h \
	usb/usb_resource_creator.cpp usb/usb_resource_creator.h \
	usb/usb_string.h
else
USB_SOURCES =
endif

if WITH_VXI
VXI_SOURCES = \
	vxi/vxi_resource.cpp vxi/vxi_resource.h \
	vxi/vxi_resource_creator.cpp vxi/vxi_resource_creator.h \
	$(AVAHI_SOURCES)
else
VXI_SOURCES =
endif

if WITH_AVAHI
AVAHI_SOURCES = \
	vxi/avahi.cpp vxi/avahi.h
else
AVAHI_SOURCES =
endif

INCLUDES = -I$(top_srcdir)/include
