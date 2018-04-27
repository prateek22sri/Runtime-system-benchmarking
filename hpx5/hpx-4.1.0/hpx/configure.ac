# Copyright (c) 2013-2017, Trustees of Indiana University 
# All rights reserved.

# This software may be modified and distributed under the terms of
# the BSD license.  See the COPYING file for details.

# This software was created at the Indiana University Center for
# Research in Extreme Scale Technologies (CREST).

AC_PREREQ([2.63])
m4_define([LIBHPX_MAJOR], [4])
m4_define([LIBHPX_MINOR], [1])
m4_define([LIBHPX_PATCH], [0])
AC_INIT([hpx-5], LIBHPX_MAJOR.LIBHPX_MINOR.LIBHPX_PATCH, [hpx@indiana.edu])

# WE OVERWRITE "-O2 -g" default flags, because there is no world where
# ./hpx/configure CFLAGS="-O2 -g" is better than ./hpx/configure CXXFLAGS="-O3
# -g"
: ${CFLAGS="-O3 -g"}
: ${CXXFLAGS="-O3 -g"}

AC_SUBST(HPX_COPYRIGHT,["Copyright (c) 2013-2017, Trustees of Indiana University."])
AC_COPYRIGHT([Copyright (c) 2013-2017, Trustees of Indiana University.])

CLEANFILES="*~ .\#*"
AC_SUBST(CLEANFILES)

# Figure out what the version of HPX is.
HPX_CONFIG_VERSION
HPX_DO_CONFIG_SCRIPT

AC_LANG([C])

# Standard preliminary autotools stuff.
AC_CONFIG_SRCDIR([include/config.h.in])
AC_CONFIG_HEADERS([include/config.h include/hpx/types.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])

# Detect the system and programming environment.
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Checks for programs that we depend on.
AC_PROG_CC_C99
AC_PROG_CXX
AC_PROG_INSTALL
AM_PROG_AS
AX_CFI_DIRECTIVES

# Initialize automake and libtool
AM_INIT_AUTOMAKE([1.11 nostdinc dist-bzip2 subdir-objects foreign tar-ustar -Wall
                  -Werror parallel-tests color-tests])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
 
# Initialize our build flags based on the programming environemnt.
#
# LIBHPX_CPPFLAGS: CPPFLAGS required to build libhpx.la
# LIBHPX_CFLAGS: CFLAGS required to build libhpx.la
# LIBHPX_CXXFLAGS: CXXFLAGS required to build libhpx.la
# LIBHPX_LIBADD: .la dependencies required to build libhpx.la
# LIBHPX_LDFLAGS: -L and -Wl flags needed to link installed libhpx
#
# HPX_APPS_CPPFLAGS: CPPFLAGS required to link to build-tree libhpx.la
# HPX_APPS_CFLAGS: CFLAGS required to link to build-tree libhpx.la
# HPX_APPS_LDFLAGS: LDFLAGS required to link to build-tree libhpx.la
# HPX_APPS_LDADD: .la and -l libraries require to link build-tree libhpx.la
# HPX_APPS_DEPS: .la library dependendencies for apps
#
# HPX_PC_CFLAGS: C and CPP flags (typically just -D) for installed libhps.so
# HPX_PC_PUBLIC_LIBS: -L and -l flags required to link to installed libhpx.so
# HPX_PC_REQUIRES_PKGS: packages required to link to installed libhpx.so
# HPX_PC_PRIVATE_LIBS: -L and -l flags required to link to installed libhpx.a
# HPX_PC_PRIVATE_PKGS: packages required to link to installed libhpx.a

# Start by adding some standard includes, the current source directory and the
# configured config.h directory.
LIBHPX_CPPFLAGS="-I\$(srcdir) -I\$(top_builddir)/include"
HPX_APPS_CPPFLAGS="-I\$(srcdir) -I\$(top_builddir)/include"

# Prefer the statically built .la libraries when they're available.
HPX_APPS_LDFLAGS="-static-libtool-libs"

HPX_CONFIG_COMPILER
HPX_CONFIG_HOST
HPX_PE_ENV

# Check for pthreads. This will set CFLAGS and LIBS as necessary in a
# platform-specific way. According to ax_pthread.m4 PTHREAD_CFLAGS are meant to
# be on the link line too, so add them for pkg-config static linking.
AX_PTHREAD(
 [LIBHPX_CFLAGS="$LIBHPX_CFLAGS $PTHREAD_CFLAGS"
  LIBHPX_APPS_LDADD="LIBHPX_APPS_LDADD $PTHREAD_LIBS"
  HPX_PC_PRIVATE_LIBS="$PTHREAD_CFLAGS $HPX_PC_PRIVATE_LIBS $PTHREAD_LIBS"],
 [AC_MSG_ERROR([Could not find pthread implementation])])

# Allow parallel configuration. We do this early so that it shows up before the
# rest of the 'enable' options. 
AC_ARG_ENABLE([parallel-config],
 [AS_HELP_STRING([--enable-parallel-config],
                 [Enable parallel configuration @<:@default=no@:>@])],
 [], [enable_parallel_config=no])
 
# If we are doing parallel config we want to make sure that ^c works and kills
# any parallel configs that are running.
AS_IF([test "x$enable_parallel_config" != xno],
 [trap "trap - SIGTERM && kill -- -$$" SIGINT SIGTERM])

