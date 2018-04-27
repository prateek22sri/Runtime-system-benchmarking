# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_TESTS(
#
# Configure the test suite.
# ------------------------------------------------------------------------------
AC_ARG_VAR([TESTS_CMD], [Command to run integration tests in 'make check'])

AC_DEFUN([HPX_CONFIG_TESTS], [
 AC_ARG_ENABLE([testsuite],
   [AS_HELP_STRING([--enable-testsuite], [Enable test suite @<:@default=yes@:>@])],
   [enable_tests=$enable_testsuite], [enable_tests=yes])

 AC_ARG_ENABLE([lengthy-tests],
   [AC_HELP_STRING([--enable-lengthy-tests],
                    [Enable long running tests @<:@default=no@:>@])],
   [], [enable_lengthy_tests=no])

 AS_IF([test "x$TESTS_CMD" == x -a "x$have_mpi" == xyes],
   [TESTS_CMD='mpirun -np 2 timeout 30s'])

 AS_IF([test "x$TESTS_CMD" == x -a "x$have_network" == xyes],
   [TESTS_CMD='timeout 30s'])
])

dnl # ---------------------------------------------------------------------------
dnl # --with-options, see config/hpx_with_pkg.m4 for details
dnl # --------------------------------------------------------------------------- 

dnl AC_ARG_WITH([tests-cmd],
dnl  [AS_HELP_STRING([--with-tests-cmd], [Use this command for running the test harness @<:@default=mpirun -np 2@:>@])],
dnl  [TESTS_CMD="$with_tests_cmd"], [])

