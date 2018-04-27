# -*- autoconf -*---------------------------------------------------------------
# HPX_PE_ENV()
#
# Sets
#   hpx_pe_env_cflags_pedantic
#   hpx_pe_env_cflags_wall
#
# Appends
#   LIBHPX_CPPFLAGS (conditionally)
#   LIBHPX_CFLAGS (conditionally)
#   LIBHPX_CCASFLAGS (conditionally)
#
# Substitutes
#   PE_ENV (conditionally)
#
# ------------------------------------------------------------------------------
AC_DEFUN([_HPX_DO_PE_CRAY], [
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS ‐h noomp"
 LIBHPX_CFLAGS="$LIBHPX_CFLAGS -h nomessage=1254"
 LIBHPX_CCASFLAGS="$LIBHPX_CCASFLAGS -h gnu -h nomessage=1254"
])

AC_DEFUN([_HPX_DO_PE_PGI], [
])

AC_DEFUN([_HPX_DO_PE_CLANG], [
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -Wno-gnu-zero-variadic-macro-arguments -Wno-unused-private-field"
 hpx_pe_env_cflags_pedantic="-pedantic"
 hpx_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([_HPX_DO_PE_INTEL], [
 # icc complains about our use of gcc memory model macros, but the complaint
 # isn't a problem or helpful, so suppress that one warning. The sync headers
 # aren't included by HPX so external users don't care.
 LIBHPX_CFLAGS="$LIBHPX_CFLAGS -wd32013"
 HPX_APPS_CFLAGS="$HPX_APPS_CFLAGS -wd32013"
 hpx_pe_env_cflags_pedantic="-pedantic"
 hpx_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([_HPX_DO_PE_GNU], [
 hpx_pe_env_cflags_pedantic="-pedantic"
 hpx_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([HPX_PE_ENV], [
 # Defaults will be overriden in a PE-specific way.
 
 # Cray uses the PE_ENV environment variable for programming environment. We
 # want to deal with this in a uniform way on all systems, so we go ahead and
 # set PE_ENV if it isn't already set.
 AC_MSG_CHECKING([for programming environment])
 AS_IF([test -z "$PE_ENV"],
    [AS_CASE([$ax_cv_c_compiler_vendor],
        [cray*], [AC_SUBST([PE_ENV], ["CRAY"])],
    [portland*], [AC_SUBST([PE_ENV], ["PGI"])],
       [intel*], [AC_SUBST([PE_ENV], ["INTEL"])],
         [gnu*], [AC_SUBST([PE_ENV], ["GNU"])],
       [clang*], [AC_SUBST([PE_ENV], ["CLANG"])],
                 [AC_SUBST([PE_ENV], ["UNKNOWN"])])])
 AC_MSG_RESULT([$PE_ENV])
 
 # Each programming environment is slightly different. Add PE-specific flags for
 # compilation.

 AS_CASE([$PE_ENV],
        [CRAY*], [_HPX_DO_PE_CRAY],
         [PGI*], [_HPX_DO_PE_PGI],
         [GNU*], [_HPX_DO_PE_GNU],
       [INTEL*], [_HPX_DO_PE_INTEL],
       [CLANG*], [_HPX_DO_PE_CLANG]
                 [AC_MSG_WARN([Unknown programming environment])])
])
