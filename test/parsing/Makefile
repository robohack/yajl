# -*- makefile-bsdmake -*-

bmake_topdir = ../..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

PROG = 	yajl_test

MAN =	# empty

# don't install
proginstall:

# XXX N.B.:  this currently creates files in the source tree, but it does clean
# them up properly if all goes well and it is not interrupted.
#
regress: all .WAIT
.if defined(USE_ASAN)
	if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
	ulimit -v unlimited && cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} sh ./run_tests.sh
.else
	cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} sh ./run_tests.sh
.endif

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# MAKEOBJDIRPREFIX=$(pwd -P)/../../build/ make regress
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
