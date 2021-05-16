# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

PROG = 	parse_config

MAN =	# empty

# don't install
proginstall:

regress: all .WAIT
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ./parse_config; fi
	ulimit -v unlimited && [ "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]
.else
	[ "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]
.endif

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# MAKEOBJDIRPREFIX=$(pwd -P)/../build/ make regress
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
