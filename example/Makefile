# -*- makefile-bsdmake -*-

bmake_topdir = ..

PROG = 	parse_config

MAN =	# empty

# don't install
INTERNALPROG =	# defined
realinstall:
proginstall:

E1 = ${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config
T1 = [ $$(expr "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config 2>&1)" : "Logging/timeFormat: utc.memory leaks:.0") -eq 39 ]
E2 = echo '{broken:' | ${.OBJDIR}/${PROG}
T2 = [ $$(expr "$$(echo '{broken:' | ${.OBJDIR}/${PROG} 2>&1)" : "tree_parse_error: lexical error: invalid char in json text.*memory leaks:.0") -eq 165 ]

regress:
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && ${E1}
	ulimit -v unlimited && ${T1}
	ulimit -v unlimited && ${E2}
	ulimit -v unlimited && ${T2}
.else
	${E1}
	${T1}
	${E2}
	${T2}
.endif

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
