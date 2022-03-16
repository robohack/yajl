# -*- makefile-bsdmake -*-

#	Common/shared setup for BSD Makefiles
#
# N.B.:  Nothing in here should be changed just to build this project.
#
# (Some settings can be overridden on the command-line or from environment
# variables though...)

# XXX see SJG's SSLrsh release for simple portability hacks via Make
# e.g. in particular the use of .-include "$Makefile.${MACHINE}.inc" etc.

# N.B.:  all sub-directory Makefiles must set ${bmake_topdir} to be a relative
# path to the top of the source tree.
#
# This is used to allow values in LDADD, DPADD, and CPPFLAGS (or CFLAGS) to
# directly reference both products and sources in other parts of the tree,
# especially if they are set in this file (though use directly in subdir
# makefiles can help them be more readable).  This is particularly useful for a
# project that builds a library used by other programs in the same project,
# and/or which may also build header files shared by several sources in
# different directories in the same project.
#
# This would not be necessary if/when all supported BSDMake versions have the
# ${.MAKE.LEVEL} feature.  ${.MAKE.LEVEL} was first found in NetBSD-current
# after 2009/09/08 but it is not found in any release until 6.0.  Unfortunately
# there is no easy way to know definitively what version of BSDMake is being
# used.  Simon Gerraty's bmake has MAKE_VERSION, as does FreeBSD's make though
# they are entirely and totally different and unrelated.  In NetBSD MAKE_VERSION
# was only briefly present since it represented the build date, not the release
# date, and it was removed in order to make it possible to do reproducible
# builds.  For example this file can be used to define the version number(s) for
# the project, and thus sources which use these numbers (header file templates,
# or shared library filenames, etc), can add a dependency on this file as
# follows:
#
#	version.h: ${.CURDIR}/version.h.in ${.CURDIR}/${bmake_topdir}/Makefile.inc
#
# On the other hand this might make it possible to invoke "make" in a
# subdirectory when _not_ using MAKEOBJDIRPREFIX.

# This file is included by default from any subdirectory makefile by BSD Make's
# standard files, or at least via any of the standard <bsd.*.mk> files which are
# used by makefiles intended to build some actual target.  Normally this happens
# very early on, just before <bsd.own.mk> is included, and it is done by
# sub-directory makes.  For most implementations certain system variables must
# be defined before <bsd.own.mk> is included, and so they can be defined here.
# See below though for why this also includes <bsd.own.mk> itself
# half-way-through.  As such we depend on the standard <bsd.*.mk> files
# protecting themselves from multiple inclusions, and though this was not true
# on the original CSRG BSD releases it has been done now in all modern releases.
#
# Note also that this is only included by default by makefiles using standard
# <bsd.*.mk> files in an immediate subdirectory -- i.e. it is done as follows:
#
#	.include "${.CURDIR}/../Makefile.inc"
#
# Thus the top level makefile may need to directly include this file, but it
# should only do so after it has defined any default target (i.e. the .MAIN
# target).
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


PACKAGE =	yajl

# The API says: "#include <yajl/yajl.h>"
#
INCSUBDIR =	/yajl

LIB_MAJOR =	2
LIB_MINOR =	1
LIB_MICRO =	0

LIB_VERSION =	${LIB_MAJOR}.${LIB_MINOR}.${LIB_MICRO}

#####################
#
# The rest is default boilerplate common to most projects....
#

# For testing and debugging -- at least on sane systems, or with 'bmake'
#
# (note these can also be overridden on the command line)
#
# (note:  bmake puts optimizer flags in DBG)
#
#DBG ?= -g
#OPTIM ?= -O0

# Contrary to what Makefile.compiler claims, do enable system-specific warnings
# -- this will likely double up on many of the flags added by Makefile.compiler,
# but it may also add some target-specific flags that might be useful.
#
# Of course this is just for developers -- it could be commented out for users.
#
WARNS = 10

# However do not enable -Werror, by default.
#
#NOGCCERROR = 1

# Note that with good compiler warnings it is possible to test standards
# compliance of this code with a build using something like the following:
#
# mkdir -p build ; CFLAGS=-std=c89 make MAKEOBJDIRPREFIX=$(pwd -P)/build CXREF=true
#
# (note how CFLAGS must be set in the environment, not on the command line, so
# as to allow it to be appended to by Makefile.compiler and the system mk-files,
# notably <bsd.sys.mk>)


