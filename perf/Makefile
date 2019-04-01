# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

SRCS = perftest.c
SRCS += documents.c

PROG = 	perftest

MAN =	# empty

# don't install
proginstall:

regress:
	./perftest

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"
