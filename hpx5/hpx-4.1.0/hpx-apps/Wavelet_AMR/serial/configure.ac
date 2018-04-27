#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([wavelet-amr (HPX-5)],[4.1.0],[hpx-users@crest.iu.edu])

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_CC_STDC
AC_PROG_INSTALL
AM_PROG_AS

# Initialize automake and libtool
AM_INIT_AUTOMAKE([no-define no-dist foreign -Wall -Werror])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

# Checks for header files.
#AC_CHECK_HEADERS([float.h limits.h stdlib.h string.h sys/time.h unistd.h])
AC_CONFIG_HEADERS([config.h])
AC_CHECK_LIB([m], [log,ceil])

AC_ARG_WITH([hpx],
     [AS_HELP_STRING([--with-hpx[[=PKGS]]], [HPX package file @<:@default=hpx@:>@])], [],
     [with_hpx=hpx])

AC_MSG_CHECKING(whether compiler understands -Wall)
old_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])],[AC_MSG_RESULT(yes)],[AC_MSG_RESULT(no)
  CFLAGS="$old_CFLAGS"])

AC_SUBST(HPX_PKG, [$with_hpx])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
