# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_METIS([path])
#
# Variables
#   with_metis
#   have_metis
#
# Appends
#   LIBHPX_CFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_CFLAGS
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_METIS
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_METIS], [
 AC_DEFINE([HAVE_METIS], [1], [We have metis support])
 have_metis=yes
])

# Check to see if there is a version of metis in the path without requiring any
# modifications. In this case, we need to add the metis library so that we can
# link to it and make sure that any pkg-config consumers know we have metis.
AC_DEFUN([_HPX_CC_METIS], [
  # check and see if metis is "just available" without any work
  AC_CHECK_HEADER([metis.h],
  [AC_CHECK_LIB([m], [log,ceil])
   AC_CHECK_LIB([metis], [METIS_PartGraphRecursive],
     [_HAVE_METIS
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lm -lmetis"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lm -lmetis"])])
])

# Use pkg-config to find a metis module.
AC_DEFUN([_HPX_PKG_METIS], [
 pkg=$1 

 PKG_CHECK_MODULES([METIS], [$pkg],
   [_HAVE_METIS
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $METIS_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $METIS_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"],
   [have_metis=no])
])

# Process the --with-metis option.
AC_DEFUN([_HPX_WITH_METIS], [
 pkg=$1

 AS_CASE($with_metis,
   # yes and system are equivalent---we first see if metis is available in the
   # user's path, and then check to see if the default module is available. I
   # don't know how to fallthrough-case with AS_CASE, so we have the identical
   # branches. 
   [yes],    [_HPX_CC_METIS
              AS_IF([test "x$have_metis" != xyes], [_HPX_PKG_METIS($pkg)])],  
   [system], [_HPX_CC_METIS
              AS_IF([test "x$have_metis" != xyes], [_HPX_PKG_METIS($pkg)])],

   # any other string is interpreted as a custom package name for pkg-config 
   [_HPX_PKG_METIS($with_metis)])
])

AC_DEFUN([HPX_CONFIG_METIS], [
 pkg=$1
 wanted=$2

 AC_ARG_WITH(metis,
   [AS_HELP_STRING([--with-metis{=system,PKG}],
                   [How we find the metis library @<:@default=system,$pkg@:>@])],
   [], [with_metis=system])
 
 AS_IF([test "x$wanted" != xno], [_HPX_WITH_METIS($pkg)])
])
