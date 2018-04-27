noinst_LTLIBRARIES = libinstrumentation.la
noinst_HEADERS     = file.h metadata.h

libinstrumentation_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libinstrumentation_la_CXXFLAGS   = $(LIBHPX_CXXFLAGS)

libinstrumentation_la_SOURCES  = file_header.cpp instrumentation.cpp \
                                 file.cpp console.cpp stats.cpp
