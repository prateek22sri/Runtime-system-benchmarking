# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_URCU([pkg-config])
# ------------------------------------------------------------------------------
#
# Appends
#   LIBHPX_CPPFLAGS
#   HPX_APPS_LDADD
#   HPX_PC_REQUIRES_PKGS
#
# Defines
#   URCU_INLINE_SMALL_FUNCTIONS
# ------------------------------------------------------------------------------

AC_DEFUN([_HAVE_URCU], [
 AC_DEFINE([URCU_INLINE_SMALL_FUNCTIONS], [1], [Optimize URCU as far as license allows])
 AC_DEFINE([HAVE_URCU], [1], [We have the urcu libraries])
 have_urcu=yes
])

AC_DEFUN([_CONTRIB_URCU], [
 contrib=$1
 pkgs=$2
 
 HPX_MERGE_STATIC_SHARED([URCU_CARGS])
 ACX_CONFIGURE_DIR([$contrib], [$contrib], ["$URCU_CARGS"])
 _HAVE_URCU
 build_urcu=yes
 
 # add the la dependency and include path for our libhpx build
 LIBHPX_LIBADD="$LIBHPX_LIBADD \$(top_builddir)/$contrib/liburcu-qsbr.la \$(top_builddir)/$contrib/liburcu-cds.la"
 LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS -I\$(top_srcdir)/$contrib/ -I\$(top_builddir)/$contrib/"

 # expose the urcu packages as a public dependency to clients
 HPX_PC_REQUIRES_PKGS="$HPX_PC_REQUIRES_PKGS $pkgs"
])

# Check to see if userspace rcu is "just available" on the system.
AC_DEFUN([_LIB_URCU], [
 AC_CHECK_HEADER([urcu-qsbr.h], [
   AC_CHECK_LIB([urcu-qsbr], [rcu_register_thread], [
     AC_CHECK_LIB([urcu-cds], [cds_lfht_new],
      [_HAVE_URCU
       HPX_APPS_LDADD="$HPX_APPS_LDADD -lurcu-qsbr -lurcu-cds"
       HPX_PC_PUBLIC_LIBS="$HPX_PC_PUBLIC_LIBS -lurcu-qsbr -lurcu-cds"])])])
])

# Try and find a package for userspace rcu.
AC_DEFUN([_PKG_URCU], [
 pkgs=$1
 
 PKG_CHECK_MODULES([URCU], [$pkgs],
   [_HAVE_URCU
    LIBHPX_CPPFLAGS="$LIBHPX_CPPFLAGS $URCU_CFLAGS"
    HPX_APPS_LDADD="$HPX_APPS_LDADD $URCU_LIBS"
    HPX_PC_PRIVATE_PKGS="$HPX_PC_PRIVATE_PKGS $pkgs"],
   [have_urcu=no])
])

AC_DEFUN([HPX_CONFIG_URCU], [
 contrib=$1
 want_urcu=$2
 pkgs="liburcu-qsbr liburcu-cds"
 
 AC_ARG_WITH(urcu,
   [AS_HELP_STRING([--with-urcu{=contrib,system,PKG}],
                   [How we find liburcu* @<:@default=system@:>@])],
   [], [with_urcu=yes])

 AS_CASE($with_urcu,
   # don't need urcu
   [no], [],
   
   # system means that we look for a library in the system path, or a
   # default-named pkg-config package
   [system|yes], [_LIB_URCU
                  AS_IF([test "x$have_urcu" != xyes], [_PKG_URCU($pkgs)])],

   # avoid using anything from the system
   [contrib], [],
   
   # any other string is interpreted as a custom pkg-config package
   [_PKG_URCU($with_urcu)])

 # if we don't have urcu but we want it then build it from contrib
 AS_IF([test "x$with_urcu" != xno -a "x$have_urcu" != xyes -a "x$want_urcu" == xyes],
   [_CONTRIB_URCU($contrib, $pkgs)])
])
