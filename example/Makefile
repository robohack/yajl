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

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd)/build " (default-value 'compile-command))
# End:
