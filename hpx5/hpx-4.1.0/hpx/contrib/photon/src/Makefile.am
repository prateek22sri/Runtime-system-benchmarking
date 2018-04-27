SUBDIRS       = util contrib phcore
DIST_SUBDIRS  = util contrib phcore smp verbs ugni fi forwarder nbc

if HAVE_SMP
SUBDIRS      += smp
endif

if HAVE_VERBS
SUBDIRS      += verbs
endif

if HAVE_UGNI
SUBDIRS      += ugni
endif

if HAVE_LIBFABRIC
SUBDIRS      += fi
endif

if HAVE_XSP
if BUILD_FORWARDER
SUBDIRS      += forwarder
endif
endif

if BUILD_NBC
SUBDIRS      += nbc
endif

lib_LTLIBRARIES       = libphoton.la
libphoton_la_SOURCES  = 
libphoton_la_LIBADD   =	$(LIBPHOTON_LIBS) $(LIBPHOTON_LIBADD)       \
			$(top_builddir)/src/phcore/libphcore.la

libphoton_la_DEPENDENCIES = $(LIBPHOTON_LIBADD)                     \
			    $(top_builddir)/src/phcore/libphcore.la
