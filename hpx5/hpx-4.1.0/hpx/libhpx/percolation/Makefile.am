noinst_LTLIBRARIES = libpercolation.la

libpercolation_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libpercolation_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libpercolation_la_SOURCES  = percolation.cpp hpx_glue.cpp

if HAVE_OPENCL
libpercolation_la_SOURCES += opencl.cpp
endif

