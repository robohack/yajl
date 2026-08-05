# -*- makefile-bsdmake -*-

PROG = 	gen-extra-close

MAN =	# empty

# don't install
INTERNALPROG =	# defined
realinstall:
proginstall:

regress:
.if defined(USE_ASAN) || (defined(USE_SANITIZER) && ${USE_SANITIZER} == "address")
	@if [  -x ${PAXCTL} ]; then ${PAXCTL} +a ${.OBJDIR}/${PROG}; fi
.endif
	ulimit -v unlimited && ${.OBJDIR}/${PROG}; rc=$$?; if [ $$rc != 0 ]; then echo "rc=$$rc"; false; fi

.include <bsd.prog.mk>

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
