# -*- makefile-bsdmake -*-

LIB =	${PACKAGE:S/^lib//}

# If these are set blank then no shared library is built
SHLIB_MAJOR ?=   ${LIB_MAJOR}
SHLIB_MINOR ?=   ${LIB_MINOR}
SHLIB_TEENY ?=   ${LIB_MICRO}

SRCS +=	yajl.c
SRCS +=	yajl_alloc.c
SRCS +=	yajl_buf.c
SRCS +=	yajl_encode.c
SRCS +=	yajl_gen.c
SRCS +=	yajl_lex.c
SRCS +=	yajl_parser.c
SRCS +=	yajl_tree.c
SRCS +=	yajl_version.c

FILES +=		yajl.pc
FILESDIR_yajl.pc =	${PKGCONFIGDIR}
FILESBUILD_yajl.pc =	yes
yajl.pc: ${.CURDIR}/yajl.pc.in
	sed -e 's/@FULLNAME@/${PACKAGE}/' \
	    -e 's/@LIB@/${LIB}/' \
	    -e 's/@LIB_MAJOR@/${LIB_MAJOR}/' \
	    -e 's/@LIB_MINOR@/${LIB_MINOR}/' \
	    -e 's/@LIB_MICRO@/${LIB_MICRO}/' \
	    -e 's|@PREFIX@|${PREFIX}|' \
	    -e 's|@INCSDIR@|${INCSDIR}|' \
	    -e 's|@LIBDIR@|${LIBDIR}|' \
	< ${.CURDIR}/yajl.pc.in > ${.TARGET}

yajl.pc: ${.CURDIR}/Makefile
yajl.pc: ${.CURDIR}/${bmake_topdir}/Makefile.inc

# FILES are not't automatically added on OSX/Darwin, FreeBSD, etc.
#
CLEANFILES += yajl.pc

# We have to do this here, not in the sub-directory, because during "make
# depend" the local actions are done before descending into the sub-directory
# (since it's not normal for a directory with products to also have
# sub-directories).
#
# N.B.:  The true BSD way to do this would be with a "make includes" step before
# building and to not only build this file, but to also install all of the other
# include files as well during that step, and then we could set '-I' to point to
# the (intermediate) "include" directory, e.g. ${.OBJDIR}/include.  Doing so now
# though would complicate the trick of getting the includes into their final
# directory (${INCDIR}) during "make install".
#
# (yajl.pc is here too because some versions of pkgsrc bootstrap-mk-files don't
# build all ${FILES} with the default "all" target)
#
beforedepend: yajl/yajl_version.h yajl.pc

yajl/yajl_version.h: ${.CURDIR}/yajl/yajl_version.h.in
	sed -e 's/$${LIB_MAJOR}/${LIB_MAJOR}/' \
	    -e 's/$${LIB_MINOR}/${LIB_MINOR}/' \
	    -e 's/$${LIB_MICRO}/${LIB_MICRO}/' \
	< ${.CURDIR}/yajl/yajl_version.h.in > ${.TARGET}

yajl/yajl_version.h: ${.CURDIR}/Makefile
yajl/yajl_version.h: ${.CURDIR}/${bmake_topdir}/Makefile.inc

# n.b.:  some variants of "make install" may (re)move ${FILES}, so a second
# "make" after an install will fail!
#
yajl_version.o: yajl/yajl_version.h

# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
# XXX ??? just use this: . if !defined(MAKEOBJDIRPREFIX)
.if !exists(${.OBJDIR}/${bmake_topdir}/src)
FILES += yajl/yajl_version.h
# n.b.:  FILESDIR_blah won't work because of the "yajl/" directory
FILESDIR = ${INCSDIR}
.endif

CLEANFILES += yajl/yajl_version.h
# note:  cleandir doesn't remove local ${.OBJDIR} so we also won't worry about
# removing the "yajl" sub-directory either.

# This .if is annoying, but some older systems don't support CPPFLAGS.
#
# XXX does this work if done before the .include <bsd.lib.mk> below?
#
# Note:  this needed in the build directory to find "yajl/yajl_version.h"
#
# XXX ideally this should just be for yajl_version.o, but support for
# target-specific CPPFLAGS is not universally implemented in BSD Makes; though
# if we moved all include files to be products copied to ${.OBJDIR}/include (for
# example) then this would be needed for everything but would already be
# supplied in the right form by ../Makefile.inc.
#
# See ../Makefile.inc for GENHDIR
#
.if defined(CPPFLAGS)
CPPFLAGS += -I${GENHDIR}
.else
CFLAGS += -I${GENHDIR}
.endif

# here we do a little trick to pre-create a symlink to a unique name for the
# static library so that on systems which don't allow full static linking, but
# where static linking to some libraries is desired, these unique static names
# can be selected.  This is also done in building the companion binaries for
# this project in order to ensure they can be tested without requiring an
# annoying configuration dance with the dynamic runtime linker.
#
# Note:  This is an example of BSD Make's need for phony targets with no action
# rules being used to trigger phony dependencies with action rules.  If the
# intermediate dependency had action rules, then it would become a target, phony
# or not.
#
# Note also this makes the link before the library may exist.
#
afterdepend: _bmake_static_lib_build_symlink

_bmake_static_lib_build_symlink:	.PHONY
	ln -fs lib$(LIB).a lib$(LIB)_s.a

CLEANFILES += lib$(LIB)_s.a

afterinstall: _bmake_static_lib_install_symlink

_bmake_static_lib_install_symlink: .PHONY
	ln -fs lib$(LIB).a ${DESTDIR}${LIBDIR}/lib$(LIB)_s.a

# As mentioned above it is not (currently) normal for a BSD Make system to allow
# a sub-directory in a library source directory, but for now it seems we can
# trick known systems into allowing it to work.  The problem is though that some
# targets are disabled for sub-directories because the are defined for the local
# directory, thus why we build yajl/yajl_version.h above.
#
# Ideally we would move yajl to ../include and force the "includes" target to be
# run for "beforedepend", installing all headers into a "include/yajl"
# sub-directory of ${.OBJDIR}.
#
SUBDIR += yajl

.include <bsd.lib.mk>
# avoid a duplicate include on some systems, probably only NetBSD and pkgsrc
# bootstrap-mk-files need this....
.if defined(_BSD_OWN_MK_) && ${_BSD_OWN_MK_} == 1
. include <bsd.subdir.mk>
.endif

# xxx hmmmm.... weird!  Avoid a "don't know how to make incinstall" error with
# Bmake-20181221 and newer (and maybe some older?  was ok in mk-20160401):
#
# (this target is defined in <bsd.inc.mk> (on at least NetBSD and DragonflyBSD,
# and in sjg's Mk-files) which is included in <bsd.lib.mk> and others)
#
.if !target(incinstall)
incinstall: .PHONY
.endif

# Note:  TARGET_OSNAME is from sjg's Mk-files (host-target.mk, own.mk), and
# OS is from pkgsrc bootstrap-mk.
#
.if ((defined(TARGET_OSNAME) && (${TARGET_OSNAME} == "Linux")) || \
	(defined(OS) && (${OS} == "Linux")))
#
# XXX stupid GNU BinUtils LD changed its command line syntax recently,
# apparently without concern for backward compatability.
#
# N.B.:  also apparently on Linux BMake currently (20200710 on ubuntu-latest,
# 20181221 on ubuntu-20.04) doesn't use SHLIB_SHFLAGS for the final command, but
# instead LD_shared, and for non-BSD platforms the value of LD_shared directly
# overridden without consulting SHLIB_SHFLAGS.
#
# So, override LD_shared to use more portable '-h' instead of the apparently now
# dead '-soname' (we could use '--soname', but '-h' is probably more canonical).
#
LD_shared=-shared -Wl,-h,lib${LIB}.so.${SHLIB_MAJOR}
.endif

# Some hacks to try to get a lib*.dylib file built and installed on macos....
#
# We'll still link our own programs and tests with the static library as that
# makes testing during build possible, but providing a dylib might be useful for
# other uses and may make packager's jobs easier....
#
.if ${MAKE} != "bsdmake" && \
	((defined(TARGET_OSNAME) && (${TARGET_OSNAME} == "Darwin")) || \
	 (defined(OS) && (${OS} == "Darwin")) || \
	 (defined(.FreeBSD) && (${.FreeBSD} == "false")))
#
# but we only need to do this for non-sjg (or really old sgj) Mk-files!
#
. if !defined(LD_solink) || (${LD_solink} != "dylib")
OBJECT_FMT = Mach-O		# XXX not really of much use....
# we should avoid :U for older bmakes, but this breaks something
.  if !defined(SHLIB_MINOR)
SHLIB_MINOR = 0
.  endif
.  if !defined(SHLIB_TEENY)
SHLIB_TEENY = 0
.  endif
.  if ${MAKE} == "bsdmake"
SHLIB_FULLVERSION ?= ${SHLIB_MAJOR}
.  else
SHLIB_FULLVERSION ?= ${SHLIB_MAJOR}.${SHLIB_MINOR}.${SHLIB_TEENY}
.  endif
SHLIB_INSTALL_VERSION ?= ${SHLIB_MAJOR}
# hmmm..... should compat be just SHLIB_MAJOR? (not according to sjg)
SHLIB_COMPATABILITY_VERSION ?= ${SHLIB_MAJOR}.${SHLIB_MINOR}
# XXX do we need "-install_name=${PREFIX}/lib/lib${LIB}.dylib" too?
# xxx -dylib_*_version might be too new for macos-10.x
SHLIB_SHFLAGS=-dylib \
	-dylib_current_version=${SHLIB_FULLVERSION} \
	-dylib_compatibility_version=${SHLIB_COMPATABILITY_VERSION}

# xxx maybe this link should point at just lib${LIB}.so ???
lib${LIB}.dylib: lib${LIB}.so.${SHLIB_FULLVERSION}
	@echo fixing up shared library name for macos: ${.TARGET}
	ln -sf lib${LIB}.so.${SHLIB_FULLVERSION} ${.TARGET}

#lib${LIB}.${SHLIB_INSTALL_VERSION}.dylib: lib${LIB}.so.${SHLIB_FULLVERSION}
#	@echo fixing up shared library name for macos: ${.TARGET}
#	ln -sf lib${LIB}.so.${SHLIB_FULLVERSION} ${.TARGET}

all: lib${LIB}.dylib

afterinstall: ${DESTDIR}${LIBDIR}/lib${LIB}.dylib

${DESTDIR}${LIBDIR}/lib${LIB}.dylib:
	ln -sf lib${LIB}.so.${SHLIB_FULLVERSION} \
		${DESTDIR}${LIBDIR}/lib${LIB}.dylib.tmp
	mv -f ${DESTDIR}${LIBDIR}/lib${LIB}.dylib.tmp \
		${DESTDIR}${LIBDIR}/lib${LIB}.dylib

. endif # !sjg
.endif

# note to do a local rebuild you can also do:  MAKEOBJDIRPREFIX=../build make
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "cd .. && mkdir -p build && MAKEOBJDIRPREFIX=$(pwd -P)/build " (default-value 'compile-command))
# End:
