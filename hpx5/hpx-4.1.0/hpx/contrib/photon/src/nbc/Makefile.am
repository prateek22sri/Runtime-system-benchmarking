noinst_LTLIBRARIES = libnbc.la libnbcpwc.la

SUBDIRS      = libdict ph
DIST_SUBDIRS = libdict ph

noinst_HEADERS = nbc.h nbc_internal.h

libnbc_la_SOURCES = \
	nbc.c \
	nbc_iallgather.c \
	nbc_iallgatherv.c \
	nbc_iallreduce.c \
	nbc_ialltoall.c \
	nbc_ialltoallv.c \
	nbc_ibarrier.c \
	nbc_ibcast.c \
	nbc_ibcast_inter.c \
	nbc_igather.c \
	nbc_igatherv.c \
	nbc_ireduce.c \
	nbc_ireduce_scatter.c \
	nbc_iscan.c \
	nbc_iscatter.c \
	nbc_iscatterv.c \
	nbc_op.c \
	nbc_internal.h \
	LICENSE \
	CHANGELOG

libnbc_la_CPPFLAGS = $(LIBPHOTON_CPPFLAGS)
libnbc_la_CFLAGS   = $(LIBPHOTON_CFLAGS) -fvisibility=hidden

libnbcpwc_la_SOURCES = \
	photon_pwc_nbc.h \
	photon_pwc_nbc.c

libnbcpwc_la_CPPFLAGS = $(LIBPHOTON_CPPFLAGS)
libnbcpwc_la_CFLAGS   = $(LIBPHOTON_CFLAGS)
libnbcpwc_la_LIBADD   = libnbc.la libdict/libdict.la ph/libph.la

libnbcpwc_la_DEPENDENCIES = libnbc.la libdict/libdict.la ph/libph.la
