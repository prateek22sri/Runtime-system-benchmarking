# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_PMI([pkg-config])
# ------------------------------------------------------------------------------
# Configure with support for PMI network bootstrapping.
#
# Variables
#   enable_pmi
#   with_pmi
#   have_pmi
#   have_pmi_cray_ext
#
# Appends
#   LIBPHOTON_CFLAGS
#   LIBPHOTON_LIBS
#   PHOTON_PC_PRIVATE_PKGS
#   PHOTON_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_PMI
#   HAVE_PMI_CRAY_EXT
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_PMI], [
 AC_DEFINE([HAVE_PMI], [1], [Enable PMI])
 have_pmi=yes
])

AC_DEFUN([_HAVE_PMI_CRAY_EXT], [
 AC_DEFINE([HAVE_PMI_CRAY_EXT], [1], [Enable Cray PMI])
 have_pmi_cray_ext=yes
])

AC_DEFUN([_PHOTON_CC_PMI], [
 # check if the compiler just supports PMI without a package
 AC_CHECK_HEADER([pmi.h],
   [AC_CHECK_LIB([pmi], [PMI_Init],
     [LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lpmi"
      PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lpmi"])])

 AC_CHECK_HEADER([pmi_cray_ext.h], [_HAVE_PMI_CRAY_EXT])
])

AC_DEFUN([_PHOTON_PKG_PMI], [
 pkg=$1
 
 # try to find PMI through a package
 PKG_CHECK_MODULES([PMI], [$pkg],
   [_HAVE_PMI
    LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $PMI_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $PMI_LIBS"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS $pkg"])

 # check for cray extensions
 old_CPPFLAGS=$CPPFLAGS
 CPPFLAGS="$CPPFLAGS $PMI_CFLAGS"
 AC_CHECK_HEADER([pmi_cray_ext.h], [_HAVE_PMI_CRAY_EXT])
 CPPFLAGS=$old_CPPFLAGS
])

AC_DEFUN([_PHOTON_WITH_PMI], [
 pkg=$1
 
 # handle with_pmi when enable_pmi is set
 AS_CASE($with_pmi,
   [no],     [AC_MSG_ERROR([--enable-pmi=$enable_pmi excludes --without-pmi])],
   [system], [_PHOTON_CC_PMI
              AS_IF([test "x$have_pmi" != xyes], [_PHOTON_PKG_PMI($pkg)])],
   [yes],    [_PHOTON_CC_PMI
              AS_IF([test "x$have_pmi" != xyes], [_PHOTON_PKG_PMI($pkg)])],
   [_PHOTON_PKG_PMI($with_pmi)])
])

AC_DEFUN([PHOTON_CONFIG_PMI], [
 pkg=$1
 
 # Allow the user to select if we should use PMI or not
 AC_ARG_ENABLE([pmi],
   [AS_HELP_STRING([--enable-pmi],
                   [Enable PMI support @<:@default=no@:>@])],
   [], [enable_pmi=no])

 AC_ARG_WITH(pmi,
   [AS_HELP_STRING([--with-pmi{=system,PKG}],
                   [How we find PMI @<:@default=system,$pkg@:>@])],
   [], [with_pmi=system])

 AS_IF([test "x$enable_pmi" != xno], [_PHOTON_WITH_PMI($pkg)])
])
