# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../Makefile.inc"
.include <bsd.own.mk>

LIB =	yajl

SHLIB_MAJOR =   ${YAJL_MAJOR}
SHLIB_MINOR =   ${YAJL_MINOR}
SHLIB_TEENY =   ${YAJL_MICRO}

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

.if defined(CPPFLAGS)
CPPFLAGS += -I. -I${.CURDIR}
.else
CFLAGS += -I. -I${.CURDIR}
.endif

yajl/yajl_version.h:	${.CURDIR}/yajl/yajl_version.h.in
	sed -e 's/$${YAJL_MAJOR}/'${YAJL_MAJOR}'/' \
	    -e 's/$${YAJL_MINOR}/'${YAJL_MINOR}'/' \
	    -e 's/$${YAJL_MICRO}/'${YAJL_MICRO}'/' \
	< ${.ALLSRC} > ${.TARGET}

# no matter which if any are done first, build the header!
#
yajl_version.d: yajl/yajl_version.h
yajl_version.go: yajl/yajl_version.h
yajl_version.ln: yajl/yajl_version.h
yajl_version.o: yajl/yajl_version.h
yajl_version.pico: yajl/yajl_version.h
yajl_version.po: yajl/yajl_version.h
yajl_version.So: yajl/yajl_version.h

SUBDIR += yajl

.include <bsd.lib.mk>
.if !defined(DISTRIBUTION)	# avoid dup include on some systems
.include <bsd.subdir.mk>
.endif
