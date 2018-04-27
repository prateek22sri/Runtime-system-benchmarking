#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Tutorial (HPX-5)],[4.1.0],[hpx-users@crest.iu.edu])

# Checks for programs.
AC_PROG_CC_C99
AC_PROG_CXX

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([string])
AC_CHECK_HEADERS([iostream])

AC_ARG_WITH([hpx],
     [AS_HELP_STRING([--with-hpx[[=PKGS]]], [HPX package file @<:@default=hpx@:>@])], [],
     [with_hpx=hpx])

AC_MSG_CHECKING(whether compiler understands -Wall)
CXXFLAGS="$CXXFLAGS -std=c++11 -Wall"

AC_SUBST(HPX_PKG, [$with_hpx])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
