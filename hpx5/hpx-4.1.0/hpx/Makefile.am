###################
# Makefile for HPX #
####################

EXTRA_DIST = \
        README.md README.apex   \
        LICENSE INSTALL AUTHORS

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I config -I contrib/hwloc/config

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = hpx.pc

SUBDIRS       = contrib libhpx include examples
DIST_SUBDIRS  = contrib libhpx include examples libapex docs tests

if HAVE_APEX
SUBDIRS      += libapex
endif

if ENABLE_DOCS
SUBDIRS      += docs
endif

if ENABLE_TESTS
SUBDIRS      += tests
endif

bin_SCRIPTS = scripts/hpx-config

install-exec-hook:
	cd $(DESTDIR)$(libdir); find $(DESTDIR)$(libdir) -type f -name \*.la -delete
