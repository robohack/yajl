# -*-makefile-bsdmake-*-

.include "${.CURDIR}/Makefile.inc"

# This Makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake (on OSX/Darwin
# one must set "SHLIB_MAJOR= SHLIB_MINOR= SHLIB_TEENY=" because Pkgsrc's
# bootstrap-mk-files (as of 20160411) are not yet fully ported to OSX/Darwin),
# and Simon Gerraty's Bmake & Mk-files from http://www.crufy.net/FreeWare/.
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Simon's Bmake works on many Unix-like systems.
#
# Note:  You can use MAKEOBJDIRPREFIX as so to build everything elsewhere, even
# withing a single sub-directory of the top of the source tree (i.e. instead of
# polluting the rest of the source tree with "obj" sub-directories):
#
#	mkdir build
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build
#
# However doing so more or less implies always invoking the build at the top of
# the source tree, with MAKEOBJDIRPREFIX set either on the command-line or in
# the environment).
#
# If you don't use MAKEOBJDIRPREFIX then "obj.${MACHINE}" sub-directories will
# be created for each directory with products, except on OSX where the "bsdmake
# obj" facility is somewhat broken and by default either assumes "/usr/obj"
# exists (on older systems with a native "bsdmake"), or uses
# "/usr/local/Cellar/bsdmake/24/obj" for systems with Apple "bsdmake" installed
# by Homebrew.  You can avoid this by creating "obj.${MACHINE}" sub-directories
# in every source sub-directory first, like this (assuming you're using git):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj.*' ! -name . -exec mkdir {}/obj.$(uname -m) \;
#
# Then use "bsdmake NO_OBJ=yes" to build, etc.
#
# If you want to (as would be sane to do) go back to using a 'build' directory
# then you can remove all the obj.* detritus with:
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj.*' ! -name . -exec rm -rf {}/obj.$(uname -m) \;
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a sub-directory under
# /usr/obj, and others treat it as a sub-directory under ${.CURDIR}.  You have
# been warned.  Just use $MAKEOBJDIRPREFIX.
#
# If you use MAKEOBJDIRPREFIX then to install the results into a "dist" subtree
# (which you can then distribute as a binary distribution that can be un-packed
# wherever desired) you can do:
#
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build DESTDIR=$(pwd)/dist install
#
# If you don't use MAKEOBJDIRPREFIX, then it's just:
#
#	bsdmake DESTDIR=$(pwd)/dist install
#
# DESTDIR can of course be any directory, e.g. /usr/local.
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it does not get in the way of pkgsrc either.)
#
# N.B.:  Do not specify DESTDIR for the build phase!

SUBDIR =	src

# Not all older mk-files support having a .WAIT in a SUBDIR list, but it is
# vital and necessary for parallel builds (i.e. use of 'make -j')
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
# Note with "bmake" this will cause obj* directories to be created in the
# existing obj* directories the second time around...
#

BUILDTARGETS =	bmake-do-obj bmake-do-depend

# this must be the first target
#
all: .PHONY .MAKE ${BUILDTARGETS}

.ORDER: ${BUILDTARGETS}

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/bmake-do-//}
.endfor

# most implementations do not make 'regress' depend on first building everything
# but we need to build everything before we can do any testing
#
regress: .PHONY all

bmake_install_dirs += ${BINDIR}
bmake_install_dirs += ${INCSDIR}
bmake_install_dirs += ${LIBDIR}
bmake_install_dirs += ${PKGCONFIGDIR}
bmake_install_dirs += ${DEBUGDIR}
bmake_install_dirs += ${DEBUGDIR}/bin
bmake_install_dirs += ${DEBUGDIR}/lib
bmake_install_dirs += ${LINTLIBDIR}
bmake_install_dirs += ${SHAREDIR}/doc/html
bmake_install_dirs += ${SHAREDIR}/doc/yajl
bmake_install_dirs += ${SHAREDIR}/man

beforeinstall: _bmake_install_dirs

# many BSD system mk files will not make directories on demand
_bmake_install_dirs: .PHONY
.for instdir in ${bmake_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# this creates 'html', 'latex-, and 'man' sub-directories with generated
# documentation
#
# xxx as-is this will always be executed since there are no real sources or targets
#
DOXYGEN ?=	doxygen
docs: .PHONY
	${DOXYGEN} src/YAJL.dxy

# xxx you can uncomment this if you have 'doxygen' installed and can build docs
#afterinstall: .PHONY install-docs

install-docs: .PHONY beforeinstall .WAIT docs
	cp -R $(MAKEOBJDIRPREFIX)/html ${DESTDIR}${SHAREDIR}/doc/
	cp README COPYING ChangeLog TODO ${DESTDIR}${SHAREDIR}/doc/yajl/
	cp -R $(MAKEOBJDIRPREFIX)/latex ${DESTDIR}${SHAREDIR}/doc/yajl/
	cp -R $(MAKEOBJDIRPREFIX)/man ${DESTDIR}${SHAREDIR}/

.include <bsd.subdir.mk>
