# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_HWLOC([path])
#
# HPX will use the hwloc library to deal with node-local topology. We
# prefer a system-installed version of hwloc overour contrib'ed
# version, but the --with-hwloc option can override this behavior.
#
# Variables
#   with_hwloc
#   have_hwloc
#   build_hwloc
#
# Substitutes
#   LIBHPX_CPPFLAGS
#   LIBHPX_CFLAGS
#   LIBHPX_LIBADD
#   HPX_APPS_LDADD
#   HPX_PC_PRIVATE_PKGS
#   HPX_PC_PRIVATE_LIBS
#
# Defines
#   HAVE_HWLOC
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_HWLOC], [
 AC_DEFINE([HAVE_HWLOC], [1], [We have hwloc support])
 have_hwloc=yes
])

AC_DEFUN([_BUILD_HWLOC], [
  AC_DEFINE([BUILD_HWLOC], [1], [We have hwloc support])
  build_hwloc=yes
 _HAVE_HWLOC
])

AC_DEFUN([_EMBEDDED], [
 _BUILD_HWLOC
 
 # attach the convenience library, cflags, and cppflags to build libhpx.la
 LIBHPX_CFLAGS="$LIBHPX_CFLAGS $HWLOC_EMBEDDED_CFLAGS"
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS $HWLOC_EMBEDDED_CPPFLAGS"
 LIBHPX_LIBADD="$LIBHPX_LIBADD $HWLOC_EMBEDDED_LDADD"
 HPX_APPS_LDADD="$HPX_APPS_LDADD $HWLOC_EMBEDDED_LIBS"

 # Export the hwloc flags internally in this case, because some of our tests
 # depend on the LIBHPX headers directly and need to find hwloc.h. This isn't
 # necessary for external apps because hpx.h doesn't have a public dependency on
 # hwloc.
 HPX_APPS_CPPFLAGS="$HPX_APPS_CPPFLAGS $HWLOC_EMBEDDED_CPPFLAGS"
 HPX_APPS_CFLAGS="$HPX_APPS_CFLAGS $HWLOC_EMBEDDED_CFLAGS"

 # Export hwloc lib dependencies to external clients, internal apps get these
 # through the libtool .la infrastructure.
 HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS $HWLOC_EMBEDDED_LIBS"
 
 # fix HWLOC so that it can build itself in a nostdinc automake environment
 HWLOC_CPPFLAGS="$HWLOC_EMBEDDED_CPPFLAGS -I\$(srcdir) -I\$(builddir)"
 AC_SUBST(HWLOC_CPPFLAGS)
])

AC_DEFUN([_LIB_HWLOC], [
 # check to see if there is a system-installed version of hwloc
 AC_CHECK_HEADER([hwloc.h],
   [AC_CHECK_LIB([hwloc], [hwloc_topology_init],
     [_HAVE_HWLOC
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lhwloc"
      HPX_PC_PRIVATE_LIBS="$HPX_PC_PRIVATE_LIBS -lhwloc"])])
])

AC_DEFUN([_PKG_HWLOC], [
 pkg=$1
 
 # use pkg-config to find an hwloc module
 PKG_CHECK_MODULES([HWLOC], [$pkg],
   [_HAVE_HWLOC
    LIBHPX_CFLAGS="$LIBHPX_CFLAGS $HWLOC_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $HWLOC_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkg"],
   [have_hwloc=no])
])

AC_DEFUN([HPX_CONFIG_HWLOC], [
 pkg=$2

 # allow the user to override how we find hwloc
 AC_ARG_WITH(hwloc,
   [AS_HELP_STRING([--with-hwloc{=system,PKG,contrib}],
                   [How we find hwloc @<:@default=system@:>@])],
   [], [with_hwloc=yes])

 AS_CASE($with_hwloc,
   # don't need anything special here, we'll just build embedded
   [contrib], [],

   # system indicates that we should look for a system-installed hwloc, first
   # in the current path and then as a default-named pkg-config package
   [system], [_LIB_HWLOC
              AS_IF([test "x$have_hwloc" != xyes], [_PKG_HWLOC($pkg)])],

   # yes indicates that we should first look for a hwloc in our path, then
   # look for a default-named pkg-config package, and then finally resort to the 
   # contribed version of hwloc
   [yes], [_LIB_HWLOC
           AS_IF([test "x$have_hwloc" != xyes], [_PKG_HWLOC($pkg)])],

   # any other string is interpreted as a custom pkg-config package name to use
   [_PKG_HWLOC($with_hwloc)])

 # If we haven't yet gotten an hwloc library, then use the embedded one.
 AS_IF([test "x$have_hwloc" != xyes], [_EMBEDDED])
])

AC_DEFUN([HPX_CONFIG_HWLOC_PRE_SETUP_CORE], [
 enable_libxml2=no
 enable_libnuma=no
 enable_opencl=no
 enable_cuda=no
 enable_nvml=no
 enable_gl=no
])

AC_DEFUN([HPX_CONFIG_HWLOC_POST_SETUP_CORE], [
 unset enable_libxml2
 unset enable_libnuma
 unset enable_opencl
 unset enable_cuda
 unset enable_nvml
 unset enable_gl
])
