# -*- makefile-bsdmake -*-

bmake_topdir = ../..

yajl_INCS +=	yajl_parse.h
yajl_INCS +=	yajl_gen.h
yajl_INCS +=	yajl_common.h
yajl_INCS +=	yajl_tree.h

.if defined(_this_mk_dir)		# special for crufty.net mk-files
INCS += ${yajl_INCS:S|^|${.CURDIR}/|}
.else
INCS += ${yajl_INCS}
.endif

INCS +=		yajl_version.h
CLEANFILES +=	yajl_version.h

.if target(install)
# get includes to install at install time
install:	includes
.endif

.include <bsd.prog.mk>			# bsd.inc.mk is insufficient (no ../Makefile.inc)
.sinclude <inc.mk>			# hack for crufty.net mk-files

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
