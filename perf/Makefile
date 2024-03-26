# -*- makefile-bsdmake -*-

SRCS = perftest.c
SRCS += documents.c

PROG = 	perftest

MAN =	# empty

# don't install
INTERNALPROG =	# defined
realinstall:
proginstall:

regress:
.if defined(USE_ASAN)
	@if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && ${.OBJDIR}/perftest
.else
	${.OBJDIR}/perftest
.endif

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
