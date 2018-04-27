# -*- autoconf -*---------------------------------------------------------------
# PHOTON_CONFIG_COMPILER
#
# Perform compiler-specific work here.
#
# Variables
#   ax_cv_c_compiler_vendor
#   ax_cv_cxx_compiler_vendor
#   ax_cv_have___builtin_unreachable
#   ax_cv_have___builtin_expect
#   ax_cv_have___builtin_ctzl
#   ax_cv_have___builtin_clzl
#   ax_cv_have___builtin_clz
#   ax_cv_have___builtin_popcountl
#
# Defines
#   HAVE___BUILTIN_UNREACHABLE
#   HAVE___BUILTIN_EXPECT
#   HAVE___BUILTIN_CTZL
#   HAVE___BUILTIN_CLZL
#   HAVE___BUILTIN_CLZ
#   HAVE___BUILTIN_POPCOUNTL
# ------------------------------------------------------------------------------

AC_DEFUN([PHOTON_CONFIG_COMPILER], [
 AX_COMPILER_VENDOR
 AX_GCC_BUILTIN([__builtin_expect])
 AX_GCC_BUILTIN([__builtin_unreachable])
 AX_GCC_BUILTIN([__builtin_ctzl])
 AX_GCC_BUILTIN([__builtin_clzl])
 AX_GCC_BUILTIN([__builtin_clz])
 AX_GCC_BUILTIN([__builtin_popcountl])
])
