# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

SRCS = perftest.c
SRCS += documents.c

PROG = 	perftest

MAN =	# empty

# don't install
proginstall:

regress: all .WAIT
.if defined(USE_ASAN)
	@if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && ${.OBJDIR}/perftest
.else
	${.OBJDIR}/perftest
.endif

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
