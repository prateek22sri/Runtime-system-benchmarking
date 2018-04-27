noinst_LTLIBRARIES   = libsmp.la

libsmp_la_CPPFLAGS = $(LIBPHOTON_CPPFLAGS)
libsmp_la_CFLAGS   = $(LIBPHOTON_CFLAGS)
libsmp_la_SOURCES  = photon_smp.h         \
		     photon_smp.c         \
		     photon_smp_connect.h


if HAVE_SMP_POSIX
libsmp_la_SOURCES += photon_smp_posix.h \
		     photon_smp_posix.c
endif
