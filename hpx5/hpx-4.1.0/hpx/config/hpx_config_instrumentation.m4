# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_INSTRUMENTATION
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_INSTRUMENTATION], [
 AC_ARG_ENABLE([instrumentation],
  [AS_HELP_STRING([--enable-instrumentation],
                  [Enable instrumentation @<:@default=no@:>@])],
  [], [enable_instrumentation=no])

 AS_IF([test "x$enable_instrumentation" != xno],
  [AC_DEFINE([ENABLE_INSTRUMENTATION], [1], [Enable instrumentationging stuff])
   have_instrumentation=yes],
 AC_CONFIG_COMMANDS([include/libhpx/event_stubs.h],
  [mkdir -p "${objroot}include/libhpx/"
   cat "${srcdir}/include/libhpx/events.def" | grep -v "//" | tr -d '\n' | sed 's/LIBHPX_EVENT/\'$'\nLIBHPX_EVENT/g' | sed 's/LIBHPX_EVENT([[ ]]*\([[a-zA-Z0-9_]]*\),[[ ]]*\([[a-zA-Z0-9_]]*\).*)/#define EVENT_\1_\2(...)/g' > "${objroot}include/libhpx/event_stubs.h"]
  [srcdir="${srcdir}"
   objroot="${objroot}"]))

 AM_CONDITIONAL([ENABLE_INSTRUMENTATION], [test "x$enable_instrumentation" != xno])
])
