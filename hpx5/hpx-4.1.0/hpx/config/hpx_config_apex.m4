# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_APEX([path])
#
# Variables
#   with_apex
#   have_apex
#
# Appends
#   LIBHPX_CFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_CFLAGS
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_APEX
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_APEX], [
 AC_DEFINE([HAVE_APEX], [1], [We have apex support])
 have_apex=yes
])

# Check to see if there is a version of apex in the path without requiring any
# modifications. In this case, we need to add the apex library so that we can
# link to it and make sure that any pkg-config consumers know we have apex.
AC_DEFUN([_HPX_CC_APEX], [
 AC_CHECK_HEADER([apex.h],
   [AC_CHECK_LIB([apex], [apex_init],
     [_HAVE_APEX
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lapex"
      HPX_PC_CFLAGS="$HPX_PC_CFLAGS -DHAVE_APEX"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lapex"])])
])

# Use pkg-config to find an apex module.
AC_DEFUN([_HPX_PKG_APEX], [
 pkg=$1 

 PKG_CHECK_MODULES([APEX], [$pkg],
   [_HAVE_APEX
    LIBHPX_CXXFLAGS="$LIBHPX_CXXFLAGS $APEX_CFLAGS"
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $APEX_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $APEX_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"])
])

# Process the --with-apex option.
AC_DEFUN([_HPX_WITH_APEX], [
 pkg=$1

 AS_CASE($with_apex,
   # yes and system are equivalent---we first see if apex is available in the
   # user's path, and then check to see if the default module us available. I
   # don't know how to fallthrough-case with AS_CASE, so we have the identical
   # branches. 
   [yes],    [_HPX_CC_APEX
              AS_IF([test "x$have_apex" != xyes], [_HPX_PKG_APEX($pkg)])],  
   [system], [_HPX_CC_APEX
              AS_IF([test "x$have_apex" != xyes], [_HPX_PKG_APEX($pkg)])],

   # any other string is interpreted as a custom package name for pkg-config 
   [_HPX_PKG_APEX($with_apex)])
])

AC_DEFUN([HPX_CONFIG_APEX], [
 pkg=$1

 AC_ARG_WITH(apex,
   [AS_HELP_STRING([--with-apex{=system,PKG}],
                   [How we find apex @<:@default=no@:>@])],
   [], [with_apex=no])
 
 AS_IF([test "x$with_apex" != xno], [_HPX_WITH_APEX($pkg)])
])
