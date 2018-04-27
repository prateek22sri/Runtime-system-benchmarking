#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([Lulesh (HPX)],[4.1.0],[hpx-users@crest.iu.edu])
AC_CONFIG_SRCDIR([lulesh-hpx.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [cbrt])
AC_CHECK_LIB([m], [sqrt])

# Checks for header files.
AC_CHECK_HEADERS([float.h limits.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([sqrt])

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
