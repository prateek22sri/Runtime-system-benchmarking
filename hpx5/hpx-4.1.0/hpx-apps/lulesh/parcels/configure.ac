#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Lulesh (HPX-5)],[4.1.0],[hpx-users@crest.iu.edu])
AC_CONFIG_SRCDIR([lulesh-hpx.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])
AC_PROG_CC_C99
AC_PROG_CXX
AM_INIT_AUTOMAKE([no-define no-dist foreign -Wall -Werror])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_ARG_ENABLE([tracing],
  [AS_HELP_STRING([--enable-tracing], [Output trace information])],
  [], [enable_tracing=no])

AS_IF([test "x$enable_tracing" != xno],
 [AC_DEFINE(ENABLE_TRACING, [1], [Output trace data (no)])])

AC_ARG_ENABLE([serial-recv],
  [AS_HELP_STRING([--enable-serial-recv], [Serialize per-domain recvs (yes)])],
  [], [enable_serial_recv=yes])

AS_IF([test "x$enable_serial_recv" != xno],
 [AC_DEFINE(ENABLE_SERIAL_RECV, [1], [Serialize recv operations])])

AC_ARG_ENABLE([timing],
  [AS_HELP_STRING([--enable-timing], [Dump per-iteration timing information (no)])],
  [], [enable_timing=no])

AS_IF([test "x$enable_timing" != xno],
 [AC_DEFINE(ENABLE_TIMING, [1], [Dump per-iteration timing information])])

AC_ARG_WITH([hpx],
 [AS_HELP_STRING([--with-hpx[[=PKGS]]], [HPX package @<:@default=hpx@:>@])],
 [], [with_hpx=hpx])

PKG_CHECK_MODULES([HPX], [$with_hpx],
 [CFLAGS="$CFLAGS $HPX_CFLAGS"
  LIBS="$LIBS $HPX_LIBS"])

AC_CHECK_LIB([m], [cbrt,sqrt])

AM_CONDITIONAL([ENABLE_TRACING], [test "x$enable_tracing" != xno])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
