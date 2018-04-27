# -*- autoconf -*---------------------------------------------------------------
# HPX_DO_AM_CONDITIONALS
#
# Set automake conditionals for use in Makefile.am settings, HWLOC-style.
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_DO_AM_CONDITIONALS], [
 HWLOC_DO_AM_CONDITIONALS

 AM_CONDITIONAL([OS_LINUX], [[[[ "x$host_os" = xlinux* ]]]])
 AM_CONDITIONAL([OS_DARWIN], [[[[ "x$host_os" = xdarwin* ]]]])
 AM_CONDITIONAL([CPU_X86_64], [test "x$host_cpu" = xx86_64])
 AM_CONDITIONAL([CPU_ARM], [test "x$host_cpu" = xarmv7l -o "x$host_cpu" = xarm])
 AM_CONDITIONAL([CPU_AARCH64], [test "x$host_cpu" = xaarch64])
 AM_CONDITIONAL([CPU_POWERPC64LE], [test "x$host_cpu" = xpowerpc64le])

 AM_CONDITIONAL([GNU_PE_ENV], [test "x$hpx_pe_env" = xGNU])
 AM_CONDITIONAL([CRAY_PE_ENV], [test "x$hpx_pe_env" = xCRAY])
 AM_CONDITIONAL([PGI_PE_ENV], [test "x$hpx_pe_env" = xPGI])
 AM_CONDITIONAL([INTEL_PE_ENV], [test "x$hpx_pe_env" = xINTEL])

 AM_CONDITIONAL([BUILD_PHOTON], [test "x$build_photon" == xyes])
 AM_CONDITIONAL([BUILD_JEMALLOC], [test "x$build_jemalloc" == xyes])
 AM_CONDITIONAL([BUILD_HWLOC], [test "x$build_hwloc" == xyes])
 AM_CONDITIONAL([BUILD_LIBFFI], [test "x$build_libffi" == xyes])   
 AM_CONDITIONAL([BUILD_LIBCUCKOO], [test "x$build_libcuckoo" == xyes])
 AM_CONDITIONAL([BUILD_URCU], [test "x$build_urcu" == xyes])
 
 AM_CONDITIONAL([HAVE_CMPXCHG16B], [test "x$have_cmpxchg16b" == xyes])
 AM_CONDITIONAL([HAVE_KNC], [test "x$pt_cv_knc_val" == xyes])
 AM_CONDITIONAL([HAVE_PHOTON], [test "x$have_photon" == xyes])
 AM_CONDITIONAL([HAVE_MPI], [test "x$have_mpi" == xyes])
 AM_CONDITIONAL([HAVE_NETWORK], [test "x$have_network" == xyes])
 AM_CONDITIONAL([HAVE_PMI], [test "x$have_pmi" == xyes])
 AM_CONDITIONAL([HAVE_JEMALLOC], [test "x$have_jemalloc" == xyes])
 AM_CONDITIONAL([HAVE_TBBMALLOC], [test "x$have_tbbmalloc" == xyes])
 AM_CONDITIONAL([HAVE_HUGETLBFS], [test "x$have_hugetlbfs" == xyes])
 AM_CONDITIONAL([HAVE_AGAS], [test "x$have_agas" == xyes])
 AM_CONDITIONAL([HAVE_LIBCUCKOO], [test "x$have_libcuckoo" == xyes])
 AM_CONDITIONAL([HAVE_REBALANCING], [test "x$have_rebalancing" == xyes])
 AM_CONDITIONAL([HAVE_METIS], [test "x$have_metis" == xyes])
 AM_CONDITIONAL([HAVE_PARMETIS], [test "x$have_parmetis" == xyes])
 AM_CONDITIONAL([HAVE_PAPI], [test "x$have_papi" == xyes])
 AM_CONDITIONAL([HAVE_APEX], [test "x$have_apex" == xyes])
 AM_CONDITIONAL([HAVE_PERCOLATION], [test "x$have_percolation" == xyes])
 AM_CONDITIONAL([HAVE_OPENCL], [test "x$have_opencl" == xyes])
 AM_CONDITIONAL([HAVE_HPXPP], [test "x$have_hpxpp" == xyes])
 AM_CONDITIONAL([HAVE_URCU], [test "x$have_urcu" == xyes])
  
 AM_CONDITIONAL([ENABLE_DOCS], [test "x$enable_docs" == xyes])
 AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" == xyes])
 AM_CONDITIONAL([ENABLE_LENGTHY_TESTS], [test "x$enable_lengthy_tests" == xyes])
])
