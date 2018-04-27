#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([libPXGL], [4.1.0], [hpx-users@crest.iu.edu])
AC_CONFIG_SRCDIR([include/pxgl/sssp.h])
AC_CONFIG_HEADERS([include/config.h])
AC_CONFIG_AUX_DIR([m4])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG([C])

# Checks for programs.
AC_PROG_CC_C99

# Initialize automake and libtool                                                                                    
AM_INIT_AUTOMAKE([1.11 dist-bzip2 subdir-objects foreign tar-ustar -Wall -Werror parallel-tests color-tests])        
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])                                                                                 
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])                                                                
LT_PREREQ([2.4.6])                                                                                                   
LT_INIT()

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h limits.h stddef.h stdint.h stdlib.h string.h unistd.h])

AC_CHECK_SIZEOF([void*])
AC_CHECK_SIZEOF([size_t])
AS_IF([test "x$ac_cv_sizeof_size_t" == x4],
  [AC_DEFINE([SSSP_32BIT], [1], [Detected 32-bit platform.])],
  [AC_DEFINE([SSSP_64BIT], [1], [Detected 64-bit platform.])])

HPX_WITH_PKG([hpx],[hpx],[build with HPX.],[yes],[HPX])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_C_RESTRICT
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strtoul])

AC_CONFIG_FILES([Makefile
                 examples/Makefile
                 generatorhelper/include/Makefile
                 include/Makefile])
AC_OUTPUT
