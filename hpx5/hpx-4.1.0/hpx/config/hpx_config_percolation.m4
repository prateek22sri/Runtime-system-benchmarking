# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_PERCOLATION
#
# Variables
#  have_percolation
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_PERCOLATION], [
 AC_ARG_ENABLE([percolation],
   [AS_HELP_STRING([--enable-percolation], [Enable percolation support @<:@default=no@:>@])],
   [], [enable_percolation=no])

 AS_IF([test "x$enable_percolation" != xno],
   [AC_DEFINE([HAVE_PERCOLATION], [1], [We have percolation])
    have_percolation=yes])  
])
