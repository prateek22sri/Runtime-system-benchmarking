ACLOCAL_AMFLAGS = -I m4

SUBDIRS              = . include examples generatorhelper/include

noinst_appsdir       = $(datarootdir)/apps/

noinst_LTLIBRARIES   = libpxgl.la

libpxgl_la_SOURCES   = src/adjacency_list.c \
                       src/edge_list.c \
                       src/sssp_common.c \
                       src/sssp_chaotic.c \
                       src/sssp_dc.c \
                       src/sssp_dc1.c \
                       src/sssp_delta.c \
                       src/sssp_kla.c \
                       src/dimacs.c \
                       src/gteps.c \
                       src/statistics.c \
                       src/pqueue.c \
                       src/termination.c \
                       src/graph500_edge_list_generator.c \
                       src/graph_generator.c \
                       generatorhelper/src/globals.c \
                       generatorhelper/src/prng.c  \
                       generatorhelper/src/splittable_mrg.c
                       #generatorhelper/src/mrg_transitions.c

libpxgl_la_CFLAGS    = $(HPX_CFLAGS)
libpxgl_la_CPPFLAGS  = -I$(top_srcdir)/include -I$(top_srcdir)/generatorhelper/include
