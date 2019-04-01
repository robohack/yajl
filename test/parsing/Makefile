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
regress:
	cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} sh ./run_tests.sh

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"
