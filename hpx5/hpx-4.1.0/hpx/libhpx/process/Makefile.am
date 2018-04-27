# The process infrastructure
noinst_LTLIBRARIES     = libprocess.la
noinst_HEADERS         = Allreduce.h Bitmap.h Continuation.h Reduce.h

libprocess_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libprocess_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libprocess_la_SOURCES  = broadcast.cpp process.cpp Bitmap.cpp \
                         Continuation.cpp Reduce.cpp Allreduce.cpp \
                         allreduce_glue.cpp
