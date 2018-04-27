noinst_LTLIBRARIES    = libphcore.la

libphcore_la_SOURCES  = libphoton.h                   \
			libphoton.c                   \
		        photon_request.h              \
		        photon_request.c              \
		        photon_event.h                \
		        photon_event.c                \
		        photon_pwc.h                  \
		        photon_pwc.c                  \
		        photon_coll.h                 \
		        photon_coll.c                 \
		        photon_pwc_coll.h             \
		        photon_pwc_coll.c             \
                        photon_backend.h              \
                        photon_backend.c              \
                        photon_exchange.h             \
                        photon_exchange.c             \
                        photon_buffer.h               \
                        photon_buffer.c               \
                        photon_msgbuffer.h            \
                        photon_msgbuffer.c            \
                        photon_buffertable.h          \
                        photon_buffertable.c          \
                        photon_rdma_ledger.h          \
                        photon_rdma_ledger.c          \
                        photon_rdma_INFO_ledger.h     \
                        photon_rdma_INFO_ledger.c     \
                        photon_rdma_EAGER_buf.h       \
                        photon_rdma_EAGER_buf.c

libphcore_la_CPPFLAGS = $(LIBPHOTON_CPPFLAGS)
libphcore_la_CFLAGS   = $(LIBPHOTON_CFLAGS)                     \
			-I$(top_srcdir)/src/verbs               \
			-I$(top_srcdir)/src/ugni                \
			-I$(top_srcdir)/src/fi                  \
			-I$(top_srcdir)/src/forwarder
libphcore_la_CXXFLAGS = $(LIBPHOTON_CXXFLAGS)

libphcore_ladir       = $(top_builddir)/src/libphcore
libphcore_la_LDFLAGS  = $(LIBPHOTON_LDFLAGS)
libphcore_la_LIBADD   = $(top_builddir)/src/util/libutil.la                   \
		        $(top_builddir)/src/contrib/bit_array/libbit_array.la \
		        $(top_builddir)/src/contrib/libsync/libsync.la

libphcore_la_DEPENDENCIES = $(top_builddir)/src/util/libutil.la                   \
			    $(top_builddir)/src/contrib/bit_array/libbit_array.la \
			    $(top_builddir)/src/contrib/libsync/libsync.la
