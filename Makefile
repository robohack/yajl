# -*- makefile-bsdmake -*-

# This Makefile (and its associated include files) works with NetBSD Make and
# Simon Gerraty's Bmake & Mk-files from http://www.crufty.net/FreeWare/, and
# with FreeBSD make with caveats.  For many systems the bmake included in pkgsrc
# will also work (see https://pkgsrc.org/).
#
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Similarly Simon's Bmake works on most any Unix or Unix-like system.
#
# You should use $MAKEOBJDIRPREFIX so as to build everything elsewhere outside
# of, or within a single sub-directory, of the source tree (i.e. instead of
# polluting the source itself tree with "obj" sub-directories everywhere).
#
#	mkdir build
#	export MAKEOBJDIRPREFIX=$(pwd -P)/build
#	export WITH_AUTO_OBJ=yes		# just for FreeBSD, sigh.
#	b(sd)make
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a sub-directory under
# /usr/obj, and others treat it as a sub-directory under ${.CURDIR}, even if it
# starts with a '/'!  You have been warned.  As with $MAKEOBJDIRPREFIX some
# older versions also only allow it to be set in the environment.  You should
# just use $MAKEOBJDIRPREFIX, set in the environment (except on OpenBSD since
# 5.5, where $MAKEOBJDIR is necessary).
#
# Optionally you may change the final installation heriarchy from the default of
# "/usr" to any path prefix of your choice by setting PREFIX on the make command
# lines, like this (or optionally in the environment):
#
#	b(sd)make PREFIX=/opt/pkg
#
# Then if the build succeeds (and assuming you're not cross-compiling) you can
# run the regression tests to see if the results are correct.
#
#	b(sd)make regress
#
# Finally to install the results into a "dist" subtree (which you can then
# distribute as a binary distribution that can be un-packed in the root of a
# target system) you can do:
#
#	b(sd)make DESTDIR=$(pwd -P)/dist install
#	cd dist && tar -cf ../dist.tar .
#
# DESTDIR can of course be any directory, e.g. /usr/local, especially if PREFIX
# is set to an empty string (PREFIX=""), but note the design is such that the
# package can be installed at build time into a private DESTDIR (as above), then
# archived and the resulting archive can be extracted at the root of the target
# system's filesystem.  The default with PREFIX="/usr" will install into the
# base of a typical unix filesystem, while something like PREFIX="/usr/pkg" will
# install into a typical package installation direcory, and PREFIX="/usr/local"
# (with DESTDIR=$(pwd -P)/dist) will be the equivalent of PREFIX="" and
# DESTDIR="/usr/local", the difference being the latter does an immediate
# install on the build system, while the former allows the DESTDIR to be
# archived and then extracted on any suitable target system.
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it matches the way pkgsrc now works internally.)
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
# on the Bmake command line or in the environment, or by uncommenting the
# following line:
#
#MKDOC = no
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
# just an out-of-date copy of the non-portable NetBSD MK files).  So, if one can
# do without the shared library then one can use the pkgsrc bmake on macOS, and
# depending on the vintage of one's pkgsrc, it may also be necessary to pass
# "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" on the command line or in
# Makefile.inc.  In current pkgsrc not doing so will install the shared library
# with the wrong name (i.e. using the ".so" naming convention instead of the
# Mach-O ".dylib" convention, and so the linker will never find and use it.
#
# The really old Apple bsdmake and mk-files do generate the correct shared
# library names, but they don't support .WAIT in the ${SUBDIR} list.
#
# So note OpenBSD's make since 5.5 (and before 2.1) does NOT support
# $MAKEOBJDIRPREFIX at all.  For recent OpenBSD, just use ${MAKEOBJDIR} instead.
#
# FreeBSD's make (or rather their mk-files) is too broken to do the right thing
# with "obj" in the target list for "all".  Their saving grace (as of at least
# 12.0) is they've implemented WITH_AUTO_OBJ, and it works, BUT ONLY IF YOU PUT
# "WITH_AUTO_OBJ=yes" ON THE COMMAND LINE OR IN THE ENVIRONMENT!
#
# See the first use of .WAIT below for comments about really old BMakes and
# mk-files that don't deal with it properly.
#
#####################
#
# More about using $MAKEOBJDIRPREFIX:
#
# Using $MAKEOBJDIRPREFIX requires always invoking the build again with
# $MAKEOBJDIRPREFIX set in the environment, and set to the same directory if you
# want to rebuild or continue a build.
#
# If you want to run make in just a sub-directory of the source tree AFTER
# you've done an initial build (or at least after you've done an initial run of
# "make obj") then you can do so provided you carefully set $MAKEOBJDIRPREFIX to
# the the pathname of the initial build directory you created.
#
# Remember BSD Make tries to change to ${MAKEOBJDIRPREFIX}${.CURDIR} to any
# rules, so if you use a fully qualified pathname then you can use the exact
# same $MAKEOBJDIRPREFIX no matter where you are in the project hierarchy.  If
# the build directory is also a sub-directory of the project's source hierarchy
# then you can also use a relative path to it from within a sub-directory.
#
# Note that setting $MAKEOBJDIRPREFIX in your shell's environment may risk
# mixing things up for different projects, though if your BSD Make does
# correctly set ${.CURDIR} to the canonical fully qualified current working
# directory where it was started from, and if you have set $MAKEOBJDIRPREFIX to
# a fully qualified pathname, then this could be a good way to share use of a
# fast scratch filesystem for builds of many different projects using BSD
# Makefiles.
#
# If you mess things up and end up with generated files in your source directory
# then run "make cleandir" to start over.
#
#####################
#
# How to do without $MAKEOBJDIRPREFIX:
#
# If you don't use $MAKEOBJDIRPREFIX then "obj.${MACHINE}" sub-directories will
# be created for each directory with products.  EXCEPT ON FreeBSD!!!  (where the
# default is always just "obj", BUT IT IS BROKEN! (as of 12.0)).
#
# If you end up with "obj.*" sub-directories and you want to go back to using a
# 'build' directory (as would be sane to do) then you can remove all the obj.*
# detritus with this command (the trailing, escaped, semicolon is important!):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj*' -exec rm -rf {}obj {}/obj.$(uname -m) \;
#
#####################
#
# The history of MAKEOBJDIRPREFIX is a bit convoluted.
#
# The original BSD's "PMake" from CSRG (right up to 4.4BSD-Lite2) did not
# support MAKEOBJDIRPREFIX at all.
#
# The first support appears to come in FreeBSD with r18339 (of make/main.c) by
# Steven Wallace on 1996-09-17.  This corresponds to about FreeBSD-2.2.1, I
# think.
#
# However the initial implementation in FreeBSD's original make since 2.2.1 when
# they first gain support, and up to about May 2014, or in 9.0; and as merged in
# NetBSD's make since (effectively) 1.5 (literally since 1.3) and prior to 7.0
# (and so in Simon's Bmake since its inception, and up to bmake-20140214); and
# in OpenBSD's make since 2.1 when they first gained support up until 5.5 (after
# which they removed all support for MAKEOBJDIRPREFIX, a change not documented
# until 6.7!!!) were all extremely beligerent about having $MAKEOBJDIRPREFIX set
# in the environment and only in the environment -- they refused to even peek at
# it if it is only set on the command line, using only getenv(3) to access it.
#
# However in all but OpenBSD this has been fixed so that MAKEOBJDIRPREFIX can
# also be set on the command line.  I think.  I have had bad experiences with
# some versions (e.g. claiming to support setting it on the command line, but in fact
# not supporting that at all.

