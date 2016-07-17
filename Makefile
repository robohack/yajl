# -*-makefile-bsdmake-*-

.include "${.CURDIR}/Makefile.inc"

#
# This makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake
# (except on OSX), and Simon Gerraty's Bmake & Mk-files.
#
# Note:  Always use $MAKEOBJDIRPREFIX, ideally set to build somewhere
# entirely outside of the project source tree:
#
#	mkdir build
#	env MAKEOBJDIRPREFIX=$(pwd)/build bsdmake
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a
# sub-directory under /usr/obj, and others treat it as a sub-directory
# under ${.CURDIR}.  You have been warned.  Just use $MAKEOBJDIRPREFIX.
#
# To install the results you can do:
#
#	env MAKEOBJDIRPREFIX=$(pwd)/build bsdmake DESTDIR=$(pwd)/dist install
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the
# best way for out-of-tree builds, and it does not get in the way of
# pkgsrc either.)
#
# N.B.:  Do not specify DESTDIR for the build phase!

SUBDIR =	src

SUBDIR +=	reformatter
SUBDIR +=	verify
#SUBDIR +=	example

#SUBDIR +=	test
#SUBDIR +=	perf

.ORDER: ${SUBDIR}

#
# The rest is just default boilerplate for stand-alone builds....
#
# (yes, we force "make obj", though with Bmake that could also be done
# with MKOBJDIRS=auto -- it is stupid to build in the source directory)
#

BUILDTARGETS =	do-obj do-depend

# this must be the first target
#
all: .PHONY .MAKE ${BUILDTARGETS}

.ORDER: ${BUILDTARGETS}

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/do-//}
.endfor

beforeinstall: _yajl_install_dirs

# many BSD system mk files will not make directories on demand
_yajl_install_dirs: .PHONY
.for instdir in ${BINDIR} ${INCSDIR} ${LIBDIR} ${DEBUGDIR} ${DEBUGDIR}/bin ${DEBUGDIR}/lib ${LINTLIBDIR}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# XXX ???
DOXYGEN ?=	doxygen
docs:
	${DOXYGEN} -g src/YAJL.dxy

.include <bsd.subdir.mk>
