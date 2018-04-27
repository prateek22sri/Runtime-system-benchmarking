# -*- autoconf -*---------------------------------------------------------------
# HPX_CHECK_CMPXCHG16B
#
# Check if architecture supports CMPXCHG16B instruction.
#
# Defines
#   HAVE_CMPXCHG16B
# ------------------------------------------------------------------------------
AC_LANG_DEFINE([_CASM], [casm], [_CASM], [CCAS], [C], [
 ac_ext=S
 ac_cpp='$CPP $CPPFLAGS'
 ac_compile='$CCAS -c $CCASFLAGS $CPPFLAGS conftest.$ac_ext >&AS_MESSAGE_LOG_FD'
 ac_link='$CCAS -o conftest$ac_exeext $CCASFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext
 $LIBS >&AS_MESSAGE_LOG_FD'
 ac_compiler_gnu=$ac_cv_ccas_compiler_gnu
])

AC_DEFUN([AC_LANG_PREPROC(_CASM)],
[AC_REQUIRE([AC_PROG_CPP])])

AC_DEFUN([AC_LANG_COMPILER(_CASM)],
[AC_REQUIRE([AC_PROG_CC])])

m4_define([AC_LANG_CONFTEST(_CASM)],
[cat confdefs.h - <<_ACEOF >conftest.$ac_ext
/* end confdefs.h.  */
$1
_ACEOF])

AC_DEFUN([HPX_CHECK_CMPXCHG16B], [
 AC_MSG_CHECKING([for support of CMPXCHG16B instruction])
 AC_LANG_PUSH([_CASM])
 AC_COMPILE_IFELSE([AC_LANG_SOURCE([[ cmpxchg16b (%rdi) ]])],
   [AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_CMPXCHG16B], [1], [Have 16-byte compare and swap])
    have_cmpxchg16b=yes],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP
])
