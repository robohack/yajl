# -*- makefile-bsdmake -*-

SUBDIR =	api
SUBDIR +=	parsing

# XXX this direct include of <bsd.own.mk> seems to be needed for sjg's Mk-files
# to see the extra SUBDIR_TARGETS defined in ../Makefile.inc (which is included
# via <init.mk>), since <subdir.mk> only includes <own.mk> _after_ the special
# subdir-* targets are defined.
#
# It should be harmless on other systems...
#
# (note in ../src/Makefile this is achieved by first including <bsd.lib.mk>)
#
.include <bsd.own.mk>

.include <bsd.subdir.mk>

# note when using MAKEOBJDIR, do a local rebuild:
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make -j 8 LDSTATIC=-static
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -r); cd $(make -v bmake_topdir) && mkdir -p ${BUILD_DIR}/$(pwd -P) && MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static")
# End:
