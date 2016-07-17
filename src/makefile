# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../Makefile.inc"
.include <bsd.own.mk>

LIB =	yajl

SHLIB_MAJOR =	2
SHLIB_MINOR =	1
SHLIB_TEENY =	0

#WARNS =	3

SRCS +=	yajl.c
SRCS +=	yajl_alloc.c
SRCS +=	yajl_buf.c
SRCS +=	yajl_encode.c
SRCS +=	yajl_gen.c
SRCS +=	yajl_lex.c
SRCS +=	yajl_parser.c
SRCS +=	yajl_tree.c
SRCS +=	yajl_version.c

# XXX This mess is because the library source stupidly requires the
# headers to be installed first, but that is not normal outside of a
# complete operating system's source tree.
#
# n.b.:  if you get a warning about an "Unknown modifier 'D'" then set
# $MAKEOBJDIRPREFIX in the environment!
#
.if !empty(MAKEOBJDIRPREFIX) && exists(${MAKEOBJDIRPREFIX})
. if defined(CPPFLAGS)
CPPFLAGS += -I${.OBJDIR}/api
. else
CFLAGS += -I${.OBJDIR}/api
. endif
.elif defined(MAKEOBJDIR)
. if defined(CPPFLAGS)
CPPFLAGS += -I.
. else
CFLAGS += -I.
. endif
.else
. if defined(CPPFLAGS)
CPPFLAGS += -I${.OBJDIR}/../api/obj${OBJMACHINE:D.${MACHINE}}
. else
CFLAGS += -I${.OBJDIR}/../api/obj${OBJMACHINE:D.${MACHINE}}
. endif
.endif

SUBDIR += api

.include <bsd.lib.mk>
# xxx hack for buggy old bsdmake on old OSX
.if !defined(DISTRIBUTION)
.include <bsd.subdir.mk>
.endif
