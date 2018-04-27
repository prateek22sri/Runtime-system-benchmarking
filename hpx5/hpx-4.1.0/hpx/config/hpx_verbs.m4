# -*- autoconf -*---------------------------------------------------------------
# HPX_WITH_VERBS()
# ------------------------------------------------------------------------------

AC_DEFUN([HPX_WITH_VERBS],[
  AH_TEMPLATE(HAVE_VERBS, Whether or not ibverbs is installed on the system)
        AC_CHECK_LIB(ibverbs, main, [
                AC_CHECK_HEADERS([infiniband/verbs.h], [
                        VERBS_LIBS="-libverbs"
                        have_verbs=yes
                ])
        ])
	AC_CHECK_LIB(rdmacm, main, [
                AC_CHECK_HEADERS([rdma/rdma_cma.h], [
                        VERBS_LIBS+=" -lrdmacm"
                        have_rdmacm=yes
                ])
        ])
        if test "x$have_verbs" == "xyes" && test "x$have_rdmacm" == "xyes"; then
                AC_DEFINE(HAVE_VERBS, 1, Build with VERBS+RDMACM support)
		have_vr=yes
        else
	      PKG_CHECK_MODULES([VERBS], [$with_verbs], [],
                [AC_MSG_WARN([pkg-config could not find verbs=$with_verbs])
                AC_MSG_WARN([falling back to {VERBS_CFLAGS, VERBS_LIBS} variables])])
	fi
       AC_SUBST(VERBS_CFLAGS)
       AC_SUBST(VERBS_LIBS)
       AC_SUBST(VERBS_PKG, [$with_verbs])
  AM_CONDITIONAL([HAVE_VERBS], [test x$have_vr = xyes])
])
