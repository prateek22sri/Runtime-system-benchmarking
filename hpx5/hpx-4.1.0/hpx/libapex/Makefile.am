####################
# Makefile for HPX #
####################

lib_LTLIBRARIES  = libapex_hpx.la

libapex_hpx_la_CFLAGS  		= -I$(top_srcdir)/include $(HPX_APPS_CFLAGS) $(APEX_CFLAGS) $(LIBHPX_CPPFLAGS)
libapex_hpx_la_SOURCES 		= apex_global_hpx.c
libapex_hpx_la_LIBADD  		= $(HPX_APPS_LDADD) 
libapex_hpx_la_DEPENDENCIES	= $(HPX_APPS_DEPS)
