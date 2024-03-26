# -*- makefile-bsdmake -*-

bmake_topdir =	..

.include <bsd.own.mk>

#
# Now some special hooks for building YAJL's API documentation.
#
# n.b.:  Use of ${MKDOC} must come after an include of <bsd.own.mk>
#
.if !defined(MKDOC) || empty(MKDOC:M[Nn][Oo])
#
# Note that here we're using MKDOC only to control HTML docs -- not to control
# the install of the basic README and COPYING files, etc.  This may not be
# standard use, but we're really only using it to avoid needing Cxref to build.
#
# Alternatively one can set CXREF=true on the command line or in the environ.
#
# N.B.:  Cxref requires two passes of each file, the first to build up the cross
#        referencing files and the second to use them.  Headers have to be done
#        first to avoid warnings about missing prototypes, and warnings should
#        be generated on the second pass.  A final index can be generated after
#        the first two passes over all the files.  Note we put the Cxref
#        database in the HTML output directory because there isn't any way to
#        tell cxref to read it from anywhere but where except where it also
#        writes it HTML output files in the second pass.  Sadly this means one
#        cannot easily share the results of the first pass with any additional
#        "2nd" passes to generate other forms of output (e.g. RTF or LaTeX).
#
# XXX because we process two example programs, each with definitions for main(),
# only the first found main() is included in the Appendix section.
#
# Note RoboDoc (textproc/robodoc, https://www.xs4all.nl/~rfsber/Robo/,
# https://github.com/gumpu/ROBODoc/) might be a viable alternative for cxref,
# and the most recent releases have the advantage of being able to produce troff
# output.  However it is even more ugly and much more difficult to use, and
# doesn't actually cross-reference C code so well.
#
# xxx this probably shouldn't be in here?
#
CXREF ?= cxref

# triggered by bmake-do-docs as a subdir target
docs: .PHONY html/yajl.apdx.html

# See below for additional, non-optional, rules for other docs
#
# XXX could this be done with <bsd.doc.mk>'s EXTRA ???
#
install-docs:: .PHONY html/yajl.apdx.html
	cd html && cp -fR ./ ${DESTDIR}${DOCDIR}/${PACKAGE}/html/
	rm -f ${DESTDIR}${DOCDIR}/${PACKAGE}/html/index.html
	ln -fs ./doc/yajl.cxref.html ${DESTDIR}${DOCDIR}/${PACKAGE}/html/index.html

# this is effectively an "obj" directory to contain just the HTML output files
#
#html:
#	mkdir -p ${.OBJDIR}/html

# XXX this really SHOULD also depend on all the $${files} found herein....
#
html/yajl.apdx.html: yajl.cxref # html
	mkdir -p html
	cd ${.CURDIR} && \
	files=$$(find ../src -depth -type d \( -name CVS -or -name .git -or -name .svn -or -name build \) -prune -or -type f \( -name '*.[ch]' -o -name '*.cxref' \) -print); \
	files="yajl.cxref $${files} ../reformatter/json_reformat.c ../example/parse_config.c"; \
	for file in $${files}; do \
		${CXREF} -R.. -xref-all -block-comments -O${.OBJDIR}/html -N${PACKAGE} -I${.CURDIR}/../src -I${GENHDIR} -CPP 'cc -std=c99 -E -U__BLOCKS__ -D__STRICT_ANSI__=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=1 -CC -x c' $${file}; \
	done;\
	for file in $${files}; do \
		${CXREF} -R.. -warn-all -xref-all -block-comments -O${.OBJDIR}/html -N${PACKAGE} -html -html-src -I${.CURDIR}/../src -I${GENHDIR} -CPP 'cc -E -std=c99 -U__BLOCKS__ -D__STRICT_ANSI__=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=1 -CC -x c' $${file}; \
	done; \
	${CXREF} -R.. -index-all -O${.OBJDIR}/html -N${PACKAGE} -html
	ln -fs doc/yajl.cxref.html ${.OBJDIR}/html/index.html

.endif	# ${MKDOC} != "no"

# n.b.:  we always install these documentation files -- they do not have to be
# built or transformed from their original source form
#
DOCFILES =		\
	README		\
	COPYING		\
	TODO

install-docs:: .PHONY beforeinstall docs .WAIT # maninstall
.for docfile in ${DOCFILES}
	cp ${.CURDIR:Q}/../${docfile} ${DESTDIR}${DOCDIR}/${PACKAGE}/
.endfor

# this is how we hook in the "docs" install...
#
afterinstall: .PHONY install-docs

# N.B.:  we don't use <bsd.doc.mk> -- it does nothing useful for us for now, and
# it causes headaches with Crufty's mk-files, and possibly FreeBSD too....
#
# xxx we really only want <bsd.files.mk>, but that can't be included directly on
# FreeBSD (and some don't include <bsd.obj.mk> either), and so the best common
# top-level mk-file is:
#
.include <bsd.prog.mk>

# work-arounds for Crufty mk-files
.if !target(.depend)
.depend:
	touch ${.TARGET}
.endif
.if !target(install)
install: .PHONY realinstall	# xxx yes this seems backwards, but it works!
.endif
