# -*- autoconf -*---------------------------------------------------------------
# HPX_MERGE_STATIC_SHARED([variable])
#
# For subconfigures we want to propagate the values of --enable-static and
# --enable-shared.
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_MERGE_STATIC_SHARED], [
 AS_IF([test "x$enable_static" == xyes],
   [cargs="--enable-static"],
   [cargs="--disable-static"])
 AS_IF([test "x$enable_shared" == xyes],
   [cargs="--enable-shared $cargs"],
   [cargs="--disable-shared $cargs"])

 $1="$cargs $$1"
])
