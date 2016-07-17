# -*-makefile-bsdmake-*-

YAJL_MAJOR =	2
YAJL_MINOR =	1
YAJL_MICRO =	0

YAJL_VERSION =	${YAJL_MAJOR}.${YAJL_MINOR}.${YAJL_MICRO}

OBJMACHINE =	yes

# N.B.:  ${DESTDIR} if set on the 'install' command line, it is also
# prepended to each of these target directories.
#
INSTALL_OPTS =	# for specific ownerships
# these will be over-ridden by pkgsrc
BINDIR ?= /bin
INCSDIR ?= /include
LIBDIR ?= /lib
DEBUGDIR ?= /libdata/debug
LINTLIBDIR ?= /libdata

# xxx otherwise some systems won't allow unprivileged installs
.if ${USER} != "root"
BINOWN =	${USER}
BINGRP !=	id -g
NOBINMODE =	644		# else OSX bsdmake can't install!
DEBUGOWN =	${USER}
DEBUGGRP !=	id -g
.endif

STRIP =		# empty (needed for OSX bsdmake install)

# hacks to prevent system dependencies with DESTDIR set
LIBC =			# empty
LIBCRT0 =		# empty
LIBCRTI =		# empty
LIBCRTBEGIN =		# empty
LIBCRTEND =		# empty
SHLIB_LDSTARTFILE =	# empty
SHLIB_LDENDFILE =	# empty
