3AC_PREREQ([2.63])
AC_INIT([photon], [1.1], [photon@crest.iu.edu])
CLEANFILES="*~ .\#*"
AC_SUBST(CLEANFILES)

AC_CONFIG_SRCDIR([include/config.h.in])
AM_CONFIG_HEADER([include/config.h])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([config])
AC_USE_SYSTEM_EXTENSIONS
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

# Initialize automake and libtool
AM_INIT_AUTOMAKE([1.9 dist-bzip2 subdir-objects foreign tar-ustar -Wall -Werror])
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
LT_PREREQ([2.2.6])
LT_INIT

AC_PROG_CC_C99
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_AS

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_CONST
AC_TYPE_PID_T

# Initialize our build flags based on the programming environemnt.
#
# LIBPHOTON_CPPFLAGS: CPPFLAGS required to build libphoton.la
# LIBPHOTON_CFLAGS: CFLAGS required to build libphoton.la
# LIBPHOTON_CXXFLAGS: CXXFLAGS required to build libphoton.la
# LIBPHOTON_LIBADD: .la dependencies required to build libphoton.la
# LIBPHOTON_LDFLAGS: -L and -Wl flags needed to link installed libphoton
# LIBPHOTON_LIBS: -l dependencies required to link installed libphoton
#
# PHOTON_APPS_CPPFLAGS: CPPFLAGS required to link to build-tree libphoton.la
# PHOTON_APPS_CFLAGS: CFLAGS required to link to build-tree libphoton.la
# PHOTON_APPS_LDFLAGS: LDFLAGS required to link to build-tree libphoton.la
# PHOTON_APPS_LDADD: .la libraries require to link build-tree libphoton.la
# PHOTON_APPS_DEPS: .la library dependendencies for apps
#
# PHOTON_PC_CFLAGS: C and CPP flags (typically just -D) for installed libhps.so
# PHOTON_PC_PUBLIC_LIBS: -L and -l flags required to link to installed libphoton.so
# PHOTON_PC_REQUIRES_PKGS: packages required to link to installed libphoton.so
# PHOTON_PC_PRIVATE_LIBS: -L and -l flags required to link to installed libphoton.a
# PHOTON_PC_PRIVATE_PKGS: packages required to link to installed libphoton.a

PHOTON_CONFIG_COMPILER
PHOTON_CONFIG_HOST
PHOTON_PE_ENV

# Check for pthreads. This will set CFLAGS and LIBS as necessary in a
# platform-specific way. According to ax_pthread.m4 PTHREAD_CFLAGS are meant to
# be on the link line too, so add them for pkg-config static linking.
AX_PTHREAD(
 [LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS $PTHREAD_CFLAGS"
  LIBPHOTON_LIBS="$LIBPHOTON_LIBS $PTHREAD_LIBS"
  PHOTON_PC_PRIVATE_LIBS="$PTHREAD_CFLAGS $PHOTON_PC_PRIVATE_LIBS $PTHREAD_LIBS"],
 [AC_MSG_ERROR([Could not find pthread implementation])])

# ---------------------------------------------------------------------------
# --enable options
# ---------------------------------------------------------------------------
AC_ARG_ENABLE([testsuite],
  [AS_HELP_STRING([--enable-testsuite],
                  [Enable test suite (requires check) @<:@default=no@:>@])],
  [], [enable_testsuite=no])

AC_ARG_ENABLE([debug],
  [AS_HELP_STRING([--enable-debug],
                  [Enable debug logging @<:@default=no@:>@])],
  [], [enable_debug=no])
AS_IF([test "x$enable_debug" != xno],
  [AC_DEFINE([ENABLE_DEBUG], [1], [Enable debug logging])])

AC_ARG_ENABLE([calltrace],
  [AS_HELP_STRING([--enable-calltrace],
                  [Enable call tracing to file @<:@default=no@:>@])],
  [], [enable_calltrace=no])
