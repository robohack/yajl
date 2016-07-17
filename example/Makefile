# -*-makefile-bsdmake-*-

yajl_topdir = ..

.include "${.CURDIR}/${yajl_topdir}/Makefile.inc"

PROG = 	parse_config

MAN=	# empty

# don't install
proginstall:

regress:
	[ "$$(./parse_config < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]

.include <bsd.prog.mk>
