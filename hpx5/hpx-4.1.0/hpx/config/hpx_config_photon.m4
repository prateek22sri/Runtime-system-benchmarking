# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_PHOTON([src-path])
#
# Make sure that a compatible installation of photon can be found.
#
# Sets
#   enable_photon
#   with_photon
#   have_photon
#   build_photon
#
# Appends
#   LIBHPX_CPPFLAGS
#   LIBHPX_LIBADD
#   HPX_APPS_LDADD
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_PHOTON
#   HAVE_NETWORK
#   HAVE_AS_GLOBAL
#   HAVE_AS_REGISTERED
# ------------------------------------------------------------------------------
AC_ARG_VAR([PHOTON_CARGS], [Additional arguments passed to Photon contrib])

AC_DEFUN([_HAVE_PHOTON], [
  AC_DEFINE([HAVE_PHOTON], [1], [Photon support available])
  AC_DEFINE([HAVE_AS_GLOBAL], [1], [We have global memory])
  AC_DEFINE([HAVE_AS_REGISTERED], [1], [We have registered memory])
  have_photon=yes
])

AC_DEFUN([_HPX_CONFIG_PHOTON], [
 contrib=$1

 # configure and build the included photon library
 HPX_MERGE_STATIC_SHARED([PHOTON_CARGS])
 # add some default CARGS
 PHOTON_CARGS="$PHOTON_CARGS LIBFABRIC_CARGS=\"$LIBFABRIC_CARGS\""
 ACX_CONFIGURE_DIR([$contrib], [$contrib], ["$PHOTON_CARGS"])
 _HAVE_PHOTON
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/$1/include"
 LIBHPX_LIBADD="$LIBHPX_LIBADD \$(top_builddir)/$1/src/libphoton.la"
 HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS photon"
])

AC_DEFUN([_HPX_PKG_PHOTON], [
 pkg=$1
 
 # search for a photon pkg-config package
 PKG_CHECK_MODULES([PHOTON], [$pkg],
   [_HAVE_PHOTON
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $PHOTON_CFLAGS"
    LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS $PHOTON_CFLAGS"
    LIBHPX_CXXFLAGS="$LIBHPX_CXXFLAGS $PHOTON_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $PHOTON_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"])
])

AC_DEFUN([_HPX_LIB_PHOTON], [
 # look for photon in the path
 AC_CHECK_HEADER([photon.h],
   [AC_CHECK_LIB([photon], [photon_init],
     [_HAVE_PHOTON
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lphoton"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lphoton"])])
])

AC_DEFUN([_HPX_WITH_PHOTON], [
 pkg=$1
 
 # handle the with_photon option, if enable_photon is selected
 AS_CASE($with_photon,
   [no], [AC_MSG_ERROR([--enable-photon=$enable_photon excludes --without-photon])],
   
   # contrib means we should just go ahead and build the library
   [contrib], [build_photon=yes],
   [yes], [build_photon=yes],
   
   # system means that we look for a library in the system path, or a
   # default-named pkg-config package
   [system], [_HPX_LIB_PHOTON
              AS_IF([test "x$with_photon" != xyes], [_HPX_PKG_PHOTON($pkg)])],

   # any other string is interpreted as a custom pkg-config package
   [_HPX_PKG_PHOTON($with_photon)])
])

AC_DEFUN([HPX_CONFIG_PHOTON], [
 contrib=$1
 pkg=$2
 
 # Allow the user to override the way we try and find photon.
 AC_ARG_ENABLE([photon],
   [AS_HELP_STRING([--enable-photon],
                   [Enable the photon network @<:@default=no@:>@])],
   [], [enable_photon=no])

 AC_ARG_WITH(photon,
   [AS_HELP_STRING([--with-photon{=contrib,system,PKG}],
                   [How we find photon @<:@default=contrib@:>@])],
   [], [with_photon=contrib])

 AS_IF([test "x$enable_photon" != xno], [_HPX_WITH_PHOTON($pkg)])
 AS_IF([test "x$build_photon" == xyes], [_HPX_CONFIG_PHOTON($contrib)])
])
