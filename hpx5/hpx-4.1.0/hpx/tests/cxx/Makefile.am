noinst_HEADERS   = 
noinst_PROGRAMS  = global_ptr actions futures system continuations

AM_CPPFLAGS      = $(HPX_APPS_CPPFLAGS) -I$(top_srcdir)/include -Wno-unused
AM_CXXFLAGS      = $(HPX_APPS_CXXFLAGS)
AM_LDFLAGS       = $(HPX_APPS_LDFLAGS) -no-install
LDADD            = $(HPX_APPS_LDADD)
LOG_COMPILER     = $(TESTS_CMD)

TESTS = $(noinst_PROGRAMS)

global_ptr_SOURCES = global_ptr.cpp
actions_SOURCES = actions.cpp
futures_SOURCES = futures.cpp
system_SOURCES = system.cpp
continuations_SOURCES = continuations.cpp

global_ptr_DEPENDENCIES = $(HPX_APPS_DEPS)
actions_DEPENDENCIES = $(HPX_APPS_DEPS)
futures_DEPENDENCIES = $(HPX_APPS_DEPS)
system_DEPENDENCIES = $(HPX_APPS_DEPS)
continuations_DEPENDENCIES = $(HPX_APPS_DEPS)
