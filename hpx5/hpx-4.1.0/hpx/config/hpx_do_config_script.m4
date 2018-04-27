# -*- autoconf -*---------------------------------------------------------------
# HPX_DO_CONFIG_SCRIPT
#
# Set automake conditionals for use in hpx-config.
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_DO_CONFIG_SCRIPT], [
  CONFIG=`echo ${ac_configure_args} | sed -e 's#'"'"'\([^ ]*\)'"'"'#\1#g'`
  AC_SUBST([CONFIG])

  srcroot=$srcdir
  if test "x${srcroot}" = "x." ; then
    srcroot=""
  else
    srcroot="${srcroot}/"
  fi
  AC_SUBST([srcroot])
  abs_srcroot="`cd \"${srcdir}\"; pwd`/"
  AC_SUBST([abs_srcroot])

  objroot=""
  AC_SUBST([objroot])
  abs_objroot="`pwd`/"
  AC_SUBST([abs_objroot])

  if test "x$prefix" = "xNONE" ; then
    prefix="/usr/local"
  fi
  if test "x$exec_prefix" = "xNONE" ; then
    exec_prefix=$prefix
  fi
  PREFIX=$prefix
  AC_SUBST([PREFIX])
  BINDIR=`eval echo $bindir`
  BINDIR=`eval echo $BINDIR`
  AC_SUBST([BINDIR])
  INCLUDEDIR=`eval echo $includedir`
  INCLUDEDIR=`eval echo $INCLUDEDIR`
  AC_SUBST([INCLUDEDIR])
  LIBDIR=`eval echo $libdir`
  LIBDIR=`eval echo $LIBDIR`
  AC_SUBST([LIBDIR])
])
