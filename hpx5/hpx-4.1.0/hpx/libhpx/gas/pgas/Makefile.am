# The PGAS library
noinst_LTLIBRARIES  = libpgas.la

noinst_HEADERS = PGAS.h HeapSegment.h Allocator.h
libpgas_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libpgas_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libpgas_la_SOURCES  = HeapSegment.cpp PGAS.cpp

if HAVE_JEMALLOC
libpgas_la_SOURCES += jemalloc.cpp
endif

if HAVE_TBBMALLOC
libpgas_la_SOURCES += tbbmalloc.cpp
endif
