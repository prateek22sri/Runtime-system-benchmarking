# LT Utility libraries
noinst_LTLIBRARIES	= libsystem.la

libsystem_la_CPPFLAGS	= -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libsystem_la_CXXFLAGS	= $(LIBHPX_CXXFLAGS)
libsystem_la_CFLAGS     = $(LIBHPX_CFLAGS)
libsystem_la_SOURCES    = affinity.cpp time.cpp Topology.cpp trace.cpp

if OS_LINUX
SUBDIRS = linux
libsystem_la_LIBADD	= linux/liblinux.la
endif

if OS_DARWIN
SUBDIRS = darwin
libsystem_la_LIBADD	= darwin/libdarwin.la
endif

