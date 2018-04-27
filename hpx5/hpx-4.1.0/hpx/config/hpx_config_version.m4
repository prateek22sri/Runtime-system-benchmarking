# -*- autoconf -*---------------------------------------------------------------
# HPX_CONFIG_VERSION
#
# Define the version we're building
#
# Currently, we use the `git describe` functionality to embed the git version,
# where it is available. If someone has a release tarball without the .git
# directory, we use the package version.
#
# We define HPX_VERSION and LIBHPX_VERSION independently, as we expect that
# there will be a time in the future where the HPX interface "version" is stable
# and the libhpx library version continues to evolve. At that point it won't
# make sense anymore to use the git description as the version.
# ------------------------------------------------------------------------------
AC_DEFUN([HPX_CONFIG_VERSION], [
 AS_IF([test -e "$ac_confdir/.git" ],
   [cmd="git --git-dir=$ac_confdir/.git describe --abbrev=7 --dirty --always --tags"
    HPX_VERSION=$($cmd)]
   [HPX_VERSION=$PACKAGE_VERSION])

 AC_DEFINE([LIBHPX_MAJOR_VERSION], [LIBHPX_MAJOR], [libhpx major version])
 AC_DEFINE([LIBHPX_MINOR_VERSION], [LIBHPX_MINOR], [libhpx minor version])
 AC_DEFINE([LIBHPX_PATCH_VERSION], [LIBHPX_PATCH], [libhpx patch version])
 AC_DEFINE_UNQUOTED([HPX_VERSION], ["$HPX_VERSION"], [The HPX interface version])
 AC_DEFINE_UNQUOTED([LIBHPX_VERSION], ["$HPX_VERSION"], [The libhpx library version])
 AC_SUBST(HPX_VERSION)
])