# N.B.:  testing in the build or 'install' (distribution) directories, or making
# a distribution that can be un-packed on a target system in any arbitrary
# hierarchy (/usr/local, /opt, /usr/pkg, etc.)  is "hard" (i.e. annoyingly more
# difficult than it should be) without dancing with the dynamic linker.  (Or
# suffering the horrors of even worse monstrosities, such as "GNU libtool"!)
#
# Setting LDSTATIC to the linker flag which forces static linking is the easy
# and best fix on most platforms which support full static linking.
#
#LDSTATIC =	-static
#
# However, sadly, OS X does not support a command-line way of forcing static
# linking.  As a result on OS X if you want to test programs without first
# installing the library in a system directory then it is necessary to set the
# variable DYLD_LIBRARY_PATH in the environment to include the location of the
# library to be tested against.
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
# program and test directories need to reference the static library, but we
# cannot just use the portable form of "-l${LIB}_s" because LIB is not always
# set and cannot always be set
#
# see src/Makefile for where LIB is set
#
# So instead we refer to ${PACKAGE}, assuming it has the same name...
#
# XXX Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
.if !defined(LIB)
. if defined(MAKEOBJDIRPREFIX)
# .OBJDIR is from MAKEOBJDIRPREFIX
LDADD += -L${.OBJDIR}/${bmake_topdir}/src
DPADD += ${.OBJDIR}/${bmake_topdir}/src/lib${PACKAGE}.a
. else
# assume .OBJDIR is a local directory, so look for bmake_topdir from the parent
# XXX this probably breaks for ${MAKEOBJDIR}
LDADD += -L../${bmake_topdir}/src/${.OBJDIR:T}
DPADD += ../${bmake_topdir}/src/${.OBJDIR:T}/lib${PACKAGE}.a
. endif
LDADD += -l${PACKAGE}_s
#
.endif	# !defined(LIB)

.if defined(MAKEOBJDIRPREFIX)
# .OBJDIR is from MAKEOBJDIRPREFIX
GENHDIR = ${.OBJDIR}/${bmake_topdir}/src
.else
# assume .OBJDIR is a local directory, so look in the parent
# XXX this probably breaks for ${MAKEOBJDIR}
GENHDIR += ${.OBJDIR}/../${bmake_topdir}/src/${.OBJDIR:T}
.endif

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

#####################
#
# NOTICE:  Nothing at all below here should be project dependent
#

# yes, always -- we use this because it's safer when using a shared network
# directory for building/testing/etc.  (though it is sometimes somewhat
# annoying)
#
OBJMACHINE ?=	yes

# Unfortunately most MK files don't provide a "beforecleandir" or
# "beforedistclean".
#
#beforecleandir: cleandir
#	rmdir ${.OBJDIR}

# N.B.:  ${DESTDIR} if set on the 'install' command line, it is also prepended
# to each of these target directories.
#
# XXX Note that NetBSD's bsd.shlib.mk magic keys off the value of BINDIR (or
# MKDYNAMICROOT) in order to set the location of SHLIBDIR and SHLINKDIR, which
# in turn then determine which dynamic linker is set for the generated binaries
# (when linking non-statically), so don't set PREFIX to an empty value if you
# don't have /libexec/ld.elf_so and you don't have MKDYNAMICROOT=no set
# somewhere (e.g. /etc/mk.conf)!
#
INSTALL_OPTS =	# for specific ownerships
# these will be over-ridden by pkgsrc
PREFIX ?= /usr
BINDIR ?= ${PREFIX}/bin
INCSDIR ?= ${PREFIX}/include${INCSUBDIR}
LIBDIR ?= ${PREFIX}/lib
DEBUGDIR ?= ${PREFIX}/libdata/debug
LINTLIBDIR ?= ${PREFIX}/libdata/lint
SHAREDIR ?= ${PREFIX}/share
DOCDIR ?= ${SHAREDIR}/doc
MANDIR ?= ${SHAREDIR}/man

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

STRIP =		# empty (needed for OSX bsdmake install)

# more hacks to avoid missing system dependencies with DESTDIR set
DPLIBC =		# empty
LIBC =			# empty
LIBCRT0 =		# empty
LIBCRTI =		# empty
LIBCRTBEGIN =		# empty
LIBCRTEND =		# empty
SHLIB_LDSTARTFILE =	# empty
SHLIB_LDENDFILE =	# empty

