# -*- makefile-bsdmake -*-

#	Makefile.inc - Common/shared setup for BSD Makefiles
#
# This file is used by projects being built outside of a BSD source tree.
#
# The canonical source for this file is in https://github.com/robohack/yajl
#
# Copyright (C) 2026 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# WARNING!!!  Do not put any targets in here without great care to protect them!

# N.B.:  Normally nothing in here should be changed just to build this project.
#
# Some settings can be overridden on the command-line or from environment
# variables though.  See "bsdmake help".
#
# NOTICE:  Nothing below here _should_ be project dependent, however some
# settings such as BINDIR, etc. might need to be changed for other projects.

# XXX see SJG's SSLrsh release for simple portability hacks via Make e.g. in
# particular the use of .-include "Makefile.${.MAKE.OS}.inc", etc.

# For any subdirectory Makefiles this ${bmake_topdir} is provided to help find
# the project's root directory.
#
# N.B.:  the top-level Makefile overrides this value, of course!
#
bmake_topdir = ..
#
# ${bmake_topdir} is used to allow values in LDADD, DPADD, and CPPFLAGS (or
# CFLAGS) to directly reference both products and sources in other parts of the
# tree, especially if they are set in this file (though use directly in subdir
# makefiles can help them be more readable).  This is particularly useful for a
# project that builds a library used by other programs in the same project,
# and/or which may also build header files shared by several sources in
# different directories in the same project.
#
# For example this file can be used to define the version number(s) for the
# project, and thus sources which use these numbers (header file templates, or
# shared library filenames, etc), can add a dependency on this file as follows:
#
#	version.h: ${.CURDIR}/version.h.in ${.CURDIR}/${bmake_topdir}/Makefile.inc
#
# This file is included by default from any subdirectory makefile by BSD Make's
# standard files, or at least via any of the standard <bsd.*.mk> files which are
# used by makefiles intended to build some actual target.  Normally this happens
# very early on, just before <bsd.own.mk> is included, and it is done by
# sub-directory makes.  For most implementations certain system variables must
# be defined before <bsd.own.mk> is included, and so they can be defined here.
# See below though for why this also includes <bsd.own.mk> itself half-way
# through.  As such we depend on the standard <bsd.*.mk> files protecting
# themselves from multiple inclusions, and though this was not true on the
# original CSRG BSD releases it has been done now in all modern releases.
#
# Note also that this is only included by default by Makefiles using standard
# <bsd.*.mk> files in an immediate subdirectory -- i.e. it is done as follows:
#
#	.include "${.CURDIR}/../Makefile.inc"
#
# Thus the top level Makefile may need to directly include this file, but it
# must only do so after it has defined the default target (i.e. the .MAIN
# target, normally "all").  The top level Makefile should not (need to) use
# ${bmake_topdir}.
#
# This also means that in any project with deeper source trees it may be
# necessary to create a "Makefile.inc" in intermediate sub-directories in order
# for this file to be shared to the deeper sub-directories.  The intermediate
# "Makefile.inc" files should have the following content:
#
#	# -*- makefile-bsdmake -*-
#	#
#	# N.B.:  ${.CURDIR} here is not needed -- it's already implied because of the
#	# include done by <bsd.prog.mk> in the subdir's Makefile
#	.include "../Makefile.inc"
#	bmake_topdir := ${bmake_topdir}/..

#bmake_srcdir =	${.CURDIR}/${bmake_topdir}
#
#CPPFLAGS +=	-I${bmake_srcdir}

# This can define common project-specific variables, such as LIB_* version IDs
# which might be needed in subdirectory Makefiles.
#
# If a project is used inside a BSD source tree then Makefile.project should be
# renamed to Makefile.inc (i.e. replacing this file).
#
# N.B.:  ${.CURDIR} here is not needed here either because the directory this
# Makefile.inc was included from is part of the set of directories searched.
#
.-include "Makefile.project"

# The full version ID for a library
#
LIB_VERSION =	${LIB_MAJOR}.${LIB_MINOR}.${LIB_MICRO}

# note: FreeBSD <bsd.subr.mk> effectively turns off SUBDIR_PARALLEL for installs
#
.if make(install) && \
	defined(MAKE_VERSION) && ${MAKE_VERSION} < 20240414 && \
	(!defined(.FreeBSD) || ${.FreeBSD} != "true") && \
	${.MAKEFLAGS:M-j} == "-j"
