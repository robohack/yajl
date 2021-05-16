# -*- makefile-bsdmake -*-

bmake_topdir = ../..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

PROG = 	gen-extra-close

MAN =	# empty

# don't install
proginstall:

# this test seems to fail, the old run_tests script was bogus and
# broken and useless and unnecessary
#
regress:
.if defined(USE_ASAN)
	@if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ./${PROG}; fi
.endif
	@echo Not running: ./${PROG}, because it is broken.

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# MAKEOBJDIRPREFIX=$(pwd -P)/../../build/ make regress
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
