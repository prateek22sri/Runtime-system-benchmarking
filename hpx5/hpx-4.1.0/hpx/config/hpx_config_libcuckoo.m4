# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_LIBCUCKOO([path])
#
# Variables
#   have_libcuckoo
#   build_libcuckoo
#
# Defines
#   HAVE_LIBCUCKOO
#
# Appends
#   LIBHPX_CPPFLAGS
#   LIBHPX_LIBADD
#   HPX_PC_PRIVATE_LIBS
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_CITYHASH], [
])

AC_DEFUN([HPX_CONFIG_LIBCUCKOO], [
 contrib=$1
 required=$2

 AS_IF([test "x$required" == xyes], 
   [#lubcuckoo uses ceil and hence requires libm

    AC_CHECK_LIB([m], [log,ceil])
    HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lm"
    HPX_APPS_LDADD="$HPX_APPS_LDADD -lm"

    # libcuckoo requires C++11, and hpx.h uses gnu extensions, so ask for [ext]
    HPX_MERGE_STATIC_SHARED([LIBCUCKOO_CARGS])
    AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])
    ACX_CONFIGURE_DIR([$contrib/examples/cityhash], [$contrib], ["$LIBCUCKOO_CARGS"])
    LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/$contrib/libcuckoo/"
    LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/$contrib/examples/cityhash/src"
    LIBHPX_LIBADD="$LIBHPX_LIBADD \$(top_builddir)/$contrib/src/libcityhash.la"
    HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lcityhash"
    AC_DEFINE([HAVE_LIBCUCKOO], [1], [We have the libcuckoo hash table])
    have_libcuckoo=yes
    build_libcuckoo=yes])
])
