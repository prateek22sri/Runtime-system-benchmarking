#######################
# Makefile for Photon #
#######################

instdir = $(prefix)/include
inst_HEADERS=include/photon.h                 \
	     include/photon_config.h          \
	     include/photon_api.h             \
	     include/photon_io.h              \
	     include/photon_xsp_forwarder.h   \
	     include/photon_attributes.h      \
	     include/photon_collectives.h     \
	     include/photon_atomics.h

EXTRA_DIST = autogen.sh                       \
	     README NEWS TODO

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS= -I config
SUBDIRS = src
DIST_SUBDIRS = src

if ENABLE_TESTS
if HAVE_MPI
SUBDIRS += test
DIST_SUBDIRS += test
endif
if HAVE_CHECK
SUBDIRS += tests
DIST_SUBDIRS += tests
endif
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = photon.pc
