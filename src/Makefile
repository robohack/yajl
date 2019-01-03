# -*-makefile-bsdmake-*-

bmake_topdir = ..

# this must be set first to avoid the top-level LDADD
LIB =	yajl

.include "${.CURDIR}/${bmake_topdir}/Makefile.inc"

.include <bsd.own.mk>

# If these are set blank then no shared library is built, and so on OSX even
# Pkgsrc bmake can then complete a successful build.
#
SHLIB_MAJOR ?=   ${YAJL_MAJOR}
SHLIB_MINOR ?=   ${YAJL_MINOR}
SHLIB_TEENY ?=   ${YAJL_MICRO}

#WARNS =	3

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
	sed -e 's/@YAJL_MAJOR@/${YAJL_MAJOR}/' \
	    -e 's/@YAJL_MINOR@/${YAJL_MINOR}/' \
	    -e 's/@YAJL_MICRO@/${YAJL_MICRO}/' \
	    -e 's|@PREFIX@|${PREFIX}|' \
	    -e 's|@INCSDIR@|${INCSDIR}|' \
	    -e 's|@LIBDIR@|${LIBDIR}|' \
	< ${.CURDIR}/yajl.pc.in > ${.TARGET}

yajl.pc: ${.CURDIR}/Makefile
yajl.pc: ${.CURDIR}/${bmake_topdir}/Makefile.inc

# We have to do this here, not in the sub-directory, because during "make
# depend" the local actions are done before descending into the sub-directory
# (since it's not normal for a directory with products to also have
# sub-directories).
#
# N.B.:  The true BSD way to do this would be with a "make includes" step and to
# not only build this file, but to also install all of the other include files
# as well during that step, and then we could set '-I' to point to the
# (intermediate) "include" directory.  In a project like this we should probably
# "make includes" into the build tree, not into the OBJDIR, not into the final
# INCSDIR since we cannot have DESTDIR set when building.
#
beforedepend: ${.OBJDIR}/yajl yajl/yajl_version.h
yajl/yajl_version.h: ${.CURDIR}/yajl/yajl_version.h.in
	sed -e 's/$${YAJL_MAJOR}/${YAJL_MAJOR}/' \
	    -e 's/$${YAJL_MINOR}/${YAJL_MINOR}/' \
	    -e 's/$${YAJL_MICRO}/${YAJL_MICRO}/' \
	< ${.CURDIR}/yajl/yajl_version.h.in > ${.TARGET}

yajl/yajl_version.h: ${.CURDIR}/Makefile
yajl/yajl_version.h: ${.CURDIR}/${bmake_topdir}/Makefile.inc

${.OBJDIR}/yajl:
	mkdir ${.TARGET}

# Handling ${.OBJDIR} both with and without use of ${MAKEOBJDIRPREFIX} is
# tedious.
#
.if !exists(${.OBJDIR}/${bmake_topdir}/src)
FILES += yajl/yajl_version.h
# n.b.:  FILESDIR_blah won't work because of the "yajl/" directory
FILESDIR = ${INCSDIR}
.endif

CLEANFILES += yajl/yajl_version.h
# note:  cleandir doesn't remove local ${.OBJDIR} so we also won't worry about
# removing the "yajl" sub-directory

# This .if is annoying, but some older systems don't support CPPFLAGS.
#
# Note:  "." (in the build directory) is needed to find yajl/yajl_version.h
#
.if defined(CPPFLAGS)
CPPFLAGS += -I.
.else
CFLAGS += -I.
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
afterdepend: _bmake_static_lib_build_symlink
afterinstall: _bmake_static_lib_install_symlink

_bmake_static_lib_build_symlink:	.PHONY
	ln -fs lib$(LIB).a lib$(LIB)_s.a

CLEANFILES += lib$(LIB)_s.a

_bmake_static_lib_install_symlink: .PHONY
	ln -fs lib$(LIB).a ${DESTDIR}${LIBDIR}/lib$(LIB)_s.a

# As mentioned above it is not (currently) normal for a BSD Make system to allow
# a sub-directory in a library source directory, but for now it seems we can
# trick known systems into allowing it to work.  The problem is though that some
# targets are disabled for sub-directories because the are defined for the local
# directory, thus why we build yajl/yajl_version.h above.
#
# Ideally we would move yajl to ../include and force the "includes" target to be
# run for "beforedepend", installing all headers into a "yajl" sub-directory of
# the build directory.
#
SUBDIR += yajl

.include <bsd.lib.mk>
.if !defined(DISTRIBUTION)	# avoid dup include on some systems
.include <bsd.subdir.mk>
.endif

# set compiler and linker flags, especially additional warnings
#
.include "${.CURDIR}/${bmake_topdir}/Makefile.compiler"

showenv: .PHONY
	@echo ""
	@echo "Vars:"
	@echo "_BSD_OWN_MK_ = ${_BSD_OWN_MK_:Q}"
	@echo "DEBUGDIR = ${DEBUGDIR:Q}"
	@echo "LINTLIBDIR = ${LINTLIBDIR:Q}"
	@echo "MAKECONF = ${MAKECONF:Q}"
	@echo "MKDEBUG = ${MKDEBUG:Q}"
	@echo "MKDEBUGLIB = ${MKDEBUGLIB:Q}"
	@echo "MKDYNAMICROOT = ${MKDYNAMICROOT:Q}"
	@echo "MKLINT = ${MKLINT:Q}"
	@echo ""
	@echo "Environment:"
	env | sort
