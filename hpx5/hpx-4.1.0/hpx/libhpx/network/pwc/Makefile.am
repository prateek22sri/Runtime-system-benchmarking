# The isend-irecv network implementations
noinst_LTLIBRARIES = libpwc.la
noinst_HEADERS     = CircularBuffer.h Commands.h \
                     registered.h PhotonTransport.h ParcelBlock.h Peer.h

libpwc_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libpwc_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libpwc_la_SOURCES  = Peer.cpp \
                     ParcelBlock.cpp \
                     CircularBuffer.cpp \
                     lco_get.cpp \
                     lco_wait.cpp \
                     Commands.cpp \
                     rendezvous_send.cpp \
                     PWCNetwork.cpp \
                     AGASNetwork.cpp \
                     PGASNetwork.cpp \
                     PhotonTransport.cpp

if HAVE_JEMALLOC
libpwc_la_SOURCES += jemalloc.cpp
endif

if HAVE_TBBMALLOC
libpwc_la_SOURCES += tbbmalloc.cpp
endif
