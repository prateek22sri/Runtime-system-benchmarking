# LT Utility libraries
noinst_LTLIBRARIES	  = libmemory.la

# libboot files and flags
libmemory_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libmemory_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libmemory_la_SOURCES  = glue.cpp

if HAVE_NETWORK
if HAVE_JEMALLOC
libmemory_la_SOURCES += jemalloc.cpp
endif
endif

if HAVE_NETWORK
if HAVE_TBBMALLOC
libmemory_la_SOURCES += tbbmalloc.cpp
endif
endif
