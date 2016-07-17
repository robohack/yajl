# -*-makefile-bsdmake-*-

#
# This makefile works with NetBSD Make, OSX bsdmake, Pkgsrc bmake
# (except on OSX), and Simon Gerraty's Bmake & Mk-files.
#

SUBDIR =	src

SUBDIR +=	reformatter
SUBDIR +=	verify
#SUBDIR +=	example

#SUBDIR +=	test
#SUBDIR +=	perf

.ORDER: ${SUBDIR}

# Note:  $MAKEOBJDIR does not work with OSX bsdmake unless you have a
# /usr/obj in which it can be created (i.e. the "/usr/obj" prefix is
# mandatory), and there is no support at all for local obj or
# obj.${MACHINE} directories.  Just use $MAKEOBJDIRPREFIX to build
# somewhere entirely outside of the project source tree:
#
#	 "env MAKEOBJDIRPREFIX=/path/somewhere bsdmake"

#
# The rest is just default boilerplate for stand-alone builds....
#

BUILDTARGETS = do-obj do-includes do-depend

# this must be the first target
#
all: .PHONY .MAKE ${BUILDTARGETS}

.ORDER:		${BUILDTARGETS}

.for targ in ${BUILDTARGETS}
${targ}: .PHONY ${targ:S/do-//}
.endfor

do-includes: do-obj
do-depend: do-includes
all: do-depend

.include <bsd.subdir.mk>
