# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_LIBHPX
#
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_LIBHPX], [
  # do substitution for the test and example makefiles---make sure libhpx
  # appears first here
  HPX_APPS_LDADD="\$(top_builddir)/libhpx/libhpx.la $HPX_APPS_LDADD"
  HPX_APPS_DEPS="$HPX_APPS_DEPS \$(top_builddir)/libhpx/libhpx.la"

  # export the things requires for our C++ build
  LIBHPX_CXXFLAGS="-D__STDC_LIMIT_MACROS -D__STDC_CONSTANT_MACROS $LIBHPX_CXXFLAGS -fno-rtti"
  HPX_PC_PUBLIC_LIBS="$HPX_PC_PUBLIC_LIBS -lstdc++"
  HPX_APPS_LDADD="$HPX_APPS_LDADD -lstdc++"

  # lz4 configuration
  LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/contrib/lz4/"
  LIBHPX_LIBADD="$LIBHPX_LIBADD \$(top_builddir)/contrib/lz4/liblz4.la"
  
  # Substitute the variables required for pkg-config linking of libhpx
  # externally. Note if we're only building libhpx.a then the --libs output must
  # contain all private dependencies as well.
  AS_IF([test "x$enable_shared" != xyes],
    [HPX_PC_PUBLIC_LIBS="$HPX_PC_PUBLIC_LIBS $HPX_PC_PRIVATE_LIBS"
     HPX_PC_REQUIRES_PKGS="$HPX_PC_REQUIRES_PKGS $HPX_PC_PRIVATE_PKGS"])
])
