# -*- makefile-bsdmake -*-

# This Makefile (and its associated include files) works with NetBSD Make, and
# Simon Gerraty's (sjg's) latest BMake from http://www.crufty.net/FreeWare/
# (with some caveats), and with FreeBSD make.  For many systems the BMake
# included in pkgsrc will also work (see https://pkgsrc.org/).  It is as yet
# untested on OpenBSD.  BMake is not supported on FreeBSD.
#
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Similarly Simon's BMake works on most any Unix or Unix-like system.
#
# You should use $MAKEOBJDIRPREFIX, set in the environment, so as to build
# everything elsewhere outside of, or within a single sub-directory, of the
# source tree (i.e. instead of polluting the source itself tree with "obj"
# sub-directories everywhere).
#
#	mkdir build
#	export MAKEOBJDIRPREFIX=$(pwd -P)/build
#	b(sd)make
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a sub-directory under
# /usr/obj, and others treat it as a sub-directory under ${.CURDIR}, even if it
# starts with a '/'!  You have been warned.  As with $MAKEOBJDIRPREFIX some
# older versions also only allow it to be set in the environment.  You should
# just use $MAKEOBJDIRPREFIX, set in the environment (except on OpenBSD since
# 5.5, where $MAKEOBJDIR is necessary, see below).
#
# You may change the final installation heriarchy from the default of "/usr" to
# any path prefix of your choice by setting PREFIX on the make command lines,
# like this (or optionally on the command line, but remember it for the install
# step too!):
#
#	export PREFIX=/opt/pkg
#	b(sd)make
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
# system's filesystem.  The default with PREFIX="/usr" (without DESTDIR) will
# install into the base of a typical unix filesystem, while something like
# PREFIX="/usr/pkg" will install into a typical package installation directory.
# Setting PREFIX="/usr/local" along with DESTDIR=$(pwd -P)/dist will install
# into the named DESTDIR and allow those files to be archived on the build
# system and then extracted (at /) on any suitable target system where they will
# end up in /usr/local.
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it matches the way pkgsrc now works internally.)
#
# WARNING:  Do not specify DESTDIR for the main build nor the regress target!
#
#####################
#
# Special Notes for Special Systems:
#
# NetBSD:
#
# N.B.:  It is assumed that only NetBSD's Mk-files define _BSD_OWN_MK_ (and in
# particular it is only defined in <bsd.own.mk>, which is in general included by
# all the other Mk-files).
#
#
# MacOS vs. various BMakes:
#
# OSX, aka macOS, since the release of Xcode 10(?) doesn't have a working
# bsdmake in the base system any longer.
#
# However the most recent version of BMake that can be installed from Homebrew
# does work.
#
# Manually installing sjg's BMake will also work, obviously.
#
# Unfortunately the BMake that comes with pkgsrc does not directly produce ideal
# results on macOS as it does not (yet?) use sjg's Mk-files but rather it uses
# the bootstrap-mk-files package, which (as of 20240210) has not yet been fully
# ported to OSX/Darwin (it is more or less just an out-of-date copy of the
# non-portable NetBSD Mk files with minor tweaks).  There are some hacks in
# src/Makefile to try to detect the bootstrap-mk-files and to produce a shared
# library, but they're not guaranteed!
#
# Alernatively if one can do without the shared library then the pkgsrc bmake
# will work as-is on macOS, and depending on the vintage of one's pkgsrc, it may
# also be necessary to pass "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" on the
# command line or in Makefile.inc to completely disable generation of the shlib.
#
# The really old Apple bsdmake does generate the correct shared library name,
# more or less, but it doesn't support .WAIT in the ${SUBDIR} list, so parallel
# builds are impossible with it.
#
# Old macOS systems with "bsdmake" may not have "mkdep" in the installed SDK
# even though the wrapper in /usr/bin/mkdep exists (and "xcrun -find mkdep"
# fails).  This can be worked around with:  MKDEPCMD=/Developer/usr/bin/mkdep
#
#
# OpenBSD:
#
# So note OpenBSD's make since 5.5 (and before 2.1) does NOT support
# $MAKEOBJDIRPREFIX at all.  For recent OpenBSD, just use $MAKEOBJDIR instead.
# See below for how to do this.
#
#
# FreeBSD:
#
# FreeBSD's mk-files don't work reliably with "obj" in the dependency list for
# "all".  There are workarounds below, but they rely on internal implementation
# details that may change at any time.  Their saving grace (as of at least 12.0)
# is they've implemented WITH_AUTO_OBJ, and it works, BUT ONLY IF YOU PUT
# "WITH_AUTO_OBJ=yes" ON THE COMMAND LINE OR IN THE ENVIRONMENT!  (Having it set
# in /etc/src-env.conf DOES NOT WORK for an out-of-/usr/src project.)
#
#
# Other BMake ports:
#
# See the first use of .WAIT below for comments about really old BMakes and
# Mk-files that don't deal with it properly.
#
# Note if you see the warning "duplicate script for target "yajl-showenv"
# ignored" (and its matching warning about the first definition), then you're
# using a version of BMake (and its Mk-files) that has some critical bugs which
# cannot be worked around (probably from between ???????? and 20200911).  The
# quickest fix is to remove an errant line in the file <subdir.mk> by applying
# the first hunk of this patch.  The remaining hunks fix parallel installs (this
# patch is against 20240404, and the next release after it may include all of
# these fixes):
#
#	--- mk/subdir.mk~	2024-02-25 11:22:39.000000000 -0800
#	+++ mk/subdir.mk	2024-03-28 05:30:17.744502242 -0700
#	@@ -37,7 +37,7 @@ __${_this}__: .NOTMAIN
#	 _SUBDIRUSE:
#	 .elif !commands(_SUBDIRUSE) && !defined(NO_SUBDIR) && !defined(NOSUBDIR)
#	 .-include <local.subdir.mk>
#	-.-include <${.CURDIR}/Makefile.inc>
#	+
#	 .if !target(.MAIN)
#	 .MAIN: all
#	 .endif
#	@@ -63,7 +63,7 @@ _SUBDIR_USE: .USE
#	 	else \
#	 		_nextdir_="$${_THISDIR_}/$$_dir"; \
#	 	fi; \
#	-	${ECHO_DIR} "===> $${_nextdir_} ($$_target)"; \
#	+	${ECHO_DIR} "===> $${_nextdir_} (${.TARGET} => $$_target)"; \
#	 	(cd ${.CURDIR}/$$_dir && \
#	 		${.MAKE} _THISDIR_="$${_nextdir_}" $$_target)
#	 
#	@@ -82,7 +82,7 @@ realinstall: beforeinstall _SUBDIRUSE
#	 
#	 # the interface from others
#	 # this may require additions to SUBDIR_TAREGTS
#	-_SUBDIRUSE: .USE subdir-${.TARGET}
#	+_SUBDIRUSE: .USE subdir-${.TARGET:C/-.*//:S/real//:S/.depend/depend/}
#	 
#	 SUBDIR_TARGETS += \
#	 	all \
#	@@ -93,7 +93,6 @@ SUBDIR_TARGETS += \
#	 	depend \
#	 	lint \
#	 	obj \
#	-	realinstall \
#	 	tags \
#	 	etags
#	 
#
# See the further discussion in src/Makefile.inc.
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
# This can be avoided by using the environment variable MAKEOBJDIR as follows
# (which more or less exactly simulates how MAKEOBJDIRPREFIX is used):
#
#	MAKEOBJDIR='${.CURDIR:S,^,'$(pwd -P)'/build,}' make
#
# If you end up with "obj.*" sub-directories and you want to go back to using a
# 'build' directory (as would be sane to do) then you can remove all the obj.*
# detritus with this command (the trailing, escaped, semicolon is important!):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj*' -exec rm -rf {}/obj.$(uname -m)-$(uname -p) \;
#
# XXX there really should be a way to remove all ${.OBJDIR} in <bsd.*.mk>!
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
# (and so in Simon's BMake since its inception, and up to bmake-20140214); and
# in OpenBSD's make since 2.1 when they first gained support up until 5.5 (after
# which they removed all support for MAKEOBJDIRPREFIX, a change not documented
# until 6.7!!!) were all extremely beligerent about having $MAKEOBJDIRPREFIX set
# in the environment and only in the environment -- they refused to even peek at
# it if it is only set on the command line, using only getenv(3) to access it.
#
# However in all but OpenBSD this has been fixed so that MAKEOBJDIRPREFIX can
# also be set on the command line.  I think.  I have had bad experiences with
# some versions (e.g. claiming to support setting it on the command line, but in
# fact not supporting that at all.  That's why the instructions above always
# show setting it in the environment.

