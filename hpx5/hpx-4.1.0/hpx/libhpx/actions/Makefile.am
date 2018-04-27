# The action registration convenience library
noinst_LTLIBRARIES       = libactions.la
noinst_HEADERS           = exit.h init.h 

libactions_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libactions_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libactions_la_SOURCES  = init.cpp marshalled.cpp vectored.cpp ffi.cpp \
                         registration.cpp call_by_parcel.cpp exit.cpp \
                         get_handler.cpp
