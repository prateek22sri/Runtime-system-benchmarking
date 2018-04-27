# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_PARMETIS([path])
#
# Variables
#   with_parmetis
#   have_parmetis
#
# Appends
#   LIBHPX_CFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_CFLAGS
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_PARMETIS
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_PARMETIS], [
 AC_DEFINE([HAVE_PARMETIS], [1], [We have parmetis support])
 have_parmetis=yes
])

# Check to see if there is a version of parmetis in the path without requiring any
# modifications. In this case, we need to add the parmetis library so that we can
# link to it and make sure that any pkg-config consumers know we have parmetis.
AC_DEFUN([_HPX_CC_PARMETIS], [
  # check and see if parmetis is "just available" without any work
  AC_CHECK_HEADER([parmetis.h],
  [AC_CHECK_LIB([m], [log,ceil])
   AC_CHECK_LIB([parmetis], [ParMETIS_V3_PartKway],
     [_HAVE_PARMETIS
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lm -lparmetis"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lm -lparmetis"])])
])

# Use pkg-config to find a parmetis module.
AC_DEFUN([_HPX_PKG_PARMETIS], [
 pkg=$1 

 PKG_CHECK_MODULES([PARMETIS], [$pkg],
   [_HAVE_PARMETIS
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $PARMETIS_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $PARMETIS_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"],
   [have_parmetis=no])
])

# Process the --with-parmetis option.
AC_DEFUN([_HPX_WITH_PARMETIS], [
 pkg=$1

 AS_CASE($with_parmetis,
   # yes and system are equivalent---we first see if parmetis is available in the
   # user's path, and then check to see if the default module is available. I
   # don't know how to fallthrough-case with AS_CASE, so we have the identical
   # branches. 
   [yes],    [_HPX_CC_PARMETIS
              AS_IF([test "x$have_parmetis" != xyes], [_HPX_PKG_PARMETIS($pkg)])],  
   [system], [_HPX_CC_PARMETIS
              AS_IF([test "x$have_parmetis" != xyes], [_HPX_PKG_PARMETIS($pkg)])],

   # any other string is interpreted as a custom package name for pkg-config 
   [_HPX_PKG_PARMETIS($with_parmetis)])
])

AC_DEFUN([HPX_CONFIG_PARMETIS], [
 pkg=$1
 wanted=$2

 AC_ARG_WITH(parmetis,
   [AS_HELP_STRING([--with-parmetis{=system,PKG}],
                   [How we find the parmetis library @<:@default=system,$pkg@:>@])],
   [], [with_parmetis=system])
 
 AS_IF([test "x$wanted" != xno], [_HPX_WITH_PARMETIS($pkg)])
])
