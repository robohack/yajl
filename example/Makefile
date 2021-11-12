# -*- makefile-bsdmake -*-

bmake_topdir = ..

PROG = 	parse_config

MAN =	# empty

# don't install
proginstall:

regress:
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ./parse_config; fi
	ulimit -v unlimited && [ "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]
.else
	[ "$$(${.OBJDIR}/${PROG} < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]
.endif

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
