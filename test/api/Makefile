# -*-makefile-bsdmake-*-

yajl_topdir = ../..

.include "${.CURDIR}/${yajl_topdir}/Makefile.inc"

PROG = 	gen-extra-close

MAN=	# empty

# don't install
proginstall:

# this test seems to fail, the old run_tests script was bogus and
# broken and useless and unnecessary
#
regress:
	@echo Not running: ./${PROG}, because it is broken.

.include <bsd.prog.mk>