. error "Parallel installs are not supported with older BMakes"
.endif

# For testing and debugging -- at least on sane systems, or with 'bmake'
#
# (note these can also be overridden on the command line)
#
# (note:  bmake puts optimizer flags in DBG)
#
#DBG ?= -g
DBG ?=		# set to '-g' or '-g2', etc. for debugging
#OPTIM ?= -O0
OPTIM ?=	# set to '-O0', etc. for debugging

LDSTATIC ?=	# set to '-static' for static linking

# If you are making changes and want to be sure to clean up all warnings....
#
#CWARNFLAGS +=	-Werror
# else
NOGCCERROR ?= 1
WARNS ?= 0

# Installation directories
#
# N.B.:  ${DESTDIR} if set on the 'install' command line, it is also prepended
# to each of these target directories for actual installation targets.
#
# XXX Note that NetBSD's bsd.shlib.mk magic keys off the value of BINDIR (or
# MKDYNAMICROOT) in order to set the location of SHLIBDIR and SHLINKDIR, which
# in turn then determine which dynamic linker is set for the generated binaries
# (when linking non-statically), so don't set PREFIX to an empty value if you
# don't have /libexec/ld.elf_so and you don't have MKDYNAMICROOT=no set
# somewhere (e.g. /etc/mk.conf)!
#
INSTALL_OPTS =	# for specific ownerships
#
# these *DIRs will be over-ridden by pkgsrc
#
# N.B.:  Normally in the system source tree an override of BINDIR is set in the
# Makefile.inc in the parent directory, e.g. for some program living in
# /usr/src/libexec/somedaemon, it would be in /usr/src/libexec/Makefile.inc
#
BINDIR ?=	/bin	# usually /bin, but may be /libexec, etc.
INCSDIR ?=	/include${INCSUBDIR}
LIBDIR ?=	/lib
DEBUGDIR ?=	/libdata/debug
LINTLIBDIR ?=	/libdata/lint
SHAREDIR ?=	/share
DOCDIR ?=	${SHAREDIR}/doc
MANDIR ?=	${SHAREDIR}/man

# hmmmm....  This apparently has no precedent in the world of BSD Make.  Base it
# off of LIBDIR....
PKGCONFIGDIR ?= ${LIBDIR}/pkgconfig

# xxx all this otherwise some systems won't allow unprivileged installs
.if ${USER} != "root"
USER_GRP !=	id -g
BINOWN =	${USER}
BINGRP =	${USER_GRP}
LIBOWN =	${USER}
LIBGRP =	${USER_GRP}
SHAREOWN =	${USER}
SHAREGRP =	${USER_GRP}
FILESOWN =	${USER}
FILESGRP =	${USER_GRP}
NOBINMODE =	644		# else OSX bsdmake can't install!
DEBUGOWN =	${USER}
DEBUGGRP =	${USER_GRP}
MANOWN =	${USER}
MANGRP =	${USER_GRP}
DOCOWN =	${USER}
DOCGRP =	${USER_GRP}
# xxx adapt for typo in mk-20150901 through at least mk-20170418
DOGGRP =	${USER_GRP}
.endif

# more modern mk-files install a ${PROG}.debug file in libdata or whatever
#
STRIPFLAG =	# empty, for bootstrap-mk-files and older bmakes
STRIP =		# empty, for bootstrap-mk-files and older bmakes

# more hacks to avoid missing system dependencies with DESTDIR set
DPLIBC =		# empty
LIBC =			# empty
LIBCRT0 =		# empty
LIBCRTI =		# empty
LIBCRTBEGIN =		# empty
LIBCRTEND =		# empty
SHLIB_LDSTARTFILE =	# empty
SHLIB_LDENDFILE =	# empty

# Finally some common settings

# yes, always -- we use this because it's safer when using a shared network
# directory for building/testing/etc.  (though it is sometimes somewhat
# annoying)
#
MKOBJ =			yes
OBJMACHINE ?=		yes
OBJMACHINE_ARCH ?=	yes
# XXX ideally this would be "auto", but that is not yet very widely supported.
# See Makefile.end.
MKOBJDIRS ?=		yes

# note MAKECONF may override
MKHTML ?=		yes	# set to 'no' to avoid building HTML manual pages

# xxx on NetBSD <bsd.sys.mk> sets this just to "paxctl", so if /usr/sbin isn't
# in the caller's PATH it fails
#
PAXCTL ?=		/usr/sbin/paxctl

