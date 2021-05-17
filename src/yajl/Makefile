# -*- makefile-bsdmake -*-

bmake_topdir = ../..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX}
# is tedious.
#
# XXX this works, but... just use this???: . if !defined(MAKEOBJDIRPREFIX)
.if exists(${.OBJDIR}/${bmake_topdir}/src)
INCS =	yajl_version.h
.endif

yajl_INCS +=	yajl_parse.h
yajl_INCS +=	yajl_gen.h
yajl_INCS +=	yajl_common.h
yajl_INCS +=	yajl_tree.h

.if defined(_this_mk_dir)		# special for crufty.net mk-files
INCS += ${yajl_INCS:S|^|${.CURDIR}/|}
.else
INCS += ${yajl_INCS}
.endif

CLEANFILES += yajl_version.h

.if target(install)
# get includes to install at install time
install:	includes
.endif

.include <bsd.prog.mk>
.sinclude <inc.mk>			# hack for crufty.net mk-files

# get additional default targets
#
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
