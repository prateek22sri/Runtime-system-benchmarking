# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_DOCS
#
# ------------------------------------------------------------------------------
AC_ARG_VAR([DOXYGEN], [doxygen binary for building docs])

AC_DEFUN([HPX_CONFIG_DOCS], [
 AC_ARG_ENABLE([docs],
   [AS_HELP_STRING([--enable-docs], [Build documentation @<:@default=no@:>@])],
   [], [enable_docs=no])
   
 AS_IF([test "x$enable_docs" != xno],
   [AC_CHECK_PROG([DOXYGEN], [doxygen], [have_doxygen=yes],
     [enable_docs=no
      AC_MSG_WARN([Doxygen not found, disabling documentation.])])])
])
