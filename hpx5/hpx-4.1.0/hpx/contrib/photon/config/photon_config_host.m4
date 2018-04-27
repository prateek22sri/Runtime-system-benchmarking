# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_HOST
#
# Perform host-specific work here.
#
# Variables
#   l1d_linesize
#   pagesize
#
# Substitutes
#
# Defines
#   PHOTON_CACHELINE_SIZE
#   HPX_CACHELINE_SIZE    (for libsync contrib)
#   PHOTON_PAGE_SIZE
#   HPX_PAGE_SIZE         (for libsync contrib)
#
# Appends
#   LIBPHOTON_LIBS
#   PHOTON_PC_PUBLIC_LIBS
# ------------------------------------------------------------------------------
AC_DEFUN([_PHOTON_DO_LINUX], [
 AS_CASE([$host_cpu],
   [x86_64], [PHOTON_CHECK_CMPXCHG16B
              l1d_linesize=`cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size`],
  [powerpc64le], [l1d_linesize=64],
  [aarch64], [PHOTON_AARCH64_GET_CACHELINESIZE
              AS_ECHO(["checking cache line size for aarch64... $l1d_linesize"])],
     [arm*], [l1d_linesize=32],
             [l1d_linesize=`getconf LEVEL1_DCACHE_LINESIZE`])
 pagesize=`getconf PAGESIZE`

 AC_DEFINE([_POSIX_C_SOURCE], [200809L], [Define the POSIX version])
 LIBPHOTON_LIBS="$LIBPHOTON_LIBS -lrt"
 PHOTON_PC_CFLAGS="$PHOTON_PC_CFLAGS -D_POSIX_C_SOURCE=200809L"
 PHOTON_PC_PUBLIC_LIBS="$PHOTON_PC_PUBLIC_LIBS -lrt"
])

AC_DEFUN([_PHOTON_DO_DARWIN], [
 l1d_linesize=`sysctl -n hw.cachelinesize`
 pagesize=`getconf PAGESIZE`
])

AC_DEFUN([PHOTON_CONFIG_HOST], [
 AS_CASE([$host_os],
    [linux*], [_PHOTON_DO_LINUX],
   [darwin*], [_PHOTON_DO_DARWIN],
              [AC_MSG_ERROR([Unsupported Host OS $host_os])])

 AC_DEFINE_UNQUOTED([PHOTON_CACHELINE_SIZE], [$l1d_linesize], [Cacheline size])
 AC_DEFINE_UNQUOTED([HPX_CACHELINE_SIZE], [$l1d_linesize], [Cacheline size])
 AC_DEFINE_UNQUOTED([PHOTON_PAGE_SIZE], [$pagesize], [OS Memory Page Size])
 AC_DEFINE_UNQUOTED([HPX_PAGE_SIZE], [$pagesize], [OS Memory Page Size])
])
