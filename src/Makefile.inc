# -*- makefile-bsdmake -*-

# N.B.:  ${.CURDIR} here is not needed -- it's already implied because of the
# include done by <bsd.prog.mk> in the subdir's Makefile
.include "../Makefile.inc"

# XXX there's something odd about sjg's Mk-files, and older versions of his BMake.
#
# For some unknown reason <subdir.mk> does .-include <${.CURDIR}/Makefile.inc>.
# No other BSD-based Mk-files does this.
#
# Also until very recently (20240314 is fixed for sure) BMake didn't implement
# .INCLUDEDFROMFILE or .INCLUDEDFROMDIR correctly!
#
# xxx so this hack only works with very recent BMake.
#
.if !defined(.INCLUDEDFROMFILE) || \
	${.INCLUDEDFROMFILE} != "subdir.mk" && ${.INCLUDEDFROMFILE} != "bsd.subdir.mk"
bmake_topdir := ${bmake_topdir}/..
.endif

#.error "bmake_topdir= ${bmake_topdir}: Included from ${.INCLUDEDFROMDIR}'/'${.INCLUDEDFROMFILE}"
