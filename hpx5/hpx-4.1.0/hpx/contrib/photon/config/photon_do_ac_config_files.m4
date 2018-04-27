# -*- autoconf -*---------------------------------------------------------------
# PHOTON_DO_AC_CONFIG_FILES
#
# Use autoconf to configure all of the files that need to be processed.
# -----------------------------------------------------------------------------
AC_DEFUN([PHOTON_DO_AC_CONFIG_FILES], [

 AC_CONFIG_FILES([
	Makefile
	src/contrib/Makefile
	src/contrib/bit_array/Makefile
	src/contrib/libsync/Makefile
	src/contrib/libsync/arch/Makefile
	src/util/Makefile
	src/phcore/Makefile
	src/nbc/Makefile
	src/nbc/libdict/Makefile
	src/nbc/ph/Makefile
	src/smp/Makefile
	src/verbs/Makefile
	src/ugni/Makefile
	src/fi/Makefile
	src/forwarder/Makefile
	src/Makefile
	photon.pc
 ])

 AS_IF([test "x$enable_tests" != xno],
  [AC_CONFIG_FILES([tests/Makefile])
   AS_IF([test x$have_mpi = xyes -o x$with_mpi != xno],
     [AC_CONFIG_FILES([test/Makefile])])])
])
