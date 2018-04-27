# LT Utility libraries
noinst_LTLIBRARIES	  = libboot.la

# libboot files and flags
libboot_la_CPPFLAGS = -I$(top_srcdir)/include $(LIBHPX_CPPFLAGS)
libboot_la_CXXFLAGS = $(LIBHPX_CXXFLAGS)
libboot_la_SOURCES  = Network.cpp SMP.cpp

if HAVE_MPI
libboot_la_CXXFLAGS	+= @MPI_CFLAGS@
libboot_la_SOURCES	+= MPI.cpp
libboot_la_LIBADD   = @MPI_LIBS@
endif

if HAVE_PMI
libboot_la_CXXFLAGS	+= @PMI_CFLAGS@
libboot_la_SOURCES 	+= PMI.cpp
libboot_la_LIBADD   = @PMI_LIBS@
endif
