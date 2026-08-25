# -*- makefile-bsdmake -*-

yajl_INCS +=	yajl_parse.h
yajl_INCS +=	yajl_gen.h
yajl_INCS +=	yajl_common.h
yajl_INCS +=	yajl_tree.h

# special for sjg's Mk-files
#
# XXX Not needed before some point, but when????  Definitely not for macos
# bsdmake!  Try 20181221 as that's where this fails on Ubuntu-20.04.
#
# but must be avoided with pkgsrc bootstrap-mk-files...
#
# Note: all known BMake Mk-files will define ${unix}, but normally only pkgsrc
# bootstrap-mk-files will set it to "We run Unix".  XXX Presumably if BMake was
# installed on a system where "uname -s" reported "Unix", this might fail.
# (Historical note:  in bygone days one AT&T UNIX systems the "system name",
# i.e. the _sysname_ field returned by uname(2), was considered to be the
# (unique) local name of the system, and it was frequently set to be the same as
# the _nodename_ field by "uname -S name", so even on real UNIX(tm) systems
# "uname -s" would be unlikely to ever report "Unix".)
#
.if defined(MAKE_VERSION) && ${MAKE_VERSION} > 20181221 && ${MAKE} != "bsdmake" && \
	${unix} != "We run Unix"
INCS += ${yajl_INCS:S|^|${.CURDIR}/|}
.else
INCS += ${yajl_INCS}
.endif

INCS +=		yajl_version.h
CLEANFILES +=	yajl_version.h

# sjg's Mk-files assume '-C' to copy the file(s), but '-c' is always backward
# compatible (or at least on NetBSD and FreeBSD it is! and in GNU fileutils too)
#
# xxx sgj's Mk-files (or here) could/should be smart to choose '-C' if on
# FreeBSD as there it offers some small advantage of not changing the
# modification time of the target if it already exists and is identical.
#
INC_COPY =	-c

.include <bsd.prog.mk>	# bsd.inc.mk is insufficient (no ../Makefile.inc)

# a hack for sjg's Mk-files where only <lib.mk> includes <inc.mk>, which would
# normally be sensible (programs don't normally install headers), but it is
# easier to trick <bsd.prog.mk> into not doing anything else....
.if !target(incinstall) && !target(inc_mkdirs)
. sinclude <inc.mk>
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

# note when using MAKEOBJDIR, do a local rebuild:
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make -j 8 LDSTATIC=-static
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -r); cd $(make -v bmake_topdir) && mkdir -p ${BUILD_DIR} && MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static")
# End:
