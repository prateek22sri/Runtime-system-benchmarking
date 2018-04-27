# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_SMP([pkg-config])
# ------------------------------------------------------------------------------
# Set up PHOTON to use SMP with some shared memory support.
#
# Sets
#   enable_smp
#   with_smp
#   have_smp
#
# Appends
#   LIBPHOTON_CFLAGS
#   LIBPHOTON_LIBS
#   PHOTON_APPS_CFLAGS
#   PHOTON_PC_PRIVATE_PKGS
#   PHOTON_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_SMP
#   HAVE_SPM_POSIX
#   HAVE_SMP_SYSV
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_SMP], [
  AC_DEFINE([HAVE_SMP], [1], [SMP support available])
  LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/src/smp/libsmp.la"
  have_smp=yes
])

AC_DEFUN([_HAVE_SMP_POSIX], [
  AC_DEFINE([HAVE_SMP_POSIX], [1], [SMP POSIX shmem support available])
  have_smp_posix=yes
])

AC_DEFUN([_HAVE_SMP_SYSV], [
  AC_DEFINE([HAVE_SMP_SYSV], [1], [SMP SYSV shmem support available])
  have_smp_sysv=yes
])

AC_DEFUN([_PHOTON_CC_SMP], [
 # see if our CC just "knows" how to cover SMP without extra work
 AC_MSG_CHECKING([for direct CC support for POSIX shmem])
 AC_LANG_PUSH([C])
 AC_LINK_IFELSE(
   [AC_LANG_PROGRAM([[
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <sys/mman.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <stdlib.h>
   ]],
   [[
	int size = 4096;
        int fd;
        void * addr;

        fd = shm_open("/config_test", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        (void)shm_unlink("/config_test");
        if (fd < 0) return 1;
        if (ftruncate(fd, size) < 0) return 1;
        addr = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
        (void)close(fd);
        if (!addr || (addr == MAP_FAILED)) return 1;
        (void)munmap(addr, size);
        return 0;
   ]])],
   [AC_MSG_RESULT([yes])
    _HAVE_SMP_POSIX],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP([C])

 AS_IF([test "$have_smp_posix" = "yes" -o "$have_smp_sysv" = "yes"],
       [_HAVE_SMP])
])

AC_DEFUN([_PHOTON_LIB_SMP], [
 # look for shmem support in the path
 AC_CHECK_HEADER([sys/mman.h],
   [AC_CHECK_LIB([rt], [shm_open],
     [_HAVE_SMP_POSIX
      LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lrt"
      PHOTON_PC_PRIVATE_LIBS="$PHOTON_PC_PRIVATE_LIBS -lrt"])])

 AS_IF([test "$have_smp_posix" = "yes" -o "$have_smp_sysv" = "yes"],
       [_HAVE_SMP])
])

AC_DEFUN([_PHOTON_PKG_SMP], [
 pkg=$1
 
 # see if there is some shared mem package
 PKG_CHECK_MODULES([SMP], [$pkg],
   [LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $SMP_CFLAGS"
    PHOTON_APPS_CFLAGS="$PHOTON_APPS_CFLAGS $SMP_CFLAGS"
    LIBPHOTON_LIBS="$LIBPHOTON_LIBS $SMP_LIBS"
    PHOTON_PC_PRIVATE_PKGS="$PHOTON_PC_PRIVATE_PKGS $pkg"],
   [enable_smp=no])
])

AC_DEFUN([_PHOTON_WITH_SMP], [
 pkg=$1
 
 # handle the with_smp option if SMP is enabled
 AS_CASE($with_smp,
   [no],     [AC_MSG_ERROR([--enable-smp=$enable_smp excludes --without-smp])],
   [system], [_PHOTON_CC_SMP
              AS_IF([test "x$have_smp" != xyes], [_PHOTON_LIB_SMP])
              AS_IF([test "x$have_smp" != xyes], [_PHOTON_PKG_SMP($pkg)])],
   [yes],    [_PHOTON_CC_SMP
              AS_IF([test "x$have_smp" != xyes], [_PHOTON_LIB_SMP])
              AS_IF([test "x$have_smp" != xyes], [_PHOTON_PKG_SMP($pkg)])],
   [_PHOTON_PKG_SMP($with_smp)])
])

AC_DEFUN([PHOTON_CONFIG_SMP], [
 pkg=$1
 
 # Select if we want to support smp, and how to find it if we do.
 AC_ARG_ENABLE([smp],
   [AS_HELP_STRING([--enable-smp],
                   [Enable SMP support @<:@default=yes@:>@])],
   [], [enable_smp=yes])

 AC_ARG_WITH(smp,
   [AS_HELP_STRING([--with-smp{=system,PKG}],
                   [How we find SMP @<:@default=system,$pkg@:>@])],
   [], [with_smp=system])

 AS_IF([test "x$enable_smp" != xno], [_PHOTON_WITH_SMP($pkg)]) 
])
