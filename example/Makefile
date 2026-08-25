# -*- makefile-bsdmake -*-

PROG = 	parse_config

MAN =	# empty

# don't install
INTERNALPROG =	# defined
realinstall:
proginstall:

E1 = ${.OBJDIR}/${PROG} Logging timeFormat < ${.CURDIR}/sample.config
T1 = [ $$(expr "$$(${.OBJDIR}/${PROG} Logging timeFormat < ${.CURDIR}/sample.config 2>&1)" : "Logging/timeFormat: .utc..memory leaks:.0") -eq 41 ]
E2 = echo '{broken:' | ${.OBJDIR}/${PROG} broken || true
T2 = [ $$(expr "$$(echo '{broken:' | ${.OBJDIR}/${PROG} broken 2>&1)" : "tree_parse_error: line: 1: offset: 1: lexical error: invalid char in json text.*memory leaks:.0") -eq 185 ]

regress:
.if defined(USE_ASAN) || (defined(USE_SANITIZER) && ${USE_SANITIZER} == "address")
	if [  -x ${PAXCTL} ]; then ${PAXCTL} +a ${.OBJDIR}/${PROG}; fi
.endif
	ulimit -v unlimited && ${E1}
	ulimit -v unlimited && ${T1}
	ulimit -v unlimited && ${E2}
	ulimit -v unlimited && ${T2}

.include <bsd.prog.mk>

# note when using MAKEOBJDIR, do a local rebuild:
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make -j 8 LDSTATIC=-static
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -r); cd $(make -v bmake_topdir) && mkdir -p ${BUILD_DIR} && MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static")
# End:
