# -*- autoconf -*---------------------------------------------------------------
# HPX_AARCH64_GET_CACHELINESIZE
# 
# Get the cachelinesize for aarch64.
#
# Defines
#   HAVE_AARCH64
#
# Variables
#   l1d_linesize
#   
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_AARCH64_GET_CACHELINESIZE], [
 AC_MSG_CHECKING([verifying cachlinesize=64 for aarch64])
 AC_LANG_PUSH([C])
 AC_RUN_IFELSE(
   [AC_LANG_PROGRAM([[#include <stdio.h>]],
                    [[unsigned int cache_info = 0;
                      unsigned int l1d_linesize;
                      asm volatile ("mrs\t%0, ctr_el0":"=r" (cache_info));
                      l1d_linesize = 4 << ((cache_info >> 16) & 0xF);
                      return (l1d_linesize == 64) ? 0 : 1;
                    ]])],
   [AC_MSG_RESULT([yes])
    l1d_linesize=64],
   [AC_MSG_RESULT([no])])
 AC_LANG_POP([C])
])