AS_IF([test "x$enable_calltrace" != xno],
  [AC_DEFINE([ENABLE_CALLTRACE], [1], [Enable call tracing])])

AC_ARG_ENABLE([nbc],
  [AS_HELP_STRING([--enable-nbc],
                  [Enable non-blocking collective (NBC) support @<:@default=no@:>@])],
  [LIBPHOTON_LIBADD="$LIBPHOTON_LIBADD \$(top_builddir)/src/nbc/libnbcpwc.la"
   LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/nbc"
   LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/nbc/ph"],
  [enable_nbc=no])
AS_IF([test "x$enable_nbc" != xno],
  [AC_DEFINE([ENABLE_NBC], [1], [Enable NBC])])

PHOTON_CONFIG_SMP([rt])
PHOTON_CONFIG_VERBS([verbs])
PHOTON_CONFIG_UGNI([cray-ugni])
PHOTON_CONFIG_LIBFABRIC([src/contrib/libfabric], [libfabric])
PHOTON_CONFIG_PMI([cray-pmi])
PHOTON_CONFIG_MPI([ompi])
#PHOTON_CONFIG_XSP([xsp])
#PHOTON_CONFIG_FORWARDER([forwarder])

# Add some known include paths
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/include"
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/util"
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/phcore"
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/forwarder"
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/contrib"
LIBPHOTON_CFLAGS="$LIBPHOTON_CFLAGS -I\$(top_srcdir)/src/contrib/libsync/include"

# ---------------------------------------------------------------------------
# --with-options, see config/photon_with_pkg.m4 for details
# ---------------------------------------------------------------------------
PHOTON_WITH_PKG([check],[check],[build with check unit tests],[no],[CHECK])
AS_IF([test "x$enable_testsuite" != xno],
  [enable_tests=yes],
  [enable_tests=no])

# Configure all of the automake conditionals that our build requires.
PHOTON_DO_AM_CONDITIONALS

# Substitute the variables required for libtool linking of libphoton internally.
AC_SUBST(LIBPHOTON_CPPFLAGS)
AC_SUBST(LIBPHOTON_CFLAGS)
AC_SUBST(LIBPHOTON_CXXFLAGS)
AC_SUBST(LIBPHOTON_CCASFLAGS)
AC_SUBST(LIBPHOTON_LDFLAGS)
AC_SUBST(LIBPHOTON_LIBADD)
AC_SUBST(LIBPHOTON_LIBS)

# Substitute the variables required for pkg-config linking of libphoton
# externally. Note if we're only building libphoton.a then the --libs output must
# contain all private dependencies as well.
AS_IF([test "x$enable_shared" != xyes],
  [PHOTON_PC_PUBLIC_LIBS="$PHOTON_PC_PUBLIC_LIBS $PHOTON_PC_PRIVATE_LIBS"
   PHOTON_PC_REQUIRES_PKGS="$PHOTON_PC_REQUIRES_PKGS $PHOTON_PC_PRIVATE_PKGS"])

AC_SUBST(PHOTON_PC_CFLAGS)
AC_SUBST(PHOTON_PC_PUBLIC_LIBS)
AC_SUBST(PHOTON_PC_PRIVATE_LIBS)
AC_SUBST(PHOTON_PC_REQUIRES_PKGS)
AC_SUBST(PHOTON_PC_PRIVATE_PKGS)

# do substitution for the test and example makefiles
PHOTON_APPS_LDADD="\$(top_builddir)/src/libphoton.la"

AC_SUBST(PHOTON_APPS_CPPFLAGS)
AC_SUBST(PHOTON_APPS_CFLAGS)
AC_SUBST(PHOTON_APPS_LDFLAGS)
AC_SUBST(PHOTON_APPS_LDADD)
AC_SUBST(PHOTON_APPS_DEPS)

PHOTON_DO_AC_CONFIG_FILES

AC_OUTPUT

cat << EOF
Config files successfully generated by ./configure $ac_configure_args !

  Install prefix     : $prefix
    
  To build and install, run:
     $ make
     $ make install

EOF
