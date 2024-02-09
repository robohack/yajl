# -*- makefile-bsdmake -*-

bmake_topdir = ../..

PROG = 	yajl_test

MAN =	# empty

# don't install
proginstall:

regress:
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} OBJDIR=${.OBJDIR} sh ./run_tests.sh
.else
	cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} OBJDIR=${.OBJDIR} sh ./run_tests.sh
.endif

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