#####################
#
#	Now, on with the show....
#
# Sub-directories of the project
#
# XXX to keep this Makefile generic it might be possible to define all subdirs
# in something like project_SUBDIR in Makefile.project, complete with any
# necessary .WAIT entries, then iterate through it here and remove .WAIT based
# on the (il)logic below.
#

SUBDIR =	src

# Some variants of Mk-files (e.g. NetBSD's) build subdirs in parallel (when
# "make -j N" with N>1 is used) and they support having a .WAIT in a SUBDIR
# list, and for them it is vital and necessary to have the .WAIT (.ORDER doesn't
# quite make up for it because of the fact these directories always exist prior
# to starting make).
#
# BMake with pkgsrc's bootstrap-mk-files are equivalent to NetBSD's native
# Mk-files, so work fine with '-j' so long as this .WAIT is present.  These set
# "unix" to "We Run Unix" in <sys.mk>.
#
# BMake with sjg's Mk-files doesn't build subdirs in parallel at all yet, and
# until 20240212 it does not support .WAIT in the SUBDIR list.  It seems like
# some older versions, notably 20181221, will try to run SUBDIRs in parallel,
# but don't support .WAIT, so we try to not allow it.
#
# BMake (i.e. native make if recent) on FreeBSD with FreeBSD's mk-files also
# does build subdirs in parallel IFF SUBDIR_PARALELL is defined, and it does
# require .WAIT in the SUBDIR list.  Note if SUBDIR_PARALLEL is not defined it
# will still work with '-j' and with or without this .WAIT as it is then
# equivalent to sjg's Mk-files as it does not run subdirs in parallel.
#
# The really old bsdmake on OSX/macOS has a wonky huge MAKE_VERSION=5200408120
#
# WARNING!!!  If .WAIT is not included in SUBDIR for any reason then DO NOT
# invoke parallel builds (no -j)!
#
.if !defined(MAKE_VERSION) || \
	(defined(unix) && ${unix} == "We run Unix" && ${MAKE} != "bsdmake") || \
	(${MAKE_VERSION} >= 20240212 && ${MAKE} != "bsdmake") || \
	(defined(.FreeBSD) && ${.FreeBSD} == "true")
