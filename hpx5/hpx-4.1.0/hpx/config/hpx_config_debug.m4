# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_DEBUG
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_DEBUG], [
 AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug], [Enable debug code @<:@default=no@:>@])],
  [], [enable_debug=no])

 AS_IF([test "x$enable_debug" != xno],
  [LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS $hpx_pe_env_cflags_wall"
   AC_DEFINE([ENABLE_DEBUG], [1], [Enable debugging stuff])
   AC_DEFINE([ENABLE_LOGGING], [1], [Enable loggging stuff])
   enable_debug=yes])
])
