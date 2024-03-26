# -*- makefile-bsdmake -*-

yajl_INCS +=	yajl_parse.h
yajl_INCS +=	yajl_gen.h
yajl_INCS +=	yajl_common.h
yajl_INCS +=	yajl_tree.h

# special for sjg's Mk-files
#
# XXX Not needed after some point, but when????  Definitely not for macos
# bsdmake!  Probably not needed after there was a <bsd.inc.mk> symlink to
# <inc.mk>!  Try 20181221 as that's where this fails on Ubuntu-20.04.
#
# but must be avoided with pkgsrc bootstrap-mk-files...
#
.if defined(MAKE_VERSION) && (${MAKE_VERSION} > 20181221 && ${MAKE} != "bsdmake") && \
	(defined(unix) && ${unix} != "We run Unix")
INCS += ${yajl_INCS:S|^|${.CURDIR}/|}
.else
INCS += ${yajl_INCS}
.endif

INCS +=		yajl_version.h
CLEANFILES +=	yajl_version.h

.include <bsd.prog.mk>	# bsd.inc.mk is insufficient (no ../Makefile.inc)
.if !target(incinstall) && !target(inc_mkdirs)
. sinclude <inc.mk>	# hack for sjg's Mk-files, esp. pre <bsd.inc.mk> link
.endif

# For NetBSD
#
# this has to come after <bsd.prog.mk> to avoid upsetting the check in any
# system Mk-file
#
.if target(install) && target(includes)
# get includes to install at install time
install:	includes
.endif

# work-arounds for sjg's Mk-files
.if !target(.depend)
.depend:
	touch ${.TARGET}
.endif

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd ../.. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
