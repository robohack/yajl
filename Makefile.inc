# -*- makefile-bsdmake -*-

#	Common/shared setup for BSD Makefiles
#
# N.B.:  Nothing in here should be changed just to build this project.
#
# (Some settings can be overridden on the command-line or from environment
# variables though...)

# XXX see SJG's SSLrsh release for simple portability hacks via Make
# e.g. in particular the use of .-include "$Makefile.${MACHINE}.inc" etc.

PACKAGE =	yajl

INCSUBDIR =	/yajl

YAJL_MAJOR =	2
YAJL_MINOR =	1
YAJL_MICRO =	0

YAJL_VERSION =	${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO}

# For testing and debugging -- at least on sane systems, or with 'bmake'
#
# (note these can also be overridden on the command line)
#
# (note:  bmake puts optimizer flags in DBG)
#
#DBG ?= -g
#OPTIM ?= -O0

NOGCCERROR = 1
WARNS = 0

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
# So instead we refer to $PACKAGE, assuming it has the same name...
#
.if !defined(LIB)
#
# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
# XXX ??? just use this: . if defined(MAKEOBJDIRPREFIX)
. if exists(${.OBJDIR}/${bmake_topdir}/src)
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

#
# NOTICE:  Nothing below here _should_ be project dependent
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

# Finally some common settings
#
# N.B.:  all sub-directory Makefiles must set ${bmake_topdir} to be a relative
# path to the top of the source tree.  This would not be necessary if/when all
# supported BSDMake versions have the ${.MAKE.LEVEL} feature.  ${.MAKE.LEVEL}
# was first found in NetBSD-current after 2009/09/08 but it is not found in any
# release until 6.0.  Unfortunately there is no easy way to know definitively
# what version of BSDMake is being used.  Simon Gerraty's bmake has
# MAKE_VERSION, as does FreeBSD's make though they are entirely and totally
# different and unrelated.  In NetBSD MAKE_VERSION was only briefly present
# since it represented the build date, not the release date, and it was removed
# in order to make it possible to do reproducible builds.
#
bmake_srcdir =	${.CURDIR}/${bmake_topdir}/src

# This .if is annoying, but some older systems don't support CPPFLAGS.
#
.if defined(CPPFLAGS)
CPPFLAGS +=	-I${bmake_srcdir}
.else
CFLAGS +=	-I${bmake_srcdir}
.endif

# XXX N.B.:  we cannot include <bsd.own.mk> before the default "all" target!!!
# xxx this doesn't actually set SUBDIR_TARGETS for FreeBSD anyway...
#
.include <bsd.own.mk>

# GAK!  So many differences in implementations!
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
.else
# For FreeBSD SUBDIR_TARGETS is first appended to in <bsd.subdir.mk>, but cannot
# be appended to afterwards (though could be appended to in something included
# by <bsd.init.mk>, such as "local.init.mk", "../Makefile.inc", or
# "/etc/make.conf", but since local.init.mk isn't portable then none of these
# are useful to us here, and so we cannot check to see if it includes a value
# already or not.
SUBDIR_TARGETS +=	docs
SUBDIR_TARGETS +=	regress
SUBDIR_TARGETS +=	tags
SUBDIR_TARGETS +=	TAGS
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

# see Makefile.compiler for default targets for the rest of the TARGETS above

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
#
