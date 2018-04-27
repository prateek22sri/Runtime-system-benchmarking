# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_VERBS([pkg-config])
# ------------------------------------------------------------------------------
# Set up PHOTON to use verbs.
#
# Sets
#   enable_verbs
#   with_verbs
#   have_verbs
#
# Appends
#   LIBPHOTON_CFLAGS
#   LIBPHOTON_LIBS
#   PHOTON_APPS_CFLAGS
#   PHOTON_PC_PRIVATE_PKGS
#   PHOTON_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_VERBS
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_VERBS], [
  AC_DEFINE([HAVE_VERBS], [1], [VERBS support available])
  LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/src/verbs/libverbs.la"
  have_verbs=yes
])

AC_DEFUN([_PHOTON_CC_VERBS], [
 # see if our CC just "knows" how to cover VERBS and RDMACM without extra work
 # (i.e., no extra CFLAGS or -lverbs
 AC_MSG_CHECKING([for direct CC support for VERBS])
 AC_LANG_PUSH([C])
 AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[#include <stddef.h>
                      #include <infiniband/verbs.h>
		      #include <rdma/rdma_cma.h>
                    ]],
                    [[struct ibv_context *ctx;
		      ibv_alloc_pd(ctx);
		      return rdma_create_event_channel();]])],
   [AC_MSG_RESULT([yes])
    _HAVE_VERBS],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP([C])
])

AC_DEFUN([_PHOTON_LIB_VERBS], [
 # look for libverbs in the path, this differs from _PHOTON_CC_VERBS in the sense that
 # it is testing -lverbs, not simply expecting something like CC to do the work
 AC_CHECK_HEADER([infiniband/verbs.h],
   [AC_CHECK_LIB([ibverbs], [ibv_reg_mr],
     [have_v=yes])])

 AC_CHECK_HEADER([rdma/rdma_cma.h],
   [AC_CHECK_LIB([rdmacm], [rdma_connect],
     [have_r=yes])])

 AS_IF([test "$have_v" = "yes" -a "$have_r" = "yes"],
       [_HAVE_VERBS
        LIBPHOTON_LIBS="$LIBPHOTON_LIBS -libverbs -lrdmacm"
	PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -libverbs -lrdmacm"])
])

AC_DEFUN([_PHOTON_PKG_VERBS], [
 pkg=$1
 
 # see if there is an verbs package
 PKG_CHECK_MODULES([VERBS], [$pkg],
   [_HAVE_VERBS
    LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $VERBS_CFLAGS"
    PHOTON_APPS_CFLAGS="$PHOTON_APPS_CFLAGS $VERBS_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $VERBS_LIBS"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS $pkg"],
   [enable_verbs=no])
])

AC_DEFUN([_PHOTON_WITH_VERBS], [
 pkg=$1
 
 # handle the with_verbs option if VERBS is enabled
 AS_CASE($with_verbs,
   [no],     [AC_MSG_ERROR([--enable-verbs=$enable_verbs excludes --without-verbs])],
   [system], [_PHOTON_CC_VERBS
              AS_IF([test "x$have_verbs" != xyes], [_PHOTON_LIB_VERBS])
              AS_IF([test "x$have_verbs" != xyes], [_PHOTON_PKG_VERBS($pkg)])],
   [yes],    [_PHOTON_CC_VERBS
              AS_IF([test "x$have_verbs" != xyes], [_PHOTON_LIB_VERBS])
              AS_IF([test "x$have_verbs" != xyes], [_PHOTON_PKG_VERBS($pkg)])],
   [_PHOTON_PKG_VERBS($with_verbs)])
])

AC_DEFUN([PHOTON_CONFIG_VERBS], [
 pkg=$1
 
 # Select if we want to support verbs, and how to find it if we do.
 AC_ARG_ENABLE([verbs],
   [AS_HELP_STRING([--enable-verbs],
                   [Enable VERBS support @<:@default=yes@:>@])],
   [], [enable_verbs=yes])

 AC_ARG_WITH(verbs,
   [AS_HELP_STRING([--with-verbs{=system,PKG}],
                   [How we find VERBS @<:@default=system,$pkg@:>@])],
   [], [with_verbs=system])

 AS_IF([test "x$enable_verbs" != xno], [_PHOTON_WITH_VERBS($pkg)]) 
])
