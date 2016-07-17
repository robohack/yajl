# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../../Makefile.inc"
.include <bsd.own.mk>

INCS =	yajl_version.h

yajl_INCS +=	yajl_parse.h
yajl_INCS +=	yajl_gen.h
yajl_INCS +=	yajl_common.h
yajl_INCS +=	yajl_tree.h

.if defined(_this_mk_dir)		# crufty.net mk-files
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
.sinclude <inc.mk>		# crufty.net mk-files
