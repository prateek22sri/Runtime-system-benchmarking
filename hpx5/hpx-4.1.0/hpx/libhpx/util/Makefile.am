# LT Utility libraries
noinst_LTLIBRARIES   = libutil.la

noinst_HEADERS       = parser.h

# libutil files and flags
libutil_la_CPPFLAGS	= -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libutil_la_CFLAGS	= $(LIBHPX_CFLAGS)
libutil_la_CXXFLAGS	= $(LIBHPX_CXXFLAGS)
libutil_la_SOURCES	= parser.c options.cpp Bitmap.cpp
