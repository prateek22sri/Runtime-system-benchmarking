# The networking library
noinst_LTLIBRARIES     = libnetwork.la

if HAVE_MPI
BUILD_ISIR             = isir
LIBISIR                = isir/libisir.la
endif

if HAVE_PHOTON
BUILD_PWC              = pwc
LIBPWC                 = pwc/libpwc.la
endif

SUBDIRS = $(BUILD_ISIR) $(BUILD_PWC)


noinst_HEADERS         = Wrappers.h SMPNetwork.h

libnetwork_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libnetwork_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libnetwork_la_SOURCES  = locality.cpp \
                         Network.cpp \
                         Wrappers.cpp \
                         parcel.cpp \
                         hpx_parcel_glue.cpp \
                         ParcelStringOps.cpp \
                         SMPNetwork.cpp \
                         InstrumentationWrapper.cpp \
                         CoalescingWrapper.cpp \
                         CompressionWrapper.cpp

libnetwork_la_LIBADD   = $(LIBPWC) $(LIBISIR)
