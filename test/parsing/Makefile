# -*-makefile-bsdmake-*-

bmake_topdir = ../..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

PROG = 	yajl_test

MAN =	# empty

# don't install
proginstall:

regress:
	cd ${.CURDIR} && testBin=${.OBJDIR}/${PROG} sh ./run_tests.sh

.include <bsd.prog.mk>
