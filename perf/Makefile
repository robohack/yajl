# -*-makefile-bsdmake-*-

yajl_topdir = ..

.include "${.CURDIR}/${yajl_topdir}/Makefile.inc"

SRCS = perftest.c
SRCS += documents.c

PROG = 	perftest

MAN=	# empty

# don't install
proginstall:

regress:
	./perftest

.include <bsd.prog.mk>
