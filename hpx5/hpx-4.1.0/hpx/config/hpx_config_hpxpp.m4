# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_HPXPP
#
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_HPXPP], [
 AC_ARG_ENABLE([hpx++],
   [AS_HELP_STRING([--enable-hpx++], [Enable HPX++ bindings (requires C++11) @<:@default=no@:>@])],
   [], [enable_hpxpp=no])

 AS_IF([test "x$enable_hpxpp" != xno],
   [AC_DEFINE([HAVE_HPXPP], [1], [We have hpx++])
    AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])
    have_hpxpp=yes])
])
