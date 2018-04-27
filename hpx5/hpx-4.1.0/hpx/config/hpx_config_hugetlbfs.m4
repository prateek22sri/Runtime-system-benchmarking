# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_HUGETLBFS([pkg-config])
# ------------------------------------------------------------------------------
#
# Variables
#   enable_hugetlbfs
#   with_hugetlbfs
#   have_hugetlbfs
#
# Appends
#   LIBHPX_CFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_HUGETLBFS
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_HUGETLBFS], [
 AC_DEFINE([HAVE_HUGETLBFS], [1], [Have support for hugetlbfs])
 have_hugetlbfs=yes
])

AC_DEFUN([_HPX_CC_HUGETLBFS], [
  # check and see if hugetlbfs is "just available" without any work
  AC_CHECK_HEADER([hugetlbfs.h],
    [AC_CHECK_LIB([hugetlbfs], [hugetlbfs_unlinked_fd],
      [_HAVE_HUGETLBFS
       HPX_APPS_LDADD="$HPX_APPS_LDADD -lhugetlbfs"
       HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lhugetlbfs"])])
])

AC_DEFUN([_HPX_PKG_HUGETLBFS], [pkg=$1
 # check if we need to use a hugetlbfs package to access the functionality
 PKG_CHECK_MODULES([HUGETLBFS], [$pkg],
   [_HAVE_HUTETLBFS
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $HUGETLBFS_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $HUGETLBFS_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"])
])

AC_DEFUN([_HPX_WITH_HUGETLBFS], [pkg=$1
 #
 AS_CASE($with_hugetlbfs,
   [no], [AC_MSG_ERROR([--enable-hugetlbfs=$enable_hugetlbfs excludes --without-hugetlbfs])],
   [system], [_HPX_CC_HUGETLBFS
              AS_IF([test "x$have_hugetlbfs" != xyes], [_HPX_PKG_HUGETLBFS($pkg)])],
   [yes], [_HPX_CC_HUGETLBFS
           AS_IF([test "x$have_hugetlbfs" != xyes], [_HPX_PKG_HUGETLBFS($pkg)])],
   [_HPX_PKG_HUGETLBFS($with_hugetlbfs)])
])

AC_DEFUN([HPX_CONFIG_HUGETLBFS], [pkg=$1
 # allow the programmer to select to use hugetlbfs support in the gas allocation
 AC_ARG_ENABLE([hugetlbfs],
   [AS_HELP_STRING([--enable-hugetlbfs],
                   [Enable hugetlbfs support @<:@default=no@:>@])],
   [], [enable_hugetlbfs=no])

 AC_ARG_WITH(hugetlbfs,
   [AS_HELP_STRING([--with-hugetlbfs{=system,PKG}],
                   [How we find HUGETLBFS @<:@default=system,$pkg@:>@])],
   [], [with_hugetlbfs=system])

 AS_IF([test "x$enable_hugetlbfs" != xno], [_HPX_WITH_HUGETLBFS($pkg)])
])
