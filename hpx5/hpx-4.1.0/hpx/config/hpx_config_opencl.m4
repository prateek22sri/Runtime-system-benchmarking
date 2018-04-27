# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_OPENCL([pkg])
# ------------------------------------------------------------------------------
#
# Variables
#   with_opencl
#   have_opencl
#
# Appends
#   LIBHPX_CFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_OPENCL
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_OPENCL], [
 AC_DEFINE([HAVE_OPENCL], [1], [Have support for opencl])
 have_opencl=yes
])

AC_DEFUN([_HPX_CC_OPENCL], [
  # check and see if opencl is "just available" without any work
  AC_CHECK_HEADER([CL/cl.h],
    [AC_CHECK_LIB([OpenCL], [clCreateContext],
      [_HAVE_OPENCL
       HPX_APPS_LDADD="$HPX_APPS_LDADD -lOpenCL"
       HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lOpenCL"])])
])

AC_DEFUN([_HPX_PKG_OPENCL], [pkg=$1
 # check if we need to use a opencl package to access the functionality
 PKG_CHECK_MODULES([OpenCL], [$pkg],
   [_HAVE_OPENCL
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $OPENCL_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $OPENCL_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"],
   [have_opencl=no])
])

AC_DEFUN([_HPX_WITH_OPENCL], [pkg=$1
 AS_CASE($with_opencl,
   [system], [_HPX_CC_OPENCL
              AS_IF([test "x$have_opencl" != xyes], [_HPX_PKG_OPENCL($pkg)])],
      [yes], [_HPX_CC_OPENCL
              AS_IF([test "x$have_opencl" != xyes], [_HPX_PKG_OPENCL($pkg)])],
   [_HPX_PKG_OPENCL($with_opencl)])
])

AC_DEFUN([HPX_CONFIG_OPENCL], [
 pkg=$1
 wanted=$2

 # allow the programmer to select to use opencl support
 AC_ARG_WITH(opencl,
   [AS_HELP_STRING([--with-opencl{=system,PKG}],
                   [How we find OpenCL @<:@default=system,$pkg@:>@])],
   [], [with_opencl=system])

 AS_IF([test "x$wanted" == xyes], [_HPX_WITH_OPENCL(pkg)])
])