# Because we don't do "make includes" in private projects, we need to allow
# other program products to be built with reference to the original source
# header files.
#
# The .if here is annoying, but some older systems, and modern FreeBSD, don't
# support CPPFLAGS.
#
.if defined(CPPFLAGS)
CPPFLAGS +=	-I${.CURDIR}/${bmake_topdir}/src
.else
CFLAGS +=	-I${.CURDIR}/${bmake_topdir}/src
.endif

# N.B.:  testing in the build or 'install' (distribution) directories, or making
# a distribution that can be un-packed on a target system in any arbitrary
# hierarchy (/usr/local, /opt, /usr/pkg, etc.)  is "hard" (i.e. annoyingly more
# difficult than it should be) without dancing with the dynamic linker.  (Or
# suffering the horrors of even worse monstrosities, such as "GNU libtool"!)
#
# Setting LDSTATIC to the linker flag which forces static linking is the easy
# and best fix on most platforms which support full static linking.
#
# This will only have effect on some BSDs, not Darwin/OSX/macOS, nor FreeBSD,
# and it is added to LDFLAGS by the system mk-files (sys.mk in general, but also
# <bsd.prog.mk> sometimes.  Note that most of the sanitizers don't work when
# static-linked.
#
#LDSTATIC =	-static
#
# However, sadly, OS X does not support a command-line way of forcing static
# linking (nor does Solaris?).  As a result on OS X if you want to test programs
# without first installing the library in its final destination directory then
# it is necessary to set the variable DYLD_LIBRARY_PATH in the environment to
# include the location of the library to be tested against.
#
# So, instead of making testing onerous for OS X we use a common trick of giving
# the static library a unique name, such as by appending "_s" or similar to it,
# in order to make it different from the normal name of the dynamic library and
# thus we can trick the linker into thinking there is only a static library
# available.  (This is actually done with a symlink in order to preserve normal
# naming conventions for the actual file.)
#
# Note that the .pc file as-built (for pkgconf, or the older pkg-config, may
# still require adjustments after a "dist" archive is un-packed since it may not
# have been original configured for the final installation directory).
#
# XXX this is, very unfortunately, somewhat ugly and package dependent!
#
# Program and test directories need to reference the static library so that they
# can be run from their build directory without the library having yet been
# installed, so we use "-l${PACKAGE_NAME:S/^lib//}_s" to do the linking.  (Note
# some packages include "lib" as the prefix in their package name!)
#
# Note:  In order to avoid problems with parallel builds LDADD and DPADD should
# not be set in src/Makefile, thus this .if:
#
# XXX Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
# XXX XXX how to handle both projects building a library, and those without?
#
# LDADD and DPADD here are intended for projects building a library and then
# also building some tools and/or tests that depend on said library.
#
# The !defined(LIB) avoids setting these when building the library itself, so
# for now we rely on a Makefile.project file setting LIB_MAJOR.
#
# see src/Makefile for where LIB is set
#
.if !defined(LIB) && defined(LIB_MAJOR)
. if defined(MAKEOBJDIRPREFIX)
# then .OBJDIR is from MAKEOBJDIRPREFIX
LDADD += -L${.OBJDIR}/${bmake_topdir}/src
DPADD += ${.OBJDIR}/${bmake_topdir}/src/lib${PACKAGE_NAME:S/^lib//}.a
. elif defined(MAKEOBJDIR)
LDADD += -L${MAKEOBJDIR}/${bmake_topdir}/src
DPADD += ${MAKEOBJDIR}/${bmake_topdir}/src/lib${PACKAGE_NAME:S/^lib//}.a
. else
# assume .OBJDIR is a local directory, so look for bmake_topdir from the parent
LDADD += -L../${bmake_topdir}/src/${.OBJDIR:T}
DPADD += ../${bmake_topdir}/src/${.OBJDIR:T}/lib${PACKAGE_NAME:S/^lib//}.a
. endif
LDADD += -l${PACKAGE_NAME:S/^lib//}_s
#
# If we're compiling programs to link with lib${PACKAGE_NAME}_s.a then on some
# systems we cannot compile or link them with -fPIC or -fPIE, so disable that
# here.
#
# Note this normally only affects <bsd.prog.mk> Makefiles, not libs, even if it
# wasn't wrapped here in ".if !defined(LIB)".
#
# Note however that most of the sanitizers don't work when static-linked.
#
. if (defined(TARGET_OSNAME) && (${TARGET_OSNAME} == "Darwin")) || defined(.FreeBSD)
# for both FreeBSD and Mac OS X....
NO_SHARED = YES
.else
NOSHARED = YES
. endif
.endif # !defined(LIB)

.if defined(MAKEOBJDIRPREFIX)
# .OBJDIR is from MAKEOBJDIRPREFIX
GENHDIR = ${.OBJDIR}/${bmake_topdir}/src
.elif defined(MAKEOBJDIR)
GENHDIR = ${MAKEOBJDIR}/${bmake_topdir}/src
.else
# assume .OBJDIR is a local directory, so look in the parent
GENHDIR += ${.OBJDIR}/../${bmake_topdir}/src/${.OBJDIR:T}
.endif

# Default build targets -- see Makefile.end
#
# Yes, "bsdmake obj" is forced -- it is stupid to build in the source directory)
# (forcing "bsdmake depend" is also good, though a bit of a waste for pkgsrc.
#
# Note with some versions of "bmake" this will cause obj* directories to be
# created in the existing obj* directories the second time around...
#
BUILDTARGETS =	bmake-do-obj
BUILDTARGETS +=	.WAIT

# Forcing "make depend" is also both good, and necessary as a beforedepend
# target in a subdirectory can be used to create "built" header files, for
# example, and an afterdepend may similarly be useful for other tricks.
#
BUILDTARGETS +=	bmake-do-depend
BUILDTARGETS +=	.WAIT

# ("docs" should probably come after "all", but should also be able to be built
# in parallel with sources)
BUILDTARGETS +=	bmake-do-docs
#BUILDTARGETS +=	.WAIT

# Special case additional convenience targets
#
# N.B.:  these must be included by all sub-directory Makefiles!
#
# In NetBSD all TARGETS are candidates for making in the SUBDIR list
#
# (but avoid creating TARGETS if it doesn't exist)
#
# XXX except it must be set before <bsd.subdir.mk> is included!!!
#
.if (defined(.FreeBSD) && (${.FreeBSD} == "true"))
#
# N.B.:  MK_AUTO_OBJ=yes must be set in the environment or on the command line.
#
# In FreeBSD and sjg's Mk-files the list of SUBDIR_TARGETS cannot be appended to
# after including <bsd.subdir.mk>, but most of the interesting targets, such as
# some of these below, are not added to the default value of SUBDIR_TARGETS
# until it is included, so it's pointless to check here if they're already in
# the list.
#
# Note:  In FreeBSD SUBDIR_TARGETS was once reserved for adding custom targets
# that should recurse into subdirs, and the main meat was in ALL_SUBDIR_TARGETS,
# but again that couldn't be seen until after <bsd.subdir.mk> was included.
#
# "regress" was once a default for FreeBSD, but not for some time now.
#
SUBDIR_TARGETS +=	obj
#SUBDIR_TARGETS +=	depend
SUBDIR_TARGETS +=	docs
SUBDIR_TARGETS +=	regress
SUBDIR_TARGETS +=	showenv
SUBDIR_TARGETS +=	TAGS
.else				# for NetBSD
. if empty(TARGETS:Mdocs)
TARGETS +=	docs
. endif
. if empty(TARGETS:Mregress)
TARGETS +=	regress
. endif
. if empty(TARGETS:Mtags)
TARGETS +=	tags
. endif
. if empty(TARGETS:MTAGS)
TARGETS +=	TAGS
. endif
. if empty(TARGETS:Mshowenv)
TARGETS +=	showenv
. endif
.endif

# default targets for the extra ones added to TARGETS above (to avoid "don't
# know how to make" errors)
#
# N.B.:  the attempt to use !target() may fail if including <bsd.own.mk> has not
# defined the proper default targets (i.e. if they are defined somewhere later
# after this file is already parsed), but for these this will not likely matter.

# Most implementations do not make 'regress' depend on first building everything
# but we need to build everything before we can do any testing.  This is done
# because 'regress' is an optional target and meant to be called directly, and
# so it should probably try its best to make sure everything is up to date
# before it tries to test those things.
#
# XXX for some reason even NetBSD Make requires the odd trailing .WAIT
#
.if !target(regress)
regress: .NOTMAIN all .WAIT
.endif

.if !target(tags)
# avoid :U for older bmakes
. if !defined(TOOL_CTAGS)
_TOOL_CTAGS = ctags
. else
_TOOL_CTAGS = ${TOOL_CTAGS}
. endif
tags: .NOTMAIN ${SRCS}
. if defined(SRCS)
	-cd ${.CURDIR} && ${_TOOL_CTAGS} -f /dev/stdout ${.ALLSRC:N*.h} | \
	    ${TOOL_SED} "s;\${.CURDIR}/;;" > ${.TARGET}
. endif
.endif

.if !target(TAGS)
# avoid :U for older bmakes
. if !defined(TOOL_ETAGS)
_TOOL_ETAGS = etags
. else
_TOOL_ETAGS = ${TOOL_ETAGS}
. endif
TAGS: .NOTMAIN ${SRCS}
. if defined(SRCS)
	-cd ${.CURDIR} && ${_TOOL_ETAGS} -I -l c -o ${.TARGET} ${.ALLSRC:N*.h}
. endif
.endif

.if !target(docs)
docs: .NOTMAIN .PHONY
.endif

# Makefile debugging support
#
.if !target(showenv)
showenv: .NOTMAIN .PHONY _GAW_bmake-showenv

_GAW_bmake-showenv: .NOTMAIN .PHONY .SILENT
	echo ""
	echo "bmake_topdir = '${bmake_topdir}'"
	echo "${.OBJDIR}/${bmake_topdir}/src"
	if [ -d "${.OBJDIR}/${bmake_topdir}/src" ]; then \
		echo "the directory shown above exists!"; \
	fi
	echo "Makefile.inc: ${.CURDIR}/${bmake_topdir}/Makefile.inc"
	if [ -f "${.CURDIR}/${bmake_topdir}/Makefile.inc" ]; then \
		echo "the file shown above exists!"; \
	else \
		echo "Where is ${.CURDIR}/${bmake_topdir}/Makefile.inc?"; \
	fi
	echo ""
	echo "Some Interesting Vars:"
	echo "unix = ${unix:Q}"
	echo "MAKE_VERSION = ${MAKE_VERSION:Q}"
	echo ".FreeBSD = ${.FreeBSD:Q}"
	echo ".CURDIR = ${.CURDIR:Q}"
.if defined(MAKEOBJDIR)
	echo "MAKEOBJDIR = ${MAKEOBJDIR:Q}"
.else
	echo "undefined(MAKEOBJDIR)"
.endif
.if defined(MAKEOBJDIRPREFIX)
	echo "MAKEOBJDIRPREFIX = ${MAKEOBJDIRPREFIX:Q}"
.else
	echo "undefined(MAKEOBJDIRPREFIX)"
.endif
	echo ".OBJDIR = ${.OBJDIR:Q}"
	echo ".OBJDIR:T = ${.OBJDIR:T:Q}"
	echo "MKOBJDIRS = ${MKOBJDIRS:Q}"
	echo "_BSD_OWN_MK_ = ${_BSD_OWN_MK_:Q}"
	echo "GENHDIR = ${GENHDIR:Q}"
	echo "DEBUGDIR = ${DEBUGDIR:Q}"
	echo "LINTLIBDIR = ${LINTLIBDIR:Q}"
	echo "MAKECONF = ${MAKECONF:Q}"
	echo "MKDEBUG = ${MKDEBUG:Q}"
	echo "MKDEBUGLIB = ${MKDEBUGLIB:Q}"
	echo "MKDYNAMICROOT = ${MKDYNAMICROOT:Q}"
	echo "MKLINT = ${MKLINT:Q}"
	echo "LDADD = ${LDADD:Q}"
	echo "DPADD = ${DPADD:Q}"
	echo "MANDIR = ${MANDIR:Q}"
	echo "INC_COPY = ${INC_COPY:Q}"
	echo "FILES_COPY = ${FILES_COPY:Q}"
	echo ""
.endif

# finally we include Makefile.compiler
#
.include "Makefile.compiler"

# to use MAKEOBJDIR instead:
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(pwd -P)','$(pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make
#
# This substitutes the "top" $PWD directory path, found and expanded at command
# execution time, with itself plus the appended build directory name, leaving
# the last part of the path untouched.  Thus for sub-directories it "just
# works".  Note that within subdirectories we can make use of make's idea of
# ${bmake_topdir} via nested command substitution to find the "top" directory's
# path (but don't use this here at the "top"!):
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -p); mkdir -p ${BUILD_DIR}; MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static all")
# End:
#
