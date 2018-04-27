noinst_PROGRAMS = guppie fibonacci hello parspawn heat
guppie_SOURCES	= guppie.cpp
fibonacci_SOURCES = fibonacci.cpp
hello_SOURCES 	= hello.cpp
parspawn_SOURCES = parspawn.cpp
heat_SOURCES	= heat.cpp

AM_CPPFLAGS     = $(HPX_APPS_CPPFLAGS) -I$(top_srcdir)/include
AM_CXXFLAGS     = $(HPX_APPS_CXXFLAGS)
AM_LDFLAGS      = $(HPX_APPS_LDFLAGS) -no-install
LDADD           = $(HPX_APPS_LDADD)

guppie_DEPENDENCIES	     = $(HPX_APPS_DEPS)
fibonacci_DEPENDENCIES	     = $(HPX_APPS_DEPS)
hello_DEPENDENCIES	     = $(HPX_APPS_DEPS)
parspawn_DEPENDENCIES	     = $(HPX_APPS_DEPS)
heat_DEPENDENCIES	     = $(HPX_APPS_DEPS)