#####################
#
# Now, on with the show....
#

bmake_topdir =	.

SUBDIR =	src

# If you're staring at the line below becaouse your build blew up with something
# resembling "warning: Extra target ignored" and/or "warning: Special and
# mundane targets don't mix. Mundane ones ignored" and/or "cd:  .../.WAIT:  No
# such file or directory", then please read the following:
#
# Many older mk-files support having a .WAIT in a SUBDIR list, but it is vital
# and necessary for parallel builds (i.e. use of 'make -j') (.ORDER doesn't
# quite make up for it because of the fact these directories always exist prior
# to starting make).
#
# The best fix by far is to upgrade to a current version of Bmake.
#
# If you can't easily do that, or you're just in a hurry to build yajl this one
# time, you can comment out the .WAIT settings here and, if necessary, below.
#
# WARNING!!!  But if you eliminate .WAIT then DO NOT invoke parallel builds!
#
SUBDIR +=	.WAIT

SUBDIR +=	reformatter
SUBDIR +=	verify
SUBDIR +=	example
SUBDIR +=	perf
SUBDIR +=	test

#####################
#
# The next section is mostly just default boilerplate for stand-alone project
# builds.  It could/should be in a separate included file.  (Except for some of
# the ${bmake_install_dirs}.)
#
# Yes, "make obj" is forced -- it is stupid to build in the source directory)
#
# This does mean nothing else can be made in the top directory though.
#
# Note with some versions of Simon's "mk-files" this will cause obj* directories
# to be created in the existing obj* directories the second time around...
#

