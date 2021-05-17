# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

SUBDIR =	api
SUBDIR +=	parsing

.include <bsd.subdir.mk>

# get additional default targets
#
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