SUBDIR +=	.WAIT
SUBDIR_PARALLEL = 1 # defined, for FreeBSD....
.elif (defined(.MAKE.JOBS) && (${.MAKE.JOBS} > 1) && \
	(!defined(MAKE_VERSION) || (${MAKE_VERSION} < 20200710 && ${MAKE} != "bsdmake"))) || \
	(${MAKE} == "bsdmake" && ${.MAKEFLAGS:M-j} == "-j")
#
# xxx:  only more recent bmake's define .MAKE.JOBS.
#
. error "Parallel builds not supported without .WAIT in SUBDIR list."
.endif

SUBDIR +=	doc
SUBDIR +=	reformatter
SUBDIR +=	verify
SUBDIR +=	example
SUBDIR +=	perf
SUBDIR +=	test

#####################
#
# The next section is mostly just default boilerplate for stand-alone project
# builds.  It could/should be in a separate included file.  (Except for some of
# the setting of ${SUBDIR} and ${bmake_install_dirs}.)
#
# Yes, "make obj" is forced -- it is stupid to build in the source directory)
#
# This does mean nothing else can be made in the top directory though.
#
# Note with some versions of Simon's "mk-files" this will cause obj* directories
# to be created in the existing obj* directories the second time around...
#
# N.B.:  These .WAIT's should be OK as they end up in a dependency list.
#
BUILDTARGETS +=	bmake-do-obj
#
# XXX extra hoops to jump through for (newer?) FreeBSD -- see below for more!
#
# Despite being included in the SUBDIR_TARGETS, obj, depend, and docs don't
# actually recurse into subdirs without all this extra goop!
#
.if (defined(.FreeBSD) && (${.FreeBSD} == "true"))
. for __dir in ${SUBDIR:N.WAIT}
BUILDTARGETS +=	obj_subdir_${__dir}
. endfor
.endif
BUILDTARGETS +=	.WAIT
# (forcing "make depend" is also both good, and necessary (see the beforedepend
# target in src/Makefile, though otherwise it is a bit of a waste for pkgsrc).
BUILDTARGETS +=	bmake-do-depend
.if (defined(.FreeBSD) && (${.FreeBSD} == "true"))
. for __dir in ${SUBDIR:N.WAIT}
BUILDTARGETS +=	depend_subdir_${__dir}
. endfor
.endif
BUILDTARGETS +=	.WAIT
# ("docs" should probably come after "all", but....)
BUILDTARGETS +=	bmake-do-docs
.if (defined(.FreeBSD) && (${.FreeBSD} == "true"))
. for __dir in ${SUBDIR:N.WAIT}
BUILDTARGETS +=	docs_subdir_${__dir}
. endfor
.endif

