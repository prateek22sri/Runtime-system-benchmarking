# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_VALGRIND([path])
#
# Appends
#   LIBHPX_CPPFLAGS
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_VALGRIND],
 [LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/$1/include"])
