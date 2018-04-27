bin_PROGRAMS = photon_test             \
	       photon_test_get         \
	       photon_test_put         \
	       photon_test_forwarder   \
	       photon_test_interleaved \
	       photon_test_coll        \
	       photon_test_nbc_coll    \
	       photon_pingpong         \
	       photon_ud_blockid       \
	       photon_ud_pingpong      \
	       photon_ud_mton          \
	       pwc_serial              \
	       pwc_thread              \
	       pwc_varcid              \
	       pwc_thread_rand         \
	       pwc_thread_check        \
	       pwc_pp                  \
	       pwc_mr

AM_CPPFLAGS  = -D_GNU_SOURCE $(PHOTON_APPS_CPPFLAGS) -I$(top_srcdir)/include
AM_CFLAGS    = $(PHOTON_APPS_CFLAGS) -I$(top_srcdir)/src/nbc -Wno-unused
AM_LDFLAGS   = $(PHOTON_APPS_LDFLAGS) -no-install -lcrypto
LDADD        = $(PHOTON_APPS_LDADD)
COMSRCS      = common.c common.h 

photon_test_SOURCES             = photon_test.c kernel.c
photon_test_get_SOURCES         = photon_test_get.c
photon_test_put_SOURCES         = photon_test_put.c
photon_test_forwarder_SOURCES   = photon_test_forwarder.c
photon_test_interleaved_SOURCES = photon_test_interleaved.c
photon_test_coll_SOURCES        = photon_test_coll.c
photon_test_nbc_coll_SOURCES    = photon_test_nbc_coll.c
photon_pingpong_SOURCES         = photon_pingpong.c

pwc_serial_SOURCES              = pwc_serial.c $(COMSRCS)
pwc_thread_SOURCES              = pwc_thread.c $(COMSRCS)
pwc_varcid_SOURCES              = pwc_varcid.c $(COMSRCS)
pwc_thread_rand_SOURCES         = pwc_thread_rand.c $(COMSRCS)
pwc_thread_check_SOURCES        = pwc_thread_check.c $(COMSRCS)
pwc_pp_SOURCES                  = pwc_pp.c $(COMSRCS)
pwc_mr_SOURCES                  = pwc_mr.c $(COMSRCS)

photon_ud_blockid_SOURCES       = ud_bravo_tests/photon_ud_blockid.c \
                                  ud_bravo_tests/cutter_ids.h        \
				  ud_bravo_tests/bravo_ids.h
photon_ud_pingpong_SOURCES      = ud_bravo_tests/photon_ud_pingpong.c
photon_ud_mton_SOURCES          = ud_bravo_tests/photon_ud_mton.c

photon_test_DEPENDENCIES             = $(PHOTON_APPS_LDADD)
photon_test_get_DEPENDENCIES         = $(PHOTON_APPS_LDADD)
photon_test_put_DEPENDENCIES         = $(PHOTON_APPS_LDADD)
photon_test_forwarder_DEPENDENCIES   = $(PHOTON_APPS_LDADD)
photon_test_interleaved_DEPENDENCIES = $(PHOTON_APPS_LDADD)
photon_test_coll_DEPENDENCIES        = $(PHOTON_APPS_LDADD)
photon_test_nbc_coll_DEPENDENCIES    = $(PHOTON_APPS_LDADD)
photon_pingpong_DEPENDENCIES         = $(PHOTON_APPS_LDADD)

pwc_serial_DEPENDENCIES              = $(PHOTON_APPS_LDADD)
pwc_thread_DEPENDENCIES              = $(PHOTON_APPS_LDADD)
pwc_varcid_DEPENDENCIES              = $(PHOTON_APPS_LDADD)
pwc_pp_DEPENDENCIES                  = $(PHOTON_APPS_LDADD)
pwc_mr_DEPENDENCIES                  = $(PHOTON_APPS_LDADD)
pwc_thread_rand_DEPENDENCIES         = $(PHOTON_APPS_LDADD)
pwc_thread_check_DEPENDENCIES        = $(PHOTON_APPS_LDADD)

photon_ud_blockid_DEPENDENCIES       = $(PHOTON_APPS_LDADD)
photon_ud_pingpong_DEPENDENCIES      = $(PHOTON_APPS_LDADD)
photon_ud_mton_DEPENDENCIES          = $(PHOTON_APPS_LDADD)
