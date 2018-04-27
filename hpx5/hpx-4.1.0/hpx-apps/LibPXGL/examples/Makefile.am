appsdir       = $(datarootdir)/apps/

apps_PROGRAMS = sssp

sssp_SOURCES   = sssp.c

AM_LDFLAGS     = $(HPX_JEMALLOC_AS_MALLOC_RPATH) $(HPX_JEMALLOC_RPATH) -no-install

sssp_CFLAGS    = $(HPX_CFLAGS) -I$(top_srcdir)/include
sssp_LDADD     = $(top_builddir)/libpxgl.la $(HPX_LIBS) -lm
