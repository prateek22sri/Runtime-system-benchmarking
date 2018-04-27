# -*- autoconf -*---------------------------------------------------------------
# PHOTON_DO_AM_CONDITIONALS
#
# Set automake conditionals for use in Makefile.am settings, HWLOC-style.
# ------------------------------------------------------------------------------
AC_DEFUN([PHOTON_DO_AM_CONDITIONALS], [

 AM_CONDITIONAL([OS_LINUX], [[[[ "x$host_os" = xlinux* ]]]])
 AM_CONDITIONAL([OS_DARWIN], [[[[ "x$host_os" = xdarwin* ]]]])
 AM_CONDITIONAL([CPU_X86_64], [test "x$host_cpu" = xx86_64])
 AM_CONDITIONAL([CPU_ARM], [test "x$host_cpu" = xarmv7l])
 AM_CONDITIONAL([CPU_AARCH64], [test "x$host_cpu" = xaarch64])
 AM_CONDITIONAL([CPU_POWERPC64LE], [test "x$host_cpu" = xpowerpc64le])

 AM_CONDITIONAL([GNU_PE_ENV], [test "x$photon_pe_env" = xGNU])
 AM_CONDITIONAL([CRAY_PE_ENV], [test "x$photon_pe_env" = xCRAY])
 AM_CONDITIONAL([PGI_PE_ENV], [test "x$photon_pe_env" = xPGI])
 AM_CONDITIONAL([INTEL_PE_ENV], [test "x$photon_pe_env" = xINTEL])

 AM_CONDITIONAL([BUILD_LIBFABRIC], [test "x$build_libfabric" == xyes])
 AM_CONDITIONAL([BUILD_FORWARDER], [test "x$build_forwarder" == xyes])
 AM_CONDITIONAL([BUILD_NBC], [test "x$enable_nbc" == xyes])

 AM_CONDITIONAL([HAVE_SMP], [test "x$have_smp" == xyes])
 AM_CONDITIONAL([HAVE_SMP_POSIX], [test "x$have_smp_posix" == xyes])
 AM_CONDITIONAL([HAVE_SMP_SYSV], [test "x$have_smp_sysv" == xyes])
 AM_CONDITIONAL([HAVE_CMPXCHG16B], [test "x$have_cmpxchg16b" == xyes])
 AM_CONDITIONAL([HAVE_VERBS], [test "x$have_verbs" == xyes])
 AM_CONDITIONAL([HAVE_UGNI], [test "x$have_ugni" == xyes])
 AM_CONDITIONAL([HAVE_LIBFABRIC], [test "x$have_libfabric" == xyes])
 AM_CONDITIONAL([HAVE_MPI], [test "x$have_mpi" == xyes])
 AM_CONDITIONAL([HAVE_PMI], [test "x$have_pmi" == xyes])
 AM_CONDITIONAL([HAVE_XSP], [test "x$have_xsp" == xyes])

 AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" == xyes])
])