# Because we don't do "make includes" in this project, we need to allow other
# program products to be built with reference to the original source header
# files.
#
# The .if here is annoying, but some older systems don't support CPPFLAGS.
#
.if defined(CPPFLAGS)
CPPFLAGS +=	-I${.CURDIR}/${bmake_topdir}/src
.else
CFLAGS +=	-I${.CURDIR}/${bmake_topdir}/src
.endif

# XXX N.B.:  we cannot include <bsd.own.mk> before the default "all" target!!!
#
.include <bsd.own.mk>

.if !empty(TARGETS)
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
.else
# In FreeBSD the list of SUBDIR_TARGETS cannot be appended to after including
# <bsd.subdir.mk>, but most of the interesting targets, such as some of these
# below, are not added to the default value of SUBDIR_TARGETS until it is
# included, so it's pointless to check here if they're already in the list.
SUBDIR_TARGETS +=	docs
SUBDIR_TARGETS +=	regress
SUBDIR_TARGETS +=	tags
SUBDIR_TARGETS +=	TAGS
SUBDIR_TARGETS +=	showenv
.endif

# Most implementations do not make 'regress' depend on first building everything
# but we need to build everything before we can do any testing.  This is done
# because 'regress' is an optional target and meant to be called directly, and
# so it should probably try its best to make sure everything is up to date
# before it tries to test those things.
#
# XXX for some reason even NetBSD Make requires the odd trailing .WAIT
#
regress: all .WAIT

#
# some default targets for the extra ones added to TARGETS above
#
# N.B.:  the attempt to use !target() may fail if including <bsd.own.mk> has not
# defined the proper default targets (i.e. if they are defined somewhere later
# after this file is already parsed), but for these this will not likely matter.

.if !target(tags)
# avoid :U for older bmakes
.if !defined(TOOL_CTAGS)
_TOOL_CTAGS = ctags
.else
_TOOL_CTAGS = ${TOOL_CTAGS}
.endif
tags: ${SRCS}
.if defined(SRCS)
	-cd ${.CURDIR} && ${_TOOL_CTAGS} -f /dev/stdout ${.ALLSRC:N*.h} | \
	    ${TOOL_SED} "s;\${.CURDIR}/;;" > ${.TARGET}
.endif
.endif

.if !target(TAGS)
# avoid :U for older bmakes
.if !defined(TOOL_ETAGS)
_TOOL_ETAGS = etags
.else
_TOOL_ETAGS = ${TOOL_ETAGS}
.endif
TAGS: ${SRCS}
.if defined(SRCS)
	-cd ${.CURDIR} && ${_TOOL_ETAGS} -I -l c -o ${.TARGET} ${.ALLSRC:N*.h}
.endif
.endif

.if !target(docs)
docs:
.endif

# finally we include Makefile.compiler
#
.include "Makefile.compiler"

# Makefile debugging support
#
showenv: .PHONY
	@echo ""
	@echo "${.OBJDIR}/${bmake_topdir}/src"
.if exists(${.OBJDIR}/${bmake_topdir}/src)
	@echo "the directory shown above exists!"
.endif
	@echo "../${bmake_topdir}/src/${.OBJDIR:T}"
	@echo ""
	@echo "Vars:"
	@echo ".CURDIR = ${.CURDIR:Q}"
	@echo ".OBJDIR = ${.OBJDIR:Q}"
	@echo ".OBJDIR:T = ${.OBJDIR:T:Q}"
	@echo "_BSD_OWN_MK_ = ${_BSD_OWN_MK_:Q}"
	@echo "GENHDIR = ${GENHDIR:Q}"
	@echo "DEBUGDIR = ${DEBUGDIR:Q}"
	@echo "LINTLIBDIR = ${LINTLIBDIR:Q}"
	@echo "MAKECONF = ${MAKECONF:Q}"
	@echo "MKDEBUG = ${MKDEBUG:Q}"
	@echo "MKDEBUGLIB = ${MKDEBUGLIB:Q}"
	@echo "MKDYNAMICROOT = ${MKDYNAMICROOT:Q}"
	@echo "MKLINT = ${MKLINT:Q}"
	@echo ""
#	@echo "Environment:"
#	env | sort

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
#
