# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_LIBFFI
#
# libffi is required by HPX for execution. HPX ships with all of its required
# dependencies, however, we prefer a system-installed version of libffi over the
# contribed version. At the same time, we will allow the user to override this
# preference using a standard --with-libffi option.
#
# Variables
#   with_libffi
#   have_libffi
#   build_libffi
#
# Appends
#   LIBHPX_CPPFLAGS
#   LIBHPX_LIBADD
#   HPX_APPS_LDADD
#   HPX_PC_REQUIRES_PKGS
#   HPX_PC_PUBLIC_LIBS
#
# Defines
#   HAVE_LIBFFI
# ------------------------------------------------------------------------------
AC_DEFUN([_HAVE_LIBFFI], [
 force_complex=$1
 AC_CHECK_LIB([ffi], [ffi_type_complex_float], [have_libffi_complex=yes])
 AS_IF([test "x$have_libffi_complex" == xyes -o "x$force_complex" == xyes], [
   AC_DEFINE([HAVE_LIBFFI_COMPLEX], [1], [We have support for libffi complex numbers])
 ])
 AC_DEFINE([HAVE_LIBFFI], [1], [We have libffi])
 have_libffi=yes
])

AC_DEFUN([_HPX_CONTRIB_LIBFFI], [
 contrib=$1

 # Configure the contributed libffi package. We install the pkg-config .pc file
 # for libffi and expose it as a public dependency of HPX, because libffi
 # symbols will appear directly in the application bindary and must be linked
 # directly to libffi, not transitively through libhpx.
 HPX_MERGE_STATIC_SHARED([LIBFFI_CARGS])
 ACX_CONFIGURE_DIR([$contrib], [$contrib], ["$LIBFFI_CARGS"])

 # We know that our contrib ffi will build with complex support, so make sure
 # that the test in HAVE_LIBFFI sets the declare correctly.
 _HAVE_LIBFFI([yes])
 
 # Add the la dependency to libhpx and make sure it can find ffi.h, and make
 # sure that the internal apps can as well. External apps will get them through
 # the installed pkg-config file.
 LIBHPX_LIBADD="$LIBHPX_LIBADD \$(top_builddir)/$contrib/libffi.la"
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_builddir)/$contrib/include $LIBFFI_CFLAGS"
 HPX_APPS_CPPFLAGS="$HPX_APPS_CPPFLAGS -I\$(top_builddir)/$contrib/include $LIBFFI_CFLAGS"

 # expose the libffi package as a public dependency to clients
 HPX_PC_REQUIRES_PKGS="$HPX_PC_REQUIRES_PKGS libffi"
])

AC_DEFUN([_HPX_LIB_LIBFFI], [
 # Search the current path for libffi, and make sure and expose libffi as a
 # public library dependency. This is necessary because application binaries
 # will contain libffi symbols and must be linked directly to libffi, rather
 # than transitively through libhpx.
 AC_CHECK_HEADER([ffi.h], [
   AC_CHECK_LIB([ffi], [ffi_raw_size],
     [_HAVE_LIBFFI
      HPX_APPS_LDADD="$HPX_APPS_LDADD -lffi"
      HPX_PC_PUBLIC_LIBS="$HPX_PC_PUBLIC_LIBS -lffi"])])
])

# This function checks to make sure that the libffi pkg-config module actually
# provides the paths and libraries required to compile and link to libffi. We
# see on cutter with the intel module and icc that it doesn't provide an include
# path for ffi.h---presumably this is a possible result elsewhere.
AC_DEFUN([_HPX_VERIFY_PKG_LIBFFI], [
 pkg=$1
 
 AC_MSG_NOTICE([Verifying the libffi package $pkg]) 
 old_CPPFLAGS="$CPPFLAGS"
 old_CFLAGS="$CFLAGS"
 CPPFLAGS="$LIBFFI_CFLAGS"
 CFLAGS="$LIBFFI_LIBS"

 # If we already ran a check for ffi.h we want to clear those results
 AS_UNSET([ac_cv_header_ffi_h])
 AS_UNSET([ac_cv_lib_ffi_ffi_raw_size])
 AC_CHECK_HEADER([ffi.h], [AC_CHECK_LIB([ffi], [ffi_raw_size], [_HAVE_LIBFFI])])
 
 CPPFLAGS="$old_CPPFLAGS"
 CFLAGS="$old_CFLAGS"
])

# This function checks to see if there is a libffi package that works.
AC_DEFUN([_HPX_PKG_LIBFFI], [
 pkg=$1

 AC_MSG_WARN([libffi packge $pkg])

 # Try and find a pkg-config package for libffi. We have found that we need some
 # special-case handling even if we find the package, because it doesn't appear
 # that the package is enough for all compilers.
 PKG_CHECK_MODULES([LIBFFI], [$pkg],
   [_HPX_VERIFY_PKG_LIBFFI([$pkg])],
   [have_libffi=no])

 # If we have verified the package, then append all the normal flags we
 # need. The libffi package is *public* in the installed package because ffi
 # symbols will appear in application binaries and must be linked directly to
 # libffi, not simply transitively through libhpx.
 AS_IF([test "x$have_libffi" == xyes],
   [# Ensure libhpx can find the ffi.h header.
    LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS $LIBFFI_CFLAGS"

    # Ensure that the included apps can find the ffi.h header (we don't use
    # pkg-config locally.
    HPX_APPS_CPPFLAGS="$HPX_APPS_CPPFLAGS $LIBFFI_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $LIBFFI_LIBS"
        
    # Ensure external users depend on the ffi pkg.
    HPX_PC_REQUIRES_PKGS="$HPX_PC_REQUIRES_PKGS $pkg"])
])

AC_DEFUN([HPX_CONFIG_LIBFFI], [
 contrib=$1
 pkg=$2
 
 # allow the user to override the way we try and find libffi.
 AC_ARG_WITH(libffi,
   [AS_HELP_STRING([--with-libffi{=system,PKG,contrib}],
                   [Control which libffi we use @<:@default=system@:>@])],
   [], [with_libffi=yes])

  AS_IF([test "x$with_libffi" == xno],
   [AC_MSG_WARN([libffi is required, defaulting --with-libffi=yes])
    with_libffi=yes])
   
 AS_CASE($with_libffi,
   [contrib], [build_libffi=yes],
     
   # system indicates that we should look for a system-installed libffi, first
   # in the current path and then as a default-named pkg-config package
   [system], [_HPX_LIB_LIBFFI
              AS_IF([test "x$have_libffi" != xyes], [_HPX_PKG_LIBFFI($pkg)])],
        
   # yes indicates that we should first look for a libffi in our path, then
   # look for a default-named pkg-config package, and then finally resort to the 
   # contributed version of libffi
   [yes], [_HPX_LIB_LIBFFI
           AS_IF([test "x$have_libffi" != xyes], [_HPX_PKG_LIBFFI($pkg)])
           AS_IF([test "x$have_libffi" != xyes], [build_libffi=yes])],
       
   # any other string is interpreted as a custom pkg-config package name to use
   [_HPX_PKG_LIBFFI($with_libffi)])

 AS_IF([test "x$build_libffi" == xyes], [_HPX_CONTRIB_LIBFFI($contrib)]) 

 AS_IF([test "x$have_libffi" != xyes],
   [AC_MSG_ERROR([Failed to find libffi for --with-libffi=$with_libffi])])
])

