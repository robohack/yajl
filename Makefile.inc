# -*-makefile-bsdmake-*-

#
# Common setup for BSD Makefiles
#
# N.B.:  Nothing in here should be changed just to build libyajl.
#

YAJL_MAJOR =	2
YAJL_MINOR =	1
YAJL_MICRO =	0

YAJL_VERSION =	${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO}

OBJMACHINE =	yes

# N.B.:  testing in the build or 'install' (distribution) directories
# is "hard" (i.e. annoyingly more difficult than it should be) without
# dancing with the dynamic linker.  (Or suffering the horrors of even
# worse monstrosities, such as "GNU libtool"!)
#
# Setting LDSTATIC to the linker flag which forces static linking is
# the easy and best fix on most platforms which support full static
# linking.  However, sadly, OS X is not one of them.  As a result on
# OS X it is necessary to set the variable DYLD_LIBRERY_PATH in the
# environment to include the location of the library to be tested:
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
# Instead for OS X we use the often-used trick of appending "_s" or
# similar to the static library's name in order to hide the normal
# name of the dynamic library and to trick the linker into thinking
# there is only a static library available.  (This is actually done
# with a symlink in order to preserve normal naming conventions for
# the actual file.)
#
# Note that the .pc file as-built (for pkgconf, or the older
# pkg-config, may still require adjustments after a "dist" archive is
# un-packed since it may not have been original configured for the
# final installation directory).
#
#LDSTATIC =	-static

# N.B.:  ${DESTDIR} if set on the 'install' command line, it is also
# prepended to each of these target directories.
#
INSTALL_OPTS =	# for specific ownerships
# these will be over-ridden by pkgsrc
BINDIR ?= /bin
INCSDIR ?= /include/yajl
LIBDIR ?= /lib
DEBUGDIR ?= /libdata/debug
LINTLIBDIR ?= /libdata

# hmmmm....  this apparently has no precedent in the world of BSD Make
# base it off of LIBDIR....
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
# N.B.:  all sub-directory Makefiles must set ${yajl_topdir} to be a
# relative path to the top of the source tree.
#
yajl_srcdir =	${.CURDIR}/${yajl_topdir}/src

# This .if is annoying, but some older systems don't support CPPFLAGS.
#
.if defined(CPPFLAGS)
CPPFLAGS +=	-I${yajl_srcdir}
.else
CFLAGS +=	-I${yajl_srcdir}
.endif

.if !defined(LIB)
#
# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX}
# is tedious.
#
. if exists(${.OBJDIR}/${yajl_topdir}/src)
LDADD += -L${.OBJDIR}/${yajl_topdir}/src
. else
LDADD += -L../${yajl_topdir}/src/${.OBJDIR:T}
. endif
LDADD += -lyajl_s
.endif