# We need to enable and configure a number of packages for HPX, most of which we
# can find in the system path somewhere. We distribute a number of these
# packages with HPX in the contrib directory for cases where a system version is
# either not available or not appropriate.
#
# Required packages will have a --with-pkg{=...} option that allows the user to
# override our standard search routine. Optional packages will have an
# additional --enable-pkg option to control if we search for the package at
# all. Some more complicated packages may require more complicated options.
#
# Each package may ultimately set a number of variables, define a number of
# #defines, set some automake conditionals, and append to a number of
# substituted variables.
#
# Variables
#   with_pkg
#   have_pkg
#   build_pkg
#   enable_pkg (optional pkgs only)
#
# Defines
#   HAVE_PKG
#
# Appends
#   LIBHPX_CPPFLAGS
#   LIBHPX_CFLAGS
#   LIBHPX_CXXFLAGS
#   LIBHPX_LIBADD
#   LIBHPX_LDFLAGS
#
#   HPX_APPS_CPPFLAGS
#   HPX_APPS_CFLAGS
#
#   HPX_PC_PUBLIC_LIBS
#   HPX_PC_REQUIRES_PKGS
#   HPX_PC_PRIVATE_LIBS
#   HPX_PC_PRIVATE_PKGS
#
# The substituted variables are straightforward. The LIBHPX_ prefixed variables
# are variables required to build an link libhpx itself. The HPX_PC variables
# are replaced in the hpx.pc file to allow external tools to link with libhpx.
HPX_CONFIG_URCU([contrib/userspace-rcu], [$want_urcu])
HPX_CONFIG_HPXPP
HPX_CONFIG_PHOTON([contrib/photon], [photon])
HPX_CONFIG_MPI([ompi-cxx])
HPX_CONFIG_PMI([cray-pmi])
HPX_CONFIG_JEMALLOC([contrib/jemalloc], ["jemalloc >= 4.0"]) 
HPX_CONFIG_TBBMALLOC

HWLOC_SET_SYMBOL_PREFIX(libhpx_)
HPX_CONFIG_HWLOC_PRE_SETUP_CORE
HWLOC_SETUP_CORE([contrib/hwloc])
HPX_CONFIG_HWLOC_POST_SETUP_CORE
HPX_CONFIG_HWLOC([hwloc])

HPX_CONFIG_HUGETLBFS([hugetlbfs])
HPX_CONFIG_VALGRIND([contrib/valgrind])
HPX_CONFIG_LIBFFI([contrib/$libffi_contrib_dir], [libffi])
HPX_CONFIG_AGAS
HPX_CONFIG_METIS([metis], [$have_agas_rebalancing])
HPX_CONFIG_CITYHASH([contrib/libcuckoo])
HPX_CONFIG_LIBCUCKOO([contrib/libcuckoo], [yes])
HPX_CONFIG_PERCOLATION
HPX_CONFIG_OPENCL([opencl], [$have_percolation])
HPX_CONFIG_DOCS
HPX_CONFIG_DEBUG
HPX_CONFIG_INSTRUMENTATION
#HPX_CONFIG_PAPI([papi], [$have_instrumentation])
HPX_CONFIG_APEX([apex])
HPX_CONFIG_TESTS
HPX_CONFIG_LIBHPX

# Set and check some composite conditions to make sure the configuration makes
# sense. 
AS_IF([test "x$have_photon" == xyes -o "x$have_mpi" == xyes],
  [AC_DEFINE([HAVE_NETWORK], [1], [We have a high speed network available])
   have_network=yes])

AS_IF([test "x$have_jemalloc" == xyes -o "x$have_tbbmalloc" == xyes],
  [AC_DEFINE([HAVE_ALLOCATOR], [1], [We have a GAS allocator available])
   have_allocator=yes])

AS_IF([test "x$have_jemalloc" == xyes -a "x$have_tbbmalloc" == xyes],
  [AC_MSG_ERROR(jemalloc and tbbmalloc are mutually exclusive)])

AS_IF([test "x$have_network" == xyes -a "x$have_allocator" != xyes],
  [AC_MSG_ERROR(HPX requires jemalloc, tbbmalloc for network operation)])

AS_IF([test "x$have_agas_rebalancing" == xyes -a "x$have_agas" == xno],
  [AC_MSG_ERROR(Automatic load-balancing requires AGAS to be enabled)])

# Configure all of the automake conditionals that our build requires.
HPX_DO_AM_CONDITIONALS

# Substitute the variables required for libtool linking of libhpx internally.
AC_SUBST(LIBHPX_CPPFLAGS)
AC_SUBST(LIBHPX_CFLAGS)
AC_SUBST(LIBHPX_CXXFLAGS)
AC_SUBST(LIBHPX_CCASFLAGS)
AC_SUBST(LIBHPX_LDFLAGS)
AC_SUBST(LIBHPX_LIBADD)

AC_SUBST(HPX_PC_CFLAGS)
AC_SUBST(HPX_PC_PUBLIC_LIBS)
AC_SUBST(HPX_PC_PRIVATE_LIBS)
AC_SUBST(HPX_PC_REQUIRES_PKGS)
AC_SUBST(HPX_PC_PRIVATE_PKGS)

AC_SUBST(HPX_APPS_CPPFLAGS)
AC_SUBST(HPX_APPS_CFLAGS)
AC_SUBST(HPX_APPS_CXXFLAGS)
AC_SUBST(HPX_APPS_LDFLAGS)
AC_SUBST(HPX_APPS_LDADD)
AC_SUBST(HPX_APPS_DEPS)

HPX_DO_AC_CONFIG_FILES

AC_OUTPUT

if test "x$enable_parallel_config" != xno; then
   echo "*** Waiting for contrib's configuration to finish..."
   wait
fi

HPX_DO_SUMMARY
