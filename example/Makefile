# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

PROG = 	parse_config

MAN =	# empty

# don't install
proginstall:

regress:
	[ "$$(./parse_config < ${.CURDIR}/sample.config)" = "Logging/timeFormat: utc" ]

.include <bsd.prog.mk>
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"