MKOBJ = yes
# XXX "auto" is actually not yet very widely supported and may not work.
MKOBJDIRS = auto

# Comment the .WAIT's out (and avoid -j) if your build blows up
#
BUILDTARGETS +=	bmake-do-obj
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

# just in case old habits prevail -- this should generally work to do everything
# in the right order with the right .WAITs for parallel builds, assuming .WAIT
# support does work in ${SUBDIR} at all.
#
dependall: .PHONY .MAKE bmake-test-obj .WAIT ${BUILDTARGETS}

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
bmake_install_dirs += ${DOCDIR}/${PACKAGE}/html/
#bmake_install_dirs += ${MANDIR} # xxx there are no manual pages, yet...

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

#####################
#
# Now some special hooks for building YAJL's API documentation.
#
# n.b.:  Use of ${MKDOC} must come after an include of <bsd.own.mk>, which was
# done via the include of "Makefile.inc" just above.
#
.if !defined(MKDOC) || empty(MKDOC:M[Nn][Oo])
#
# Note that here we're using MKDOC only to control HTML docs -- not to control
# the install of the basic README and COPYING files, etc.  This may not be
# standard use, but we're really only using it to avoid needing Cxref to build.
#
# Alternatively one can set CXREF=true on the command line or in the environ.
#

docs: doc/html/yajl.apdx.html

# See below for additional, non-optional, rules for other docs
#
install-docs:: ${DESTDIR}${DOCDIR}/${PACKAGE}/html/
	cd doc/html && cp -R ./ ${DESTDIR}${DOCDIR}/${PACKAGE}/html/

# See the helper settings below the include of <bsd.obj.mk> for how ${.OBJDIR}
# is properly reset even before it has been made.
#
# Note on first run before ${.OBJDIR} exists, make obviously has not yet been
# able to chdir there, so we have to do so explicitly in this rule after
# depending on bmake-do-obj to do the making of the directory.
#
doc/html: bmake-do-obj
	cd ${.OBJDIR} && mkdir -p doc/html

