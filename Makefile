# -*- makefile-bsdmake -*-

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
# Then if the build succeeds (and assuming you're not cross-compiling) you can
# run the regression tests to see if the results are correct.
#
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build regress
#
# However using MAKEOBJDIRPREFIX more or less implies always invoking the build
# at the top of the source tree, with MAKEOBJDIRPREFIX set either on the
# command-line or in the environment).
#
# If you want to run make in just a sub-directory of the source tree AFTER
# you've done an initial build then you can do so provided you always carefully
# set MAKEOBJDIRPREFIX to the the fully qualified pathname of the initial build
# directory you've initially created.
#
# You can create the initial build directory skeleton infrastructure by directly
# building the "obj" and "depend" targets:
#
#	mkdir build
#	bsdmake MAKEOBJDIRPREFIX=$(pwd)/build obj depend
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
# N.B.:  Do not specify DESTDIR for the build or regress targets!

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
# the DEBUGDIR ones could/should maybe depend on MKDEBUGLIB
bmake_install_dirs += ${DEBUGDIR}
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/bin
bmake_install_dirs += ${DEBUGDIR}/${PREFIX}/lib
# LINTLIBDIR could depend on MKLINT
bmake_install_dirs += ${LINTLIBDIR}
# XXX at the moment, without Doxygen, we won't really need these...
bmake_install_dirs += ${DOCDIR}/yajl/html
bmake_install_dirs += ${DOCDIR}/yajl/latex
bmake_install_dirs += ${MANDIR}
# (in general though it is safest to always make them all)

beforeinstall: _bmake_install_dirs

# many BSD system mk files will not make directories on demand
_bmake_install_dirs: .PHONY
.for instdir in ${bmake_install_dirs}
	${INSTALL} -d ${DESTDIR}${instdir}
.endfor

# If you have "doxygen" installed then this creates 'html', 'latex-, and 'man'
# sub-directories with generated documentation.
#
# XXX with different versions of BSDMake we end up needing ${.CURDIR} or similar
# in the environment but we're safest to do that explicitly on the command line,
# and also some (older) versions of doxygen don't allow "." in environment
# variable names, so we have to be careful how we do it.
#
DOXYGEN ?=	doxygen
docs: .PHONY
	env MAKEOBJDIRPREFIX=$(MAKEOBJDIRPREFIX:Q) CURDIR=${.CURDIR:Q} ${DOXYGEN} ${.CURDIR:Q}/src/YAJL.dxy

# xxx you can uncomment this if you have 'doxygen' installed and can build docs
#afterinstall: .PHONY install-docs

install-docs: .PHONY beforeinstall .WAIT docs
	cp ${.CURDIR:Q}/README ${.CURDIR:Q}/COPYING ${.CURDIR:Q}/ChangeLog ${.CURDIR:Q}/TODO ${DESTDIR}${SHAREDIR}/doc/yajl/
	cp -R $(MAKEOBJDIRPREFIX:Q)/html ${DESTDIR}${SHAREDIR}/doc/yajl/
	cp -R $(MAKEOBJDIRPREFIX:Q)/latex ${DESTDIR}${SHAREDIR}/doc/yajl/
	cp -R $(MAKEOBJDIRPREFIX:Q)/man ${DESTDIR}${SHAREDIR}/

.include <bsd.subdir.mk>


#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "mkdir -p build; " (default-value 'compile-command) " MAKEOBJDIRPREFIX=$(pwd)/build")
# End:
#
