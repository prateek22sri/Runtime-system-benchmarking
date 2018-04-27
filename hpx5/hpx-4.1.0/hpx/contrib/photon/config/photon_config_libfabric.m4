# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_LIBFABRIC([src-path])
#
# Make sure that a compatible installation of libfabric can be found.
#
# Sets
#   enable_libfabric
#   with_libfabric
#   have_libfabric
#   build_libfabric
#
# Appends
#   LIBPHOTON_CPPFLAGS
#   LIBPHOTON_LIBADD
#   LIBPHOTON_LIBS
#   PHOTON_PC_PRIVATE_LIBS
#   PHOTON_PC_PRIVATE_PKGS
#
# Defines
#   HAVE_LIBFABRIC
# ------------------------------------------------------------------------------
AC_ARG_VAR([LIBFABRIC_CARGS], [Additional arguments passed to libfabric contrib])

AC_DEFUN([_HAVE_LIBFABRIC], [
  AC_DEFINE([HAVE_LIBFABRIC], [1], [libfabric support available])
  LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/src/fi/libfi.la"
  have_libfabric=yes
])

AC_DEFUN([_PHOTON_CONFIG_LIBFABRIC], [
 contrib=$1
 
 # configure and build the included libfabric library
 PHOTON_MERGE_STATIC_SHARED([LIBFABRIC_CARGS])
 ACX_CONFIGURE_DIR([$contrib], [$contrib], ["$LIBFABRIC_CARGS"])
 _HAVE_LIBFABRIC
 LIBPHOTON_CPPFLAGS="$LIBPHOTON_CPPFLAGS -I\$(top_srcdir)/$1/include"
 LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/$1/src/libfabric.la"
 PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS libfabric"
])

AC_DEFUN([_PHOTON_PKG_LIBFABRIC], [
 pkg=$1
 
 # search for a libfabric pkg-config package
 PKG_CHECK_MODULES([LIBFABRIC], [$pkg],
   [_HAVE_LIBFABRIC
    LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $LIBFABRIC_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $LIBFABRIC_LIBS"
    PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lfabric"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS libfabric"])
])

AC_DEFUN([_PHOTON_LIB_LIBFABRIC], [
 # look for libfabric in the path
 AC_CHECK_HEADER([fabric.h],
   [AC_CHECK_LIB([libfabric], [fi_getinfo],
     [_HAVE_LIBFABRIC
      LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lfabric"
      PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lfabric"])])
])

AC_DEFUN([_PHOTON_WITH_LIBFABRIC], [
 pkg=$1
 
 # handle the with_photon option, if enable_libfabric is selected
 AS_CASE($with_libfabric,
   [no], [AC_MSG_ERROR([--enable-photon=$enable_libfabric excludes --without-photon])],
   
   # contrib means we should just go ahead and build the library
   [contrib], [build_libfabric=yes],
   [yes], [build_libfabric=yes],
   
   # system means that we look for a library in the system path, or a
   # default-named pkg-config package
   [system], [_PHOTON_LIB_LIBFABRIC
              AS_IF([test "x$with_libfabric" != xyes], [_PHOTON_PKG_LIBFABRIC($pkg)])],

   # any other string is interpreted as a custom pkg-config package
   [_PHOTON_PKG_LIBFABRIC($with_libfabric)])
])

AC_DEFUN([PHOTON_CONFIG_LIBFABRIC], [
 contrib=$1
 pkg=$2
 
 # Allow the user to override the way we try and find libfabric.
 AC_ARG_ENABLE([libfabric],
   [AS_HELP_STRING([--enable-libfabric],
                   [Enable the libfabric backend @<:@default=yes@:>@])],
   [], [enable_libfabric=yes])

 AC_ARG_WITH(libfabric,
   [AS_HELP_STRING([--with-libfabric{=contrib,system,PKG}],
                   [How we find libfabric @<:@default=contrib@:>@])],
   [], [with_libfabric=contrib])

 AS_IF([test "x$enable_libfabric" != xno], [_PHOTON_WITH_LIBFABRIC($pkg)])
 AS_IF([test "x$build_libfabric" == xyes], [_PHOTON_CONFIG_LIBFABRIC($contrib)])
])
