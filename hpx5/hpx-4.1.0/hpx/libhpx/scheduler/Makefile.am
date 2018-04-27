SUBDIRS = arch lco

# The scheduler library
noinst_LTLIBRARIES       = libscheduler.la
noinst_HEADERS           = Condition.h Thread.h TatasLock.h

libscheduler_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libscheduler_la_CFLAGS   = $(LIBHPX_CFLAGS)
libscheduler_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libscheduler_la_SOURCES  = Condition.cpp Scheduler.cpp Thread.cpp Worker.cpp \
                           hpx_glue.cpp libhpx_glue.cpp
libscheduler_la_LIBADD   = arch/libarch.la lco/liblco.la

if ENABLE_INSTRUMENTATION
libscheduler_la_SOURCES += events.cpp
endif
