# -*-makefile-bsdmake-*-

.include "${.CURDIR}/../../Makefile.inc"
.include <bsd.own.mk>

INCS =	yajl_version.h

yajl_apifiles +=	yajl_parse.h
yajl_apifiles +=	yajl_gen.h
yajl_apifiles +=	yajl_common.h
yajl_apifiles +=	yajl_tree.h

INCS += ${yajl_apifiles}

# XXX This mess is because the library source stupidly requires the
# headers to be installed first, but that is not normal outside of a
# complete operating system's source tree.
#
# So, we create the sub-directory in ${.OBJDIR} and install there.
# (see the trickery in ../../Makefile.inc)

includes: yajl

.for incfile in ${INCS}
CLEANFILES += yajl/${incfile}
.endfor

CLEANFILES += yajl_version.h
yajl_version.h:	${.CURDIR}/yajl_version.h.cmake
	sed -e 's/$${YAJL_MAJOR}/'${YAJL_MAJOR}'/' \
	    -e 's/$${YAJL_MINOR}/'${YAJL_MINOR}'/' \
	    -e 's/$${YAJL_MICRO}/'${YAJL_MICRO}'/' \
	< ${.ALLSRC} > ${.TARGET}

yajl:
	mkdir yajl

.if exists(yajl) && make(cleandir)
cleandir: rmyajl

rmyajl: .PHONY clean
	rmdir yajl
.endif

.include <bsd.prog.mk>
.sinclude <inc.mk>		# crufty.net mk-files

# xxx for old OSX bsdmake
buildincludes: yajl ${INCS}

# for crufty.net mk-files:
.if target(incinstall) && !target(__incinstall)
INCS = yajl_version.h ${yajl_apifiles:S|^|${.CURDIR}/|}
includes: .PHONY incinstall
.endif

.if !target(installincludes)
# more trickery to do the includes again at install time for older OSX
install: includes
.endif
