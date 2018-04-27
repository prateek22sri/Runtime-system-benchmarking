# -*- autoconf -*---------------------------------------------------------------
# HPX_DO_AC_CONFIG_FILES
#
# Use autoconf to configure all of the files that need to be processed.
# -----------------------------------------------------------------------------
AC_DEFUN([HPX_DO_AC_CONFIG_FILES], [
 AC_CONFIG_FILES([scripts/hpx-config], 
                 [chmod +x scripts/hpx-config])

 AC_CONFIG_FILES([
   hpx.pc
   Makefile
   contrib/Makefile
   contrib/lz4/Makefile
   libhpx/Makefile
   libhpx/boot/Makefile
   libhpx/actions/Makefile
   libhpx/gas/Makefile
   libhpx/gas/affinity/Makefile
   libhpx/gas/pgas/Makefile
   libhpx/gas/agas/Makefile
   libhpx/memory/Makefile
   libhpx/network/Makefile
   libhpx/network/isir/Makefile
   libhpx/network/pwc/Makefile
   libhpx/system/Makefile
   libhpx/system/linux/Makefile
   libhpx/system/darwin/Makefile
   libhpx/util/Makefile
   libhpx/scheduler/Makefile
   libhpx/scheduler/arch/Makefile
   libhpx/scheduler/lco/Makefile
   libhpx/process/Makefile
   libhpx/instrumentation/Makefile
   include/Makefile
   include/libhpx/Makefile
   include/libhpx/boot/Makefile
   include/libhpx/util/Makefile
   include/libhpx/gas/Makefile
   examples/Makefile])

 AS_IF([test "x$have_hpxpp" != xno], [
   AC_CONFIG_FILES([
     hpx++.pc
     examples/cxx/Makefile
     tests/cxx/Makefile])])
   
 AS_IF([test "x$have_docs" != xno], [
   AC_CONFIG_FILES([
     docs/Doxyfile
     docs/Makefile])])

 AS_IF([test "x$have_apex" != xno], [
   AC_CONFIG_FILES([
     libapex/Makefile])])

 AS_IF([test "x$have_percolation" != xno], [
   AC_CONFIG_FILES([
     libhpx/percolation/Makefile])])

 AS_IF([test "x$enable_tests" != xno -o "x$enable_lengthy_tests" != xno], [
   AC_CONFIG_FILES([
     tests/Makefile
     tests/unit/Makefile
     tests/perf/Makefile])])
])
