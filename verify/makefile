# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../Makefile.inc"

PROG = 	json_verify

# XXX This mess is because the library source stupidly requires the
# headers to be installed first, but that is not normal outside of a
# complete operating system's source tree.
#
.if !empty(MAKEOBJDIRPREFIX) && exists(${MAKEOBJDIRPREFIX})
. if defined(CPPFLAGS)
CPPFLAGS += -I${.OBJDIR}/../src/api
. else
CFLAGS += -I${.OBJDIR}/../src/api
. endif
LDADD += -L${.OBJDIR}/../src
.elif defined(MAKEOBJDIR)
. if defined(CPPFLAGS)
CPPFLAGS += -I.
. else
CFLAGS += -I.
. endif
LDADD += -L.
.else
. if defined(CPPFLAGS)
CPPFLAGS += -I${.OBJDIR}/../../src/api/obj${OBJMACHINE:D.${MACHINE}}
. else
CFLAGS += -I${.OBJDIR}/../../src/api/obj${OBJMACHINE:D.${MACHINE}}
. endif
LDADD += -L${.OBJDIR}/../../src/obj${OBJMACHINE:D.${MACHINE}}
.endif

LDADD+= -lyajl

MAN=	# empty

.include <bsd.prog.mk>

# xxx for old OSX bsdmake
LIBC=	# empty
