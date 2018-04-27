# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_TBB()
# ------------------------------------------------------------------------------
# Set up HPX to use mpi. This includes both the mpi network and the mpirun.
#
# Sets
#   with_tbbroot
#   have_tbbmalloc
#
# Appends
#
# Defines
#   HAVE_TBB
# ------------------------------------------------------------------------------
AC_ARG_VAR([TBBROOT], [The TBB root installation directory])

AC_DEFUN([_HAVE_TBBMALLOC], [
  AC_DEFINE([HAVE_TBBMALLOC], [1], [We have the tbbmalloc allocator])
  have_tbbmalloc=yes
])

AC_DEFUN([_HPX_CC_TBBMALLOC], [
 AC_MSG_NOTICE([Checking for TBB with CPPFLAGS="$CPPFLAGS" and LDFLAGS="$LDFLAGS"])
 AC_CHECK_HEADER([tbb/scalable_allocator.h],
   [AC_CHECK_LIB([tbbmalloc], [scalable_malloc],
     [_HAVE_TBBMALLOC
      LIBHPX_LDFLAGS="$LIBHPX_LDFLAGS -ltbbmalloc_proxy -ltbbmalloc"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -ltbbmalloc_proxy -ltbbmalloc"])])
])

AC_DEFUN([_HPX_LIB_TBBMALLOC], [
  tbbinclude=$1
  tbblib=$2
  
  old_CPPFLAGS="$CPPFLAGS"
  CPPFLAGS="-I$tbbinclude"
  old_LDFLAGS="$LDFLAGS"
  LDFLAGS="-L$tbblib"
  _HPX_CC_TBBMALLOC
  LDFLAGS="$old_LDFLAGS"
  CPPFLAGS="$old_CPPFLAGS"
  
  AS_IF([test "x$have_tbbmalloc" == xyes],
    [LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I$tbbinclude"
     LIBHPX_LDFLAGS="-L$tbblib -R$tbblib $LIBHPX_LDFLAGS"
     HPX_APPS_LDFLAGS="-L$tbblib -R$tbblib $HPX_APPS_LDFLAGS"
     HPX_APPS_LDADD="$HPX_APPS_LDADD $TBBMALLOC_LIBS"
     HPX_PC_PUBLIC_LIBS="-L$tbblib -Wl,-rpath,$tbblib $HPX_PC_PUBLIC_LIBS"
     HPX_PC_PRIVATE_LIBS="-L$tbblib -Wl,-rpath,$tbblib $HPX_PC_PRIVATE_LIBS"])
])

AC_DEFUN([_HPX_DO_CONFIG_TBBMALLOC], [
 # CC might just "know" how to deal with tbb
 _HPX_CC_TBBMALLOC

 tbbinclude="$with_tbbroot/include"
 tbbarch="$with_tbbroot/lib/$with_tbbarch"
 AS_IF([test "x$have_tbbmalloc" != xyes],
   [AS_UNSET([ac_cv_header_tbb_scalable_allocator_h])
    AS_UNSET([ac_cv_lib_tbbmalloc_scalable_malloc])
    _HPX_LIB_TBBMALLOC($tbbinclude, "$tbbarch/gcc4.4")])

 AS_IF([test "x$have_tbbmalloc" != xyes],
   [AS_UNSET([ac_cv_header_tbb_scalable_allocator_h])
    AS_UNSET([ac_cv_lib_tbbmalloc_scalable_malloc])
    _HPX_LIB_TBBMALLOC($tbbinclude, "$tbbarch/gcc4.1")])

 AS_IF([test "x$have_tbbmalloc" != xyes],
   [AS_UNSET([ac_cv_header_tbb_scalable_allocator_h])
    AS_UNSET([ac_cv_lib_tbbmalloc_scalable_malloc])
    _HPX_LIB_TBBMALLOC($tbbinclude, "$tbbarch")])

 AS_IF([test "x$have_tbbmalloc" != xyes], [
   AC_MSG_ERROR([Could not find Intel's TBB malloc library. Try loading the
                  Intel module file and/or setting --with-tbbroot and
                  --with-tbbarch.])])
])

AC_DEFUN([HPX_CONFIG_TBBMALLOC], [
 # handle options related to tbbmalloc
 AC_ARG_ENABLE([tbbmalloc],
   [AS_HELP_STRING([--enable-tbbmalloc],
                   [Enable the tbbmalloc allocator @<:@default=no@:>@])],
   [], [enable_tbbmalloc=no])

 AC_ARG_WITH(tbbroot,
   [AS_HELP_STRING([--with-tbbroot],
                   [Path to TBB headers @<:@default=$TBBROOT@:>@])],
   [], [with_tbbroot=$TBBROOT])

 AC_ARG_WITH(tbbarch,
   [AS_HELP_STRING([--with-tbbarch={intel64,ia32,mic}],
                   [The library architecture to use with TBB])],
   [], [with_tbbarch=intel64])

 AS_IF([test "x$enable_tbbmalloc" == xyes], [_HPX_DO_CONFIG_TBBMALLOC])  
])
