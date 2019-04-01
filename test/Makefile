# -*- makefile-bsdmake -*-

bmake_topdir = ..

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

SUBDIR =	api
SUBDIR +=	parsing

.include <bsd.subdir.mk>

# set compiler and linker flags, especially additional warnings
# (here mostly to get the "docs" target and for "regress")
#
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"
