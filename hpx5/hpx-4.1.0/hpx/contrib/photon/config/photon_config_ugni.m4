# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_UGNI([pkg-config])
# ------------------------------------------------------------------------------
# Set up PHOTON to use ugni.
#
# Sets
#   enable_ugni
#   with_ugni
#   have_ugni
#
# Appends
#   LIBPHOTON_CFLAGS
#   LIBPHOTON_LIBS
#   PHOTON_APPS_CFLAGS
#   PHOTON_PC_PRIVATE_PKGS
#   PHOTON_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_UGNI
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_UGNI], [
  AC_DEFINE([HAVE_UGNI], [1], [UGNI support available])
  LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/src/ugni/libugni.la"
  have_ugni=yes
])

AC_DEFUN([_PHOTON_CC_UGNI], [
 # see if our CC just "knows" how to cover UGNI and RDMACM without extra work
 # (i.e., no extra CFLAGS or -lugni
 AC_MSG_CHECKING([for direct CC support for UGNI])
 AC_LANG_PUSH([C])
 AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[#include <stddef.h>
                      #include <gni_pub.h>
                    ]],
                    [[gni_cdm_handle_t handle;
		      return GNI_EpCreate(0x100, 0, 0, 0, &handle);]])],
   [AC_MSG_RESULT([yes])
    _HAVE_UGNI],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP([C])
])

AC_DEFUN([_PHOTON_LIB_UGNI], [
 # look for libugni in the path, this differs from _PHOTON_CC_UGNI in the sense that
 # it is testing -lugni, not simply expecting something like CC to do the work
 AC_CHECK_HEADER([gni_pub.h],
   [AC_CHECK_LIB([ugni], [GNI_MemRegister],
     [_HAVE_UGNI
      LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lugni"
      PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lugni"])])
])

AC_DEFUN([_PHOTON_PKG_UGNI], [
 pkg=$1
 
 # see if there is an ugni package
 PKG_CHECK_MODULES([UGNI], [$pkg],
   [_HAVE_UGNI
    LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $UGNI_CFLAGS"
    PHOTON_APPS_CFLAGS="$PHOTON_APPS_CFLAGS $UGNI_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $UGNI_LIBS"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS $pkg"],
   [enable_ugni=no])
])

AC_DEFUN([_PHOTON_WITH_UGNI], [
 pkg=$1
 
 # handle the with_ugni option if UGNI is enabled
 AS_CASE($with_ugni,
   [no],     [AC_MSG_ERROR([--enable-ugni=$enable_ugni excludes --without-ugni])],
   [system], [_PHOTON_CC_UGNI
              AS_IF([test "x$have_ugni" != xyes], [_PHOTON_LIB_UGNI])
              AS_IF([test "x$have_ugni" != xyes], [_PHOTON_PKG_UGNI($pkg)])],
   [yes],    [_PHOTON_CC_UGNI
              AS_IF([test "x$have_ugni" != xyes], [_PHOTON_LIB_UGNI])
              AS_IF([test "x$have_ugni" != xyes], [_PHOTON_PKG_UGNI($pkg)])],
   [_PHOTON_PKG_UGNI($with_ugni)])
])

AC_DEFUN([PHOTON_CONFIG_UGNI], [
 pkg=$1
 
 # Select if we want to support ugni, and how to find it if we do.
 AC_ARG_ENABLE([ugni],
   [AS_HELP_STRING([--enable-ugni],
                   [Enable UGNI support @<:@default=no@:>@])],
   [], [enable_ugni=no])

 AC_ARG_WITH(ugni,
   [AS_HELP_STRING([--with-ugni{=system,PKG}],
                   [How we find UGNI @<:@default=system,$pkg@:>@])],
   [], [with_ugni=system])

 AS_IF([test "x$enable_ugni" != xno], [_PHOTON_WITH_UGNI($pkg)]) 
])
