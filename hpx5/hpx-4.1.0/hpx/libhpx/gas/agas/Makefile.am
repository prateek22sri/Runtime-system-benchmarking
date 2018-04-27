# The AGAS library
noinst_LTLIBRARIES  = libagas.la

noinst_HEADERS      = AGAS.h BlockTranslationTable.h ChunkTable.h GlobalVirtualAddress.h
libagas_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libagas_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libagas_la_SOURCES  = BlockTranslationTable.cpp ChunkTable.cpp \
                      ChunkAllocator.cpp AGAS.cpp

if HAVE_JEMALLOC
libagas_la_SOURCES += jemalloc.cpp
endif

if HAVE_TBBMALLOC
libagas_la_SOURCES += tbbmalloc.cpp
endif

if HAVE_REBALANCING
libagas_la_CFLAGS = $(LIBHPX_CFLAGS)
noinst_HEADERS     += rebalancer.h BlockStatisticsTable.h
libagas_la_SOURCES += Rebalancer.cpp partitioning.cpp BlockStatisticsTable.cpp
endif
