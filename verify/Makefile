# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../Makefile.inc"

PROG = 	json_verify

yajl_srcdir =	${.CURDIR}/../src

.if defined(CPPFLAGS)
CPPFLAGS += -I${yajl_srcdir}
.else
CFLAGS += -I${yajl_srcdir}
.endif

LDADD += -L${.OBJDIR}/../src
LDADD+= -lyajl

MAN=	# empty

.include <bsd.prog.mk>

# xxx for old OSX bsdmake
LIBC=	# empty
