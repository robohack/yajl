# -*- makefile-bsdmake -*-

.include "${.CURDIR}/Makefile.inc"

# This Makefile (and its associated include files) works with NetBSD Make and
# Simon Gerraty's Bmake & Mk-files from http://www.crufty.net/FreeWare/, and
# with FreeBSD make with caveats.  For many systems the bmake included in pkgsrc
# will also work (see https://pkgsrc.org/).
#
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Simon's Bmake works on many Unix-like systems.  Note warnings about
# FreeBSD make's stupidities below.
#
# You should use $MAKEOBJDIRPREFIX so as to build everything elsewhere outside
# of, or within a single sub-director, of the source tree; (i.e. instead of
# polluting the rest of the source tree with "obj" sub-directories).
#
#	mkdir build
#	export MAKEOBJDIRPREFIX=$(pwd)/build
#	export WITH_AUTO_OBJ=yes		# just for FreeBSD, sigh.
#	bsdmake					# or just "make" where possible!
#
# (I.e. use "make" on non-GNU systems where it is Bmake; or use "bmake" or
# "bsdmake" as needed on other systems.)
#
# Then if the build succeeds (and assuming you're not cross-compiling) you can
# run the regression tests to see if the results are correct.
#
#	bsdmake regress
#
# Finally to install the results into a "dist" subtree (which you can then
# distribute as a binary distribution that can be un-packed wherever desired)
# you can do:
#
#	bsdmake DESTDIR=$(pwd)/dist install
#
# DESTDIR can of course be any directory, e.g. /usr/local.
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it does not get in the way of pkgsrc either.)
#
# WARNING:  Do not specify DESTDIR for the main build nor the regress target!
#
#####################
#
# Building Documentation:
#
# The documentation is all currently within comments in the source code and we
# use Cxref to extract it and turn it into something more useful and coherent,
# which by default is a set of HTML pages.  This has the added advantage of
# providing a comprehensive hyperlinked cross-reference of all the types and
# functions in all of the source files.
#
# If your Bmake system defined MKDOC, but you do not have Cxref, you can disable
# the building and installation of the HTML documentation by setting "MKDOC=no"
# on the Bmake command line.  UTSL!
#
# Cxref can be found at:  https://www.gedanken.org.uk/software/cxref/
#
#####################
#
# Special Notes for Special Systems:
#
# OSX, aka macOS, since use of Xcode 10(?) doesn't have a working bsdmake in the
# base system, nor does the one installable from Homebrew work.  However the
# version of Bmake that can be installed from Homebrew does work (and presumably
# a manual install of Simon's Bmake will also work).  Unfortunately the Bmake
# that comes with pkgsrc does not work properly on macOS.  Pkgsrc does not
# include Simon's MK files, but rather the bootstrap-mk-files package, which (as
# of 20180901) is not yet fully ported to OSX/Darwin (it is more or less just a
# copy of the non-portable NetBSD MK files).  If one can do without the shared
# library then one can use the pkgsrc bmake on macOS by passing "SHLIB_MAJOR=
# SHLIB_MINOR= SHLIB_TEENY=" on the command line or in Makefile.inc.
#
# FreeBSD's make (up to and including 12.0) is extremely beligerent about having
# $MAKEOBJDIRPREFIX set in the environment and only in the environment -- it
# refuses to even peek at it if it has only been set on the command line (and
# their manual page lies in its second mention of this, claiming it can be set
# on the command line).  Grrrr...  (Older versions of NetBSD make (and thus
# Bmake) had this problem too -- but they fixed it.)
#
# FreeBSD's make is also too broken to do the right thing with "obj" in the
# target list for "all".  Their saving grace (as of at least 12.0) is they've
# implemented WITH_AUTO_OBJ, and it works, BUT ONLY IF YOU PUT IT ON THE COMMAND
# LINE OR IN THE ENVIRONMENT!
#
#####################
#
# More about $MAKEOBJDIRPREFIX:
#
# Using $MAKEOBJDIRPREFIX requires always invoking the build again with
# $MAKEOBJDIRPREFIX set in the environment, and set to the same value if you
# want to rebuild or continue a build.
#
# If you want to run make in just a sub-directory of the source tree AFTER
# you've done an initial build (or at least after you've done an initial run of
# "make obj") then you can do so provided you always carefully set
# $MAKEOBJDIRPREFIX to the the fully qualified pathname of the initial build
# directory you've initially created.  Remember make tried to change to
# ${MAKEOBJDIRPREFIX}${.CURDIR} to run, so you always use the exact same
# $MAKEOBJDIRPREFIX no matter where you are in the hierarchy.  (Yes, this is
# unfortunately painful to do from within emacs as you cannot use a relative
# path back to the top to specify $MAKEOBJDIRPREFIX, so you can't set it easily
# from local variables -- and if you set it with `setenv' then you will risk
# having the wrong value for different projects.)
#
# If you mess things up and end up with generated files in your source directory
# then run "make cleandir" to start over.
#
# How to do without $MAKEOBJDIRPREFIX:
#
# If you don't use $MAKEOBJDIRPREFIX then "obj.${MACHINE}" sub-directories will
# be created for each directory with products.  EXCEPT ON FreeBSD!!!  (where the
# default is always just "obj", BUT IT IS BROKEN! (as of 12.0)).  Just use
# $MAKEOBJDIRPREFIX!!!
#
# If you end up with "obj.*" sub-directories and you want to go back to using a
# 'build' directory (as would be sane to do) then you can remove all the obj.*
# detritus with (the trailing, quoted, semicolon is important!):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj.*' -exec rm -rf {}/obj.$(uname -m) \;
#
# N.B.:  Some variants of BSD Make treat ${MAKEOBJDIR} as a sub-directory under
# /usr/obj, and others treat it as a sub-directory under ${.CURDIR}.  You have
# been warned.  Some only allow it to be set in the environment.  You should
# just use $MAKEOBJDIRPREFIX.

