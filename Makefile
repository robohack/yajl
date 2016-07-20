# -*-makefile-bsdmake-*-

.include "${.CURDIR}/Makefile.inc"

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake (on
# OSX/Darwin one must set "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY="
# because that Pkgsrc's bootstrap-mk-files (as of 20160411) are not
# yet fully ported to OSX/Darwin), and Simon Gerraty's Bmake &
# Mk-files from http://www.crufy.net/FreeWare/.
#
# Pkgsrc will install on a vast number of systems, including
# MS-Windows with Cygwin.  Simon's Bmake works on many Unix-like
# systems.
#
# Note:  You can use MAKEOBJDIRPREFIX as so to build everything
# elsewhere, even withing a single sub-directory of the top of the
# source tree (i.e. instead of polluting the rest of the source tree
# with "obj" sub-directories):
#
#	mkdir build
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build
#
# However doing so more or less implies always invoking the build at
# the top of the source tree, with MAKEOBJDIRPREFIX set either on the
# command-line or in the environment).
#
# If you don't use MAKEOBJDIRPREFIX then "obj.${MACHINE}"
# sub-directories will be created for each directory with products,
# except on OSX where the "bsdmake obj" facility is somewhat broken
# and by default either assumes "/usr/obj" exists (on older systems
# with a native "bsdmake"), or uses "/usr/local/Cellar/bsdmake/24/obj"
# for systems with Apple "bsdmake" installed by Nomebrew.  You can
# avoid this by creating "obj.${MACHINE}" sub-directories in every
# source sub-directory first, like this (assuming you're using git):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj.*' ! -name . -exec mkdir {}/obj.$(uname -m) \;
#
# Then use "bsdmake NO_OBJ=yes" to build, etc.
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a
# sub-directory under /usr/obj, and others treat it as a sub-directory
# under ${.CURDIR}.  You have been warned.  Just use $MAKEOBJDIRPREFIX.
#
# To install the results you can do:
#
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build DESTDIR=$(pwd)/dist install
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the
# best way for out-of-tree builds, and it does not get in the way of
# pkgsrc either.)
#
# N.B.:  Do not specify DESTDIR for the build phase!

SUBDIR =	src

# Not all older mk-files support having a .WAIT in a SUBDIR list, but
# it is vital and necessary for parallel builds (i.e. use of 'make -j')
#
#SUBDIR +=	.WAIT

SUBDIR +=	reformatter
SUBDIR +=	verify
SUBDIR +=	example
SUBDIR +=	perf
SUBDIR +=	test

.ORDER: ${SUBDIR}

#
# The rest is just default boilerplate for stand-alone builds....
#
# (yes, "make obj" is forced -- it is stupid to build in the source directory)
#

BUILDTARGETS =	yajl-do-obj yajl-do-depend

# this must be the first target
#
all: .PHONY .MAKE ${BUILDTARGETS}

.ORDER: ${BUILDTARGETS}

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/yajl-do-//}
.endfor

yajl_install_dirs += ${BINDIR}
yajl_install_dirs += ${INCSDIR}
yajl_install_dirs += ${LIBDIR}
yajl_install_dirs += ${PKGCONFIGDIR}
yajl_install_dirs += ${DEBUGDIR}
yajl_install_dirs += ${DEBUGDIR}/bin
yajl_install_dirs += ${DEBUGDIR}/lib
yajl_install_dirs += ${LINTLIBDIR}

beforeinstall: _yajl_install_dirs

# many BSD system mk files will not make directories on demand
_yajl_install_dirs: .PHONY
.for instdir in ${yajl_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# XXX ???
DOXYGEN ?=	doxygen
docs:
	${DOXYGEN} -g src/YAJL.dxy

.include <bsd.subdir.mk>
