# The isend-irecv network implementations
noinst_LTLIBRARIES  = libisir.la
noinst_HEADERS      = emulate_pwc.h MPITransport.h IRecvBuffer.h ISendBuffer.h \
                      FunneledNetwork.h parcel_utils.h

libisir_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libisir_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libisir_la_SOURCES  = FunneledNetwork.cpp \
                      ISendBuffer.cpp \
                      IRecvBuffer.cpp \
                      emulate_pwc.cpp \
                      lco_get.cpp \
                      lco_wait.cpp
