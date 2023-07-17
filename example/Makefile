# -*- makefile-bsdmake -*-

bmake_topdir = ..

PROG = 	parse_config

MAN =	# empty

# don't install
proginstall:

# xxx hmmm....  stdout and stderr are reversed
#
T1 = [ $$(expr "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config 2>&1)" : "memory leaks:.0.Logging/timeFormat: utc") -eq 39 ]
T2 = [ $$(expr "$$(echo '{broken:' | ${.OBJDIR}/${PROG} 2>&1)" : "tree_parse_error: lexical error: invalid char in json text.*memory leaks:.0") -eq 165 ]

regress:
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && ${T1}
	ulimit -v unlimited && ${T2}
.else
	${T1}
	${T2}
.endif

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
