# -*- autoconf -*---------------------------------------------------------------
# HPX_WITH_PKG([WITH],[PKG_DEFAULT],[STRING],[{yes,no}],[PREFIX])
# ------------------------------------------------------------------------------
# Sets
#   $with_WITH
#
# Substitutes
#   PREFIX_CFLAGS
#   PREFIX_LIBS
#   PREFIX_PKG
#
# Defines
#   HAVE_PREFIX
#
# AM Conditionals
#   HAVE_PREFIX
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_WITH_PKG],
  [AC_ARG_WITH([$1],
     [AS_HELP_STRING([--with-$1[[=PKGS]]], [$3 @<:@default=$4@:>@])], [],
     [with_$1=$4])

   AS_IF([test "x$with_$1" = xyes], [with_$1=$2])

   AS_IF([test "x$with_$1" != xno],
     [PKG_CHECK_MODULES([$5], [$with_$1], [],
        [AC_MSG_WARN([pkg-config could not find $1=$with_$1])
         AC_MSG_WARN([falling back to {$5_CFLAGS, $5_LIBS} variables])])
      AC_SUBST($5_CFLAGS)
      AC_SUBST($5_LIBS)
      AC_SUBST($5_PKG, [$with_$1])])
      
   AS_IF([test "x$with_$1" != xno], [AC_DEFINE([HAVE_$5], [1], [$3])])

   AM_CONDITIONAL([HAVE_$5], [test "x$with_$1" != xno])])
