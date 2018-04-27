# -*- autoconf -*---------------------------------------------------------------
# PHOTON_PE_ENV()
#
# Sets
#   photon_pe_env_cflags_pedantic
#   photon_pe_env_cflags_wall
#
# Appends
#   LIBPHOTON_CPPFLAGS (conditionally)
#   LIBPHOTON_CFLAGS (conditionally)
#   LIBPHOTON_CCASFLAGS (conditionally)
#
# Substitutes
#   PE_ENV (conditionally)
#
# ------------------------------------------------------------------------------
AC_DEFUN([_PHOTON_DO_PE_CRAY], [
 LIBPHOTON_CPPFLAGS="$LIBPHOTON_CPPFLAGS ‐h noomp"
 LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -h nomessage=1254"
 LIBPHOTON_CCASFLAGS="$LIBPHOTON_CCASFLAGS -h gnu -h nomessage=1254"
])

AC_DEFUN([_PHOTON_DO_PE_PGI], [
])

AC_DEFUN([_PHOTON_DO_PE_CLANG], [
 LIBPHOTON_CPPFLAGS="$LIBPHOTON_CPPFLAGS -Wno-gnu-zero-variadic-macro-arguments"
 photon_pe_env_cflags_pedantic="-pedantic"
 photon_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([_PHOTON_DO_PE_INTEL], [
 photon_pe_env_cflags_pedantic="-pedantic"
 photon_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([_PHOTON_DO_PE_GNU], [
 photon_pe_env_cflags_pedantic="-pedantic"
 photon_pe_env_cflags_wall="-Wall"
])

AC_DEFUN([PHOTON_PE_ENV], [
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
        [CRAY*], [_PHOTON_DO_PE_CRAY],
         [PGI*], [_PHOTON_DO_PE_PGI],
         [GNU*], [_PHOTON_DO_PE_GNU],
       [INTEL*], [_PHOTON_DO_PE_INTEL],
       [CLANG*], [_PHOTON_DO_PE_CLANG]
                 [AC_MSG_WARN([Unknown programming environment])])
])