# XXX this really SHOULD also depend on all the $${files} found herein....
#
doc/html/yajl.apdx.html: doc/html yajl.cxref
	cd ${.CURDIR} && \
	files=$$(find ./src -depth -type d \( -name CVS -or -name .git -or -name .svn -or -name build \) -prune -or -type f \( -name '*.[ch]' -o -name '*.cxref' \) -print); \
	files="yajl.cxref $${files} reformatter/json_reformat.c example/parse_config.c"; \
	for file in $${files}; do \
		${CXREF} -xref-all -block-comments -O${.OBJDIR}/doc/html -N${PACKAGE} -I${.CURDIR}/src -I${GENHDIR} -CPP 'cc -std=c99 -E -U__BLOCKS__ -D__STRICT_ANSI__=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=1 -CC -x c' $${file}; \
	done;\
	for file in $${files}; do \
		${CXREF} -warn-all -xref-all -block-comments -O${.OBJDIR}/doc/html -N${PACKAGE} -html -html-src -I${.CURDIR}/src -I${GENHDIR} -CPP 'cc -E -std=c99 -U__BLOCKS__ -D__STRICT_ANSI__=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=1 -CC -x c' $${file}; \
	done; \
	${CXREF} -index-all -O${.OBJDIR}/doc/html -N${PACKAGE} -html
	ln -fs yajl.cxref.html ${.OBJDIR}/doc/html/index.html

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
	cp ${.CURDIR:Q}/${docfile} ${DESTDIR}${DOCDIR}/${PACKAGE}/
.endfor

# this is how we hook in the "docs" install...
#
afterinstall: .PHONY install-docs

# n.b. this may be needed for making docs/html from this top-level directory
#
# XXX We include it first because it might include <bsd.subdir.mk>!
#
.include <bsd.obj.mk>

# include the "standard" mk-file for building in sub-directories
#
# XXX unfortunately BMake's (and older FreeBSD's) mk-files will again include
# <bsd.subdir.mk> from within <bsd.obj.mk>.  Worse there's no common macro
# defined in all common versions of <bsd.subdir.mk>.
#
# XXX However it currently seems as if every <bsd.obj.mk> that includes
# <bsd.subdir.mk> on its own also defines ${CANONICALOBJDIR}, so we protect this
# include with that knowledge:
#
.if !defined(CANONICALOBJDIR)
.include <bsd.subdir.mk>
.endif

# This block must come after <bsd.obj.mk> in order to use ${__objdir} or
# ${CANONICALOBJDIR}:
#
# XXX This futzing with .OBJDIR could be avoided if we didn't do it from here
# (i.e. in the top-level directory) -- I.e.:  Consider moving the main header
# file, i.e. yajl.cxref, to a "docs" subdir and do it all there.
#
# This reset is needed IFF ${.OBJDIR} didn't exist on startup and, because this
# is the top-level Makefile, make won't have been able to chdir there yet on
# first invocation (i.e. before it does the bmake-do-obj target).  Perhaps all
# <bsd.obj.mk> files should have been/be resetting ${.OBJDIR} internally!
#
# XXX this (obviously) makes "make -V .OBJDIR" show the expected value, but it
# doesn't immediately go there, i.e. without the cd in the doc/html rule above.
#
# (Note that IFF ${.OBJDIR} doesn't exist then this also leaves ${.OBJDIR} as a
# relative directory, not an absolute path, but that's ok as doc/html depends on
# bmake-do-obj and so will create the missing ${.OBJDIR} before using it.)
#
.if !defined(MKDOC) || empty(MKDOC:M[Nn][Oo])
#
# first for NetBSD's native mk-files:
#
. if defined(__objdir)
#
# reset .OBJDIR so it expands correctly herein on first go when it doesn't exist
#
.OBJDIR = ${__objdir}
#
# If ${.OBJDIR} does now exist then (re)canonicalize .OBJDIR, and reset internal
# stuff (chdir(), set $PWD, etc.)
#
.OBJDIR: ${.OBJDIR}
#
# now do the same for BMake's (including FreeBSD') (and Apple's old bsdmake)
# mk-files...
#
# XXX this should also work for modern bmake-using FreeBSD and for Simon's
# mk-files, but then again with WITH_AUTO_OBJ it may not be necessary.
#
. elif defined(CANONICALOBJDIR)
.OBJDIR = ${CANONICALOBJDIR}
.OBJDIR: ${.OBJDIR}
. endif
#
.endif	# ${MKDOC} != "no"

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
#
