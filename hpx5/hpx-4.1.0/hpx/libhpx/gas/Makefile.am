SUBDIRS = affinity

# LT Utility libraries
noinst_LTLIBRARIES = libgas.la

EXTRA_DIST = smp/SMP.h

# libgas files and flags
libgas_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libgas_la_CFLAGS   = $(LIBHPX_CFLAGS)
libgas_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libgas_la_SOURCES  = hpx_gas_glue.cpp \
                     GAS.cpp \
                     gpa.c \
                     smp/SMP.cpp \
                     bcast.cpp \
                     Allocator.cpp
libgas_la_LIBADD   = affinity/libaffinity.la

if HAVE_NETWORK
SUBDIRS           += pgas
libgas_la_LIBADD  += pgas/libpgas.la
endif

if HAVE_NETWORK
if HAVE_AGAS
SUBDIRS           += agas
libgas_la_LIBADD  += agas/libagas.la
endif
endif