# Now, on with the show....

SUBDIR =	src

# Not all older mk-files (e.g. Apple's for bsdmake) support having a .WAIT in a
# SUBDIR list, but it is vital and necessary for parallel builds (i.e. use of
# 'make -j') (.ORDER doesn't quite make up for it because of the fact these
# directories always exist prior to starting make).
#
# Comment out the .WAIT settings here and below out if your build blows up with
# something resembling "warning: Extra target ignored" and/or "warning: Special
# and mundane targets don't mix. Mundane ones ignored" and/or "cd:  .../.WAIT:
# No such file or directory"
#
# WARNING!!!  But if you eliminate .WAIT then DO NOT invoke parallel builds!
#
SUBDIR +=	.WAIT

SUBDIR +=	reformatter
SUBDIR +=	verify
SUBDIR +=	example
SUBDIR +=	perf
SUBDIR +=	test

#
# The rest is mostly just default boilerplate for stand-alone builds....
#
# Yes, "make obj" is forced -- it is stupid to build in the source directory)
#
# This does mean nothing can be made in the top directory though.
#
# Note with "bmake" this will cause obj* directories to be created in the
# existing obj* directories the second time around...
#

MKOBJ = yes
# XXX "auto" is actually not yet very widely supported and may not work.
MKOBJDIRS = auto

# Comment the .WAIT's out (and avoid -j) if your build blows up
#
BUILDTARGETS =	bmake-do-obj
BUILDTARGETS +=	.WAIT
# (forcing "make depend" is also both good, and necessary (see the beforedepend
# target in src/Makefile, though otherwise it is a bit of a waste for pkgsrc).
BUILDTARGETS +=	bmake-do-depend
BUILDTARGETS +=	.WAIT

# this ("all") must be the VERY first target
# (there shouldn't be any .includes above!)
#
# (Remove the .WAIT if your build blows up.)
#
all: .PHONY .MAKE bmake-test-obj .WAIT ${BUILDTARGETS}

.ORDER: bmake-test-obj bmake-do-obj bmake-do-depend ${SUBDIR}

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/bmake-do-//}
.endfor

# XXX this is just a very crude check...
#
bmake-test-obj: .PHONY
	@if [ $$(pwd) = ${.CURDIR:Q} -a ! -z ${MAKEOBJDIRPREFIX:Q} -a ! -d ${MAKEOBJDIRPREFIX:Q} ]; then echo "You must create ${MAKEOBJDIRPREFIX}!"; false; fi

# most implementations do not make 'regress' depend on first building everything
# but we need to build everything before we can do any testing
#
regress: all
.PHONY: regress

bmake_install_dirs += ${BINDIR}
bmake_install_dirs += ${INCSDIR}
bmake_install_dirs += ${LIBDIR}
bmake_install_dirs += ${PKGCONFIGDIR}
# these DEBUGDIR ones could/should maybe depend on MKDEBUGLIB, but that's only
# defined after a .include <bsd.*.mk> below....
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/bin
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/lib
bmake_install_dirs += ${DOCDIR}/${PACKAGE}
#bmake_install_dirs += ${MANDIR} # xxx there are no manual pages yet...
# (in general though it is safest to always make them all)

beforeinstall: _bmake_install_dirs

# many BSD system mk files will not make directories on demand
_bmake_install_dirs: .PHONY
.for instdir in ${bmake_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

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
CXREF ?= cxref
docs: .PHONY ${MAKEOBJDIRPREFIX:Q}/doc/html/yajl.apdx.html

${MAKEOBJDIRPREFIX:Q}/doc/html:
	-mkdir -p ${MAKEOBJDIRPREFIX:Q}/doc/html

# XXX this should also depend on all the $${files} found herein....
#
${MAKEOBJDIRPREFIX:Q}/doc/html/yajl.apdx.html: ${MAKEOBJDIRPREFIX:Q}/doc/html yajl.cxref
	cd ${.CURDIR} && \
	files=$$(find ./src -depth -type d \( -name CVS -or -name .git -or -name .svn -or -name build \) -prune -or -type f \( -name '*.[ch]' -o -name '*.cxref' \) -print); \
	files="yajl.cxref $${files} reformatter/json_reformat.c example/parse_config.c"; \
	for file in $${files}; do \
		${CXREF} -xref-all -block-comments -O${MAKEOBJDIRPREFIX:Q}/doc/html -N${PACKAGE} -I${.CURDIR}/src -I${MAKEOBJDIRPREFIX:Q}${.CURDIR}/src -CPP 'cc -E -CC -x c' $${file}; \
	done;\
	for file in $${files}; do \
		${CXREF} -warn-all -xref-all -block-comments -O${MAKEOBJDIRPREFIX:Q}/doc/html -N${PACKAGE} -html -html-src -I${.CURDIR}/src -I${MAKEOBJDIRPREFIX:Q}${.CURDIR}/src -CPP 'cc -E -CC -x c' $${file}; \
	done; \
	${CXREF} -index-all -O${MAKEOBJDIRPREFIX:Q}/doc/html -N${PACKAGE} -html
	ln -fs ${MAKEOBJDIRPREFIX:Q}/doc/html/yajl.cxref.html ${MAKEOBJDIRPREFIX:Q}/doc/html/index.html

install-docs:: .PHONY beforeinstall .WAIT docs # .WAIT maninstall
	cp ${.CURDIR:Q}/README ${.CURDIR:Q}/COPYING ${.CURDIR:Q}/ChangeLog ${.CURDIR:Q}/TODO ${DESTDIR}${SHAREDIR}/doc/${PACKAGE}/

# this is how we hook in the "docs" install...
#
afterinstall: .PHONY install-docs

.include <bsd.subdir.mk>

# This block must come after some <bsd.*.mk> in order to (maybe) set MKDOC
#
.if ${MKDOC:Uno} != "no"
#
# add docs to the prerequisites for "all"
#
BUILDTARGETS +=	bmake-do-docs
bmake-do-docs: .PHONY docs
all: bmake-do-docs
install-docs::
	cd ${MAKEOBJDIRPREFIX:Q}/doc/html && cp -R ./ ${DESTDIR}${SHAREDIR}/doc/${PACKAGE}/html/
.endif

# set compiler and linker flags, especially additional warnings
# (here for supporting "regress")
#
.include "${.CURDIR}/Makefile.compiler"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd)/build " (default-value 'compile-command))
# End:
#
