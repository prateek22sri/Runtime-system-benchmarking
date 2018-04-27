SUBDIRS = arch

noinst_LTLIBRARIES = libsync.la

libsync_la_CPPFLAGS = $(LIBPHOTON_CPPFLAGS)
libsync_la_CFLAGS   = $(LIBPHOTON_CFLAGS) -fvisibility=hidden
libsync_la_SOURCES  = locks.c sr_barrier.c two_lock_queue.c \
                      chase_lev_ws_deque.c backoff.c lockable_ptr.c \
                      clh_lock.c mcs_lock.c nop.h

if HAVE_CMPXCHG16B
  libsync_la_SOURCES += ms_queue.c
endif
libsync_la_LIBADD  = arch/libarch.la

EXTRA_DIST         = include
