# -*- makefile-bsdmake -*-

bmake_topdir = ../..

PROG = 	gen-extra-close

MAN =	# empty

# don't install
proginstall:

# this test seems to fail, the old run_tests script was bogus and
# broken and useless and unnecessary
#
regress:
.if defined(USE_ASAN)
	@if [  -x /usr/sbin/paxctl ]; then /usr/sbin/paxctl +a ${.OBJDIR}/${PROG}; fi
.endif
	@echo Not running: ${.OBJDIR}/${PROG}, because it is broken.

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
