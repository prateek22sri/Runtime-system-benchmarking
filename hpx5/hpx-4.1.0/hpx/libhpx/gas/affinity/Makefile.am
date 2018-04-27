# LT Utility libraries
noinst_LTLIBRARIES = libaffinity.la

# libgas files and flags
libaffinity_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libaffinity_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libaffinity_la_SOURCES  = Affinity.cpp None.cpp CuckooHash.cpp

if HAVE_URCU
libaffinity_la_SOURCES += URCU.cpp
endif
