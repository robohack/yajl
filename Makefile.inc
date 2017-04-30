# -*-makefile-bsdmake-*-

# Common/shared setup for BSD Makefiles
#
# N.B.:  Nothing in here should be changed just to build libyajl.
#
# (Some settings can be overridden on the command-line or from environment
# variables though...)

YAJL_MAJOR =	2
YAJL_MINOR =	1
YAJL_MICRO =	0

YAJL_VERSION =	${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO}

# NOTICE:  Nothing below here _should_ be project dependent
# xxx except see INCSDIR and 2nd last line...  sigh.

OBJMACHINE =	yes

# For testing and debugging -- at least on sane systems, or with 'bmake'
#
# (note this can also be set on the command line)
#
#DBG = -O0 -g

# N.B.:  testing in the build or 'install' (distribution) directories, or making
# a distribution that can be un-packed on a target system in any arbitrary
# hierarchy (/usr/local, /opt, /usr/pkg, etc.)  is "hard" (i.e. annoyingly more
# difficult than it should be) without dancing with the dynamic linker.  (Or
# suffering the horrors of even worse monstrosities, such as "GNU libtool"!)
#
# Setting LDSTATIC to the linker flag which forces static linking is the easy
# and best fix on most platforms which support full static linking.  However,
# sadly, OS X is not one of them.  As a result on OS X it is necessary to set
# the variable DYLD_LIBRARY_PATH in the environment to include the location of
# the library to be tested:
#
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build
#	DYLD_LIBRARY_PATH=$(pwd)/build/$(pwd)/src $(pwd)/build/$(pwd)/example/parse_config
#
# or:
#
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build DESTDIR=$(pwd)/dist install
#	cd dist
#	DYLD_LIBRARY_PATH=lib bin/parse_config
#
# Instead for OS X we use the often-used trick of appending "_s" or similar to
# the static library's name in order to hide the normal name of the dynamic
# library and to trick the linker into thinking there is only a static library
# available.  (This is actually done with a symlink in order to preserve normal
# naming conventions for the actual file.)
#
# Note that the .pc file as-built (for pkgconf, or the older pkg-config, may
# still require adjustments after a "dist" archive is un-packed since it may not
# have been original configured for the final installation directory).
#
#LDSTATIC =	-static

# N.B.:  ${DESTDIR} if set on the 'install' command line, is also prepended to
# each of these target directories.
#
INSTALL_OPTS =	# for specific ownerships
# these will be over-ridden by pkgsrc
BINDIR ?= /bin
INCSDIR ?= /include/yajl
LIBDIR ?= /lib
DEBUGDIR ?= /libdata/debug
LINTLIBDIR ?= /libdata
SHAREDIR ?= /share

# hmmmm....  This apparently has no precedent in the world of BSD Make.  Base it
# off of LIBDIR....
PKGCONFIGDIR ?= ${LIBDIR}/pkgconfig

# xxx all this otherwise some systems won't allow unprivileged installs
.if ${USER} != "root"
BINOWN =	${USER}
BINGRP !=	id -g
LIBOWN =	${USER}
LIBGRP !=	id -g
SHAREOWN =	${USER}
SHAREGRP !=	id -g
FILESOWN =	${USER}
FILESGRP !=	id -g
NOBINMODE =	644		# else OSX bsdmake can't install!
DEBUGOWN =	${USER}
DEBUGGRP !=	id -g
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
# unrelated. In NetBSD MAKE_VERSION was only briefly present since it
# represented the build date, not the release date, and it was removed in order
# to make it possible to do reproducible builds.
#
bmake_srcdir =	${.CURDIR}/${bmake_topdir}/src

# This .if is annoying, but some older systems don't support CPPFLAGS.
#
.if defined(CPPFLAGS)
CPPFLAGS +=	-I${bmake_srcdir}
.else
CFLAGS +=	-I${bmake_srcdir}
.endif

# XXX this is, very unfortunately, project dependent!
#
.if !defined(LIB)
#
# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
. if exists(${.OBJDIR}/${bmake_topdir}/src)
LDADD += -L${.OBJDIR}/${bmake_topdir}/src
. else
LDADD += -L../${bmake_topdir}/src/${.OBJDIR:T}
. endif
# see src/Makefile for where LIB is set
# -l${LIB}_s
LDADD += -lyajl_s
.endif
