# The LCO library
noinst_LTLIBRARIES  = liblco.la
noinst_HEADERS      = LCO.h

liblco_la_CPPFLAGS  = -I$(top_srcdir)/include -I$(srcdir)/../ $(LIBHPX_CPPFLAGS)
liblco_la_CFLAGS    = $(LIBHPX_CFLAGS)
liblco_la_CXXFLAGS  = $(LIBHPX_CXXFLAGS)
liblco_la_SOURCES   = LCO.cpp And.cpp Future.cpp Semaphore.cpp AllReduce.cpp \
                      Dataflow.cpp Gather.cpp Reduce.cpp AllToAll.cpp \
                      GenerationCounter.cpp UserLCO.cpp monoid.cpp
