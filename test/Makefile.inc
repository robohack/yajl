# -*- makefile-bsdmake -*-

# N.B.:  ${.CURDIR} here is not needed -- it's already implied because of the
# include done by <bsd.prog.mk> in the subdir's Makefile
.include "../Makefile.inc"

bmake_topdir := ${bmake_topdir}/..