.for targ in ${BUILDTARGETS:N.WAIT:N*_subdir_*}
${targ}: .PHONY .MAKE ${targ:S/bmake-do-//}
.endfor

# XXX extra hoops to jump through for (newer?) FreeBSD
#
.if (defined(.FreeBSD) && (${.FreeBSD} == "true"))
. for __targ in obj depend docs
.  for __dir in ${SUBDIR:N.WAIT}
${__dir}: ${__targ}_subdir_${__dir} .PHONY .MAKE
.  endfor
. endfor
.endif

# this ("all") must be the VERY first target
# (there shouldn't be any .includes above, including Makefile.inc!)
#
all: .PHONY .MAKE bmake-test-obj .WAIT ${BUILDTARGETS}

# just in case old habits prevail
#
dependall: .PHONY all

# xxx the .WAIT's in here are probably not necessary???
.ORDER: bmake-test-obj bmake-do-obj bmake-do-depend bmake-do-docs

# XXX this is just a very crude check...  not as complete as the FreeBSD check
#
bmake-test-obj: .PHONY
	@if [ $$(pwd -P) = ${.CURDIR} -a ! -z "${MAKEOBJDIRPREFIX}" -a ! -d "${MAKEOBJDIRPREFIX}" ]; then echo "You must create ${MAKEOBJDIRPREFIX} first!"; false; fi

# n.b.:  Makefile.inc includes <bsd.own.mk>, which defines a default "all"
# target (amongst others), so it must come after all the above, but since it
# also defines additional values for variables used as .for lists it must come
# before <bsd.subdir.mk> and before anything else that uses values it sets in
# .for lists, e.g. the directories for bmake_install_dirs just below.
#
.include "${.CURDIR}/Makefile.inc"

beforeinstall: _bmake_install_dirs

# many BSD system mk files will not make directories on demand
_bmake_install_dirs: .PHONY
.for instdir in ${bmake_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# XXX This seems necessary for BMake (in pkgsrc and on Linux) to avoid parallel
# jobs during install from running ahead of the install directories being
# made....
#
${bmake_install_dirs:S|^|${DESTDIR}/|}: _bmake_install_dirs

# include the "standard" mk-file for building in sub-directories
#
.include <bsd.subdir.mk>

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
#
