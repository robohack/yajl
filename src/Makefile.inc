# -*- makefile-bsdmake -*-

# N.B.:  ${.CURDIR} here is not needed -- it's already implied because of the
# include done by <bsd.prog.mk> in the subdir's Makefile
.include "../Makefile.inc"

# XXX there's something odd about older versions of sjg's Mk-files.
#
# For some unknown reason <subdir.mk> does .-include <${.CURDIR}/Makefile.inc>.
# No other BSD-based Mk-files does this.  It is fixed since 20240414.
#
# Also until very recently BMake (and NetBSD make) didn't implement
# .INCLUDEDFROMFILE or .INCLUDEDFROMDIR correctly!  It may have worked a long
# time ago, but was broken for some time.  This was probably fixed in NetBSD
# make with the following change:
#
# revision 1.281
# date: 2020-09-05 11:18:05 -0700;  author: rillig;  state: Exp;  lines: +60 -66;  commitid: 6IOaNyfoXI226TmC;
# make(1): fix .INCLUDEDFROMDIR/.INCLUDEDFROMFILE
#
# So it is possibly fixed in BMake 20200911, and certainly in 20200927.
#
# xxx so this hack to avoid the double inclusion only works with more recent
# versions of BMake.  Sadly that doesn't include Ubuntu's stable BMake-20200710
# package, including the GitHub Ubuntu runners.
#
# So since it's not really doing anything useful for current users it is left
# commented out.
#
#.if !defined(.INCLUDEDFROMFILE) || \
#	${.INCLUDEDFROMFILE} != "subdir.mk"
bmake_topdir := ${bmake_topdir}/..
#.info "appended '/..' to bmake_topdir!"
#.else
#. warning "NOT appending '/..' to bmake_topdir!"
#.endif

#.warning "bmake_topdir= ${bmake_topdir}: Included from ${.INCLUDEDFROMDIR}'/'${.INCLUDEDFROMFILE}"
