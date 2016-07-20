# -*-makefile-bsdmake-*-

yajl_topdir = ../..

.include "${.CURDIR}/${yajl_topdir}/Makefile.inc"

.include <bsd.own.mk>

# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX}
# is tedious.
#
.if exists(${.OBJDIR}/${yajl_topdir}/src)
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