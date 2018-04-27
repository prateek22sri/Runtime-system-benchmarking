# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_MPI([pkg-config])
# ------------------------------------------------------------------------------
# Set up PHOTON to use mpi. This includes both the mpi network and the mpirun.
#
# Sets
#   enable_mpi
#   with_mpi
#   have_mpi
#
# Appends
#   LIBPHOTON_CFLAGS
#   LIBPHOTON_LIBS
#   PHOTON_APPS_CFLAGS
#   PHOTON_PC_PRIVATE_PKGS
#   PHOTON_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_MPI
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_MPI], [
  AC_DEFINE([HAVE_MPI], [1], [MPI support available])
  have_mpi=yes
])

AC_DEFUN([_PHOTON_CC_MPI], [
 # see if our CC just "knows" how to compile MPI without extra work (i.e., no
 # extra CFLAGS or -lmpi
 AC_MSG_CHECKING([for direct CC support for MPI])
 AC_LANG_PUSH([C])
 AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[#include <stddef.h>
                      #include <mpi.h>
                    ]],
                    [[return MPI_Init(NULL, NULL);]])],
   [AC_MSG_RESULT([yes])
    _HAVE_MPI],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP([C])
])

AC_DEFUN([_PHOTON_LIB_MPI], [
 # look for libmpi in the path, this differs from _PHOTON_CC_MPI in the sense that
 # it is testing -lmpi, not simply expecting something like mpicc to do the work
 AC_CHECK_HEADER([mpi.h],
   [AC_CHECK_LIB([mpi], [MPI_Init],
     [_HAVE_MPI
      LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lmpi"
      PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lmpi"])])
])

AC_DEFUN([_PHOTON_PKG_MPI], [
 pkg=$1
 
 # see if there is an mpi package
 PKG_CHECK_MODULES([MPI], [$pkg],
   [_HAVE_MPI
    LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $MPI_CFLAGS"
    PHOTON_APPS_CFLAGS="$PHOTON_APPS_CFLAGS $MPI_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $MPI_LIBS"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS $pkg"])
])

AC_DEFUN([_PHOTON_WITH_MPI], [
 pkg=$1
 
 # handle the with_mpi option if MPI is enabled
 AS_CASE($with_mpi,
   [no],     [AC_MSG_ERROR([--enable-mpi=$enable_mpi excludes --without-mpi])],
   [system], [_PHOTON_CC_MPI
              AS_IF([test "x$have_mpi" != xyes], [_PHOTON_LIB_MPI])
              AS_IF([test "x$have_mpi" != xyes], [_PHOTON_PKG_MPI($pkg)])],
   [yes],    [_PHOTON_CC_MPI
              AS_IF([test "x$have_mpi" != xyes], [_PHOTON_LIB_MPI])
              AS_IF([test "x$have_mpi" != xyes], [_PHOTON_PKG_MPI($pkg)])],
   [_PHOTON_PKG_MPI($with_mpi)])
])

AC_DEFUN([PHOTON_CONFIG_MPI], [
 pkg=$1
 
 # Select if we want to support mpi, and how to find it if we do.
 AC_ARG_ENABLE([mpi],
   [AS_HELP_STRING([--enable-mpi],
                   [Enable MPI support (network, mpirun) @<:@default=no@:>@])],
   [], [enable_mpi=no])

 AC_ARG_WITH(mpi,
   [AS_HELP_STRING([--with-mpi{=system,PKG}],
                   [How we find MPI @<:@default=system,$pkg@:>@])],
   [], [with_mpi=system])

 AS_IF([test "x$enable_mpi" != xno], [_PHOTON_WITH_MPI($pkg)]) 
])
