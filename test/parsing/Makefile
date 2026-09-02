# -*- makefile-bsdmake -*-

PROG = 	yajl_test

MAN =	# empty

# don't install
INTERNALPROG =	# defined
realinstall:
proginstall:

regress:
.if defined(PAXCTL) && !empty(PAXCTL) && \
    (defined(USE_ASAN) || (defined(USE_SANITIZER) && ${USE_SANITIZER} == "address"))
	if [  -x ${PAXCTL} ]; then ${PAXCTL} +a ${.OBJDIR}/${PROG}; fi
.endif
	ulimit -v unlimited && cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} OBJDIR=${.OBJDIR} sh ./run_tests.sh

.include <bsd.prog.mk>

# note when using MAKEOBJDIR, do a local rebuild:
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make -j 8 LDSTATIC=-static
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -r); cd $(make -v bmake_topdir) && mkdir -p ${BUILD_DIR}/$(pwd -P) && MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static")
# End:
