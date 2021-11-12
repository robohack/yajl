# -*- makefile-bsdmake -*-

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
#	export MAKEOBJDIRPREFIX=$(pwd -P)/build
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
#	bsdmake DESTDIR=$(pwd -P)/dist install
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
# version of Bmake that can be installed from Homebrew does mostly(*) work (and
# presumably a manual install of Simon's Bmake will also work).  Unfortunately
# the Bmake that comes with pkgsrc does not work properly on macOS.  Pkgsrc does
# not include Simon's MK files, but rather the bootstrap-mk-files package, which
# (as of 20180901) are not yet fully ported to OSX/Darwin (it is more or less
# just a copy of the non-portable NetBSD MK files).  If one can do without the
# shared library then one can use the pkgsrc bmake on macOS by passing
# "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" on the command line or in
# Makefile.inc.
#
# (*) The Homebrew bmake-20200902 version does not run regress rules, and also
# complains as follows:
#
# bmake: "/usr/local/Cellar/bmake/20200902/share/mk/bsd.subdir.mk" line 47: warning: Extra target ignored
# bmake: "/usr/local/Cellar/bmake/20200902/share/mk/bsd.subdir.mk" line 47: warning: Special and mundane targets don't mix. Mundane ones ignored
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
# $MAKEOBJDIRPREFIX set in the environment, and set to the same directory if you
# want to rebuild or continue a build.
#
# If you want to run make in just a sub-directory of the source tree AFTER
# you've done an initial build (or at least after you've done an initial run of
# "make obj") then you can do so provided you carefully set $MAKEOBJDIRPREFIX to
# the the pathname of the initial build directory you've initially created.
#
# Remember make tries to change to ${MAKEOBJDIRPREFIX}${.CURDIR} to run, so if
# you use a fully qualified pathname then you can use the exact same
# $MAKEOBJDIRPREFIX no matter where you are in the project hierarchy.  If the
# build directory is also a sub-directory of the project's source hierarchy then
# you can also use a relative path to it from within a sub-directory.
#
# If you mess things up and end up with generated files in your source directory
# then run "make cleandir" to start over.
#
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

bmake_topdir =	.

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
BUILDTARGETS +=	bmake-do-docs

# this ("all") must be the VERY first target
# (there shouldn't be any .includes above, including Makefile.inc!)
#
# (Remove the .WAIT if your build blows up.)
#
all: .PHONY .MAKE bmake-test-obj .WAIT ${BUILDTARGETS}

.ORDER: bmake-test-obj bmake-do-obj bmake-do-depend ${SUBDIR} bmake-do-docs

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/bmake-do-//}
.endfor

# XXX this is just a very crude check...  not as complete as the FreeBSD check
#
bmake-test-obj: .PHONY
	@if [ $$(pwd -P) = ${.CURDIR:Q} -a ! -z "${MAKEOBJDIRPREFIX:Q}" -a ! -d "${MAKEOBJDIRPREFIX:Q}" ]; then echo "You must create ${MAKEOBJDIRPREFIX}!"; false; fi

# n.b.:  Makefile.inc includes <bsd.own.mk>, which defines a default "all"
# target (amongst others), so it must come after all the above, but since it
# also defines additional values for variables used as .for lists it must come
# before <bsd.subdir.mk> and before anything else that uses values it sets in
# .for lists, e.g. the directories for bmake_install_dirs just below.
#
.include "${.CURDIR}/Makefile.inc"

bmake_install_dirs += ${BINDIR}
bmake_install_dirs += ${INCSDIR}
bmake_install_dirs += ${LIBDIR}
bmake_install_dirs += ${PKGCONFIGDIR}
# these DEBUGDIR ones could/should maybe depend on MKDEBUGLIB, but that's only
# defined after a .include <bsd.*.mk> below....
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/bin
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/lib
bmake_install_dirs += ${DOCDIR}/${PACKAGE}
#bmake_install_dirs += ${MANDIR} # xxx there are no manual pages, yet...
bmake_install_dirs += ${SHAREDIR}/doc/${PACKAGE}/html/

beforeinstall: _bmake_install_dirs

# many BSD system mk files will not make directories on demand
_bmake_install_dirs: .PHONY
.for instdir in ${bmake_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# XXX This, along with the dependency of install-docs on the HTML directory,
# seems necessary for Bmake (in pkgsrc and on Linux) to avoid parallel jobs
# during install from running ahead of the install directories being made....
#
${bmake_install_dirs:S|^|${DESTDIR}|}: _bmake_install_dirs

# See below for additional, optional, rules for HTML docs
#
install-docs:: .PHONY beforeinstall docs .WAIT # maninstall
	cp ${.CURDIR:Q}/README ${.CURDIR:Q}/COPYING ${.CURDIR:Q}/ChangeLog ${.CURDIR:Q}/TODO ${DESTDIR}${SHAREDIR}/doc/${PACKAGE}/

# this is how we hook in the "docs" install...
#
afterinstall: .PHONY install-docs

.include <bsd.subdir.mk>
.include <bsd.obj.mk>	# n.b. may be needed for making docs
.include "${.CURDIR}/Makefile.compiler"

# This block must come after some <bsd.*.mk> in order to use MKDOC
#
.if ${MKDOC:Uno} != "no"
#
# Note that here we're using MKDOC only to control HTML docs -- not to control
# the install of the basic README and COPYING files, etc.  This may not be
# standard use, but we're really only using it to avoid needing Cxref to build.
#

# XXX Some of the futzing with .OBJDIR below would probably be avoided if we
# didn't do it from here in the top-level directory -- I.e. consider moving the
# main header file, i.e. yajl.cxref, to a "docs" subdir and do it all there.

# Make sure make changes to the .OBJDIR properly
#
# This seems to be needed IFF ${.OBJDIR} didn't exist on startup, and because
# this is the top-level Makefile, make won't have been able to chdir there yet.
#
# XXX this (obviously) makes print-objdir work, but it doesn't go there,
# i.e. without the cd in doc/html???

# (note that IFF .OBJDIR doesn't exist then this also leaves .OBJDIR as a
# relative directory, not an absolute path)
#
# XXX this probably won't work for FreeBSD, but then again with WITH_AUTO_OBJ it
# may not be necessary.
#
. if defined(__objdir)
# reset .OBJDIR so it expands correctly herein on first go when it doesn't exist
.OBJDIR = ${__objdir}
# If .OBJDIR does exist then (re)canonicalize .OBJDIR, and reset internal stuff
# (chdir(), set $PWD, etc.)
# (this isn't actually necessary, but it improves the possibility that use of
# MAKEOBJDIR might work, especially on a second invocation)
.OBJDIR: ${.OBJDIR}
. endif

# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
. if defined(MAKEOBJDIRPREFIX)
# .OBJDIR is from MAKEOBJDIRPREFIX
GENHDIR = ${.OBJDIR}/${bmake_topdir}/src
. else
# assume .OBJDIR is a local directory, so look for bmake_topdir from the parent
#
# XXX this probably breaks for ${MAKEOBJDIR}
#
# N.B.: note the inclusion of ${.OBJDIR} -- this is because the rule does a "cd"
# Note also here in the top-level we could avoid the dance, but this form is
# reusable in a subdir.
GENHDIR += ${.OBJDIR}/../${bmake_topdir}/src/${.OBJDIR:T}
. endif

docs: doc/html/yajl.apdx.html

install-docs:: ${DESTDIR}${SHAREDIR}/doc/${PACKAGE}/html/
	cd doc/html && cp -R ./ ${DESTDIR}${SHAREDIR}/doc/${PACKAGE}/html/

# XXX MAGIC!  N.B.:  The "cd ${.OBJDIR}" makes make believe ${.OBJDIR} exists!!!
doc/html:
	cd ${.OBJDIR} && mkdir -p doc/html

# XXX this should also depend on all the $${files} found herein....
#
doc/html/yajl.apdx.html: doc/html yajl.cxref
	cd ${.CURDIR} && \
	files=$$(find ./src -depth -type d \( -name CVS -or -name .git -or -name .svn -or -name build \) -prune -or -type f \( -name '*.[ch]' -o -name '*.cxref' \) -print); \
	files="yajl.cxref $${files} reformatter/json_reformat.c example/parse_config.c"; \
	for file in $${files}; do \
		${CXREF} -xref-all -block-comments -O${.OBJDIR}/doc/html -N${PACKAGE} -I${.CURDIR}/src -I${GENHDIR} -CPP 'cc -E -CC -x c' $${file}; \
	done;\
	for file in $${files}; do \
		${CXREF} -warn-all -xref-all -block-comments -O${.OBJDIR}/doc/html -N${PACKAGE} -html -html-src -I${.CURDIR}/src -I${GENHDIR} -CPP 'cc -E -CC -x c' $${file}; \
	done; \
	${CXREF} -index-all -O${.OBJDIR}/doc/html -N${PACKAGE} -html
	ln -fs ${.OBJDIR}/doc/html/yajl.cxref.html ${.OBJDIR}/doc/html/index.html

.endif

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
#
