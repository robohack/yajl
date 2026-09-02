# -*- makefile-bsdmake -*-

#	Makefile - input for BSD Make
#
# This file is used by projects being built outside of a BSD source tree.
#
# The canonical source for this file is in https://github.com/robohack/yajl
#
# Copyright (C) 2026 Greg A. Woods - This work is licensed under the Creative
# Commons Attribution-ShareAlike 4.0 International License.  To view a copy of
# the license, visit <URL:http://creativecommons.org/licenses/by-sa/4.0/>, or
# send a letter to:  Creative Commons, PO Box 1866, Mountain View, CA 94042, USA
#

# This Makefile (and its associated include files) works with NetBSD Make, and
# Simon Gerraty's (sjg's) latest BMake from http://www.crufty.net/FreeWare/
# (with some caveats), and with FreeBSD make (both the old one, and the
# newer-since-14.0 BMake based one).  For many other systems the BMake included
# in pkgsrc will also work (see https://pkgsrc.org/).  This Makefile has not yet
# been tested on OpenBSD.
#
# See:  http://www.crufty.net/ftp/pub/sjg/help/bmake.htm
#
# Pkgsrc will install on a vast number of systems, including MS-Windows with
# Cygwin.  Similarly Simon's BMake works on most any Unix or Unix-like system.

# N.B.:  The main rules for this project are in Makefile.main
#
# You can easily use this and the related Makefile sections and associated
# include files to wrap any BSD Makefile and use the result to build a simple
# project outside of the main BSD source tree, e.g. as an add-on package,
# perhaps on a non-BSD machine, using Simon's Bmake and Mk-files.  Simply rename
# the original Makefile to Makefile.main, then copy this file, Makefile.inc,
# Makefile.compiler, and Makefile.end to your project.  If your code is portable
# enough then no "configure" step will be necessary!  Simple system dependencies
# can be managed with an optional Makefile.${.MAKE.OS}, etc.

# BUILD:
#
#	mkdir -p build/$(pwd); MAKEOBJDIRPREFIX=$(pwd)/build b(sd)make all
#
#
# INSTALL:
#
#	MAKEOBJDIRPREFIX=$(pwd)/build b(sd)make DESTDIR=/usr/local install
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it does not get in the way of pkgsrc either.)
#
# N.B.:  Do not specify DESTDIR for the build phase!
#
#
# HELP:
#
#	b(sd)make help
#
#
# (where "b(sd)make" is the native BSD Make, or is BMake)
#
# Notes:
#
# You should use $MAKEOBJDIRPREFIX, set in the environment, so as to build
# everything elsewhere outside of, or within a single sub-directory, of the
# source tree (i.e. instead of polluting the source itself tree with "obj"
# sub-directories everywhere).  Make sure to create the initial object directory
# -- make must be able to chdir to the initial object directory under
# MAKEOBJDIRPREFIX (i.e. with the current canonical (physical) PWD appended)
# else it will ignore it entirely.
#
#	BUILD_DIR=build-$(uname -s)-$(uname -p)-$(uname -r)
#	mkdir ${BUILD_DIR}/$(pwd -P)
#	export MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR}
#	b(sd)make all
#
# You may change the final installation heriarchy from the default of "/usr" to
# any path prefix of your choice by setting PREFIX on the make command lines,
# like this (or optionally on the command line, but remember it for the install
# step too!):
#
#	export PREFIX=/opt/pkg
#
# Then if the build succeeds (and assuming you're not cross-compiling) you can
# run the regression tests to see if the results are correct.
#
#	b(sd)make regress
#
# Finally to install the results into a "dist" subtree (which you can then
# distribute as a binary distribution that can be un-packed in the root of a
# target system) you can do:
#
#	b(sd)make DESTDIR=$(pwd -P)/dist install
#	cd dist && tar -cf ../dist.tar .
#
# DESTDIR can of course be any directory, e.g. /usr/local, especially if PREFIX
# is set to an empty string (PREFIX=""), but note the design is such that the
# package can be installed at build time into a private DESTDIR (as above), then
# archived and the resulting archive can be extracted at the root of the target
# system's filesystem.  The default with PREFIX="/usr" (without DESTDIR) will
# install into the base of a typical unix filesystem, while something like
# PREFIX="/usr/pkg" will install into a typical package installation directory.
# Setting PREFIX="/usr/local" along with DESTDIR=$(pwd -P)/dist will install
# into the named DESTDIR and allow those files to be archived on the build
# system and then extracted (at /) on any suitable target system where they will
# end up in /usr/local.
#
# (This is not the normal use of DESTDIR in BSD Make, but it is the best way for
# out-of-tree builds, and it matches the way pkgsrc now works internally.)
#
# WARNING:  Do not specify DESTDIR for the main build nor the regress target!
#
# Some comments on default settings, mostly for the purposes of "make help":
#
# Note:  If your platform does have libwrap (and tcpd.h), but they're not in a
# system directory searched by default then you can pass appropriate -I and -L
# flags by setting CPPFLAGS and LDFLAGS in the environment.
#
CPPFLAGS ?=	# Additional preprocessor flags, e.g. -I/usr/local/include (in env!)
LDFLAGS ?=	# Additional linker flags, e.g. -I/usr/local/lib (in env!)
#
# N.B.:  You CANNOT set make variables on the command line if they must be
# adjusted, e.g. ap|pre-pended to, within a Makefile -- they can only be set in
# the environment!
#
# Variables set on the command line are effectively always set last, after all
# Makefiles have been read and processed.  Variables set in the environment are
# set first, before any Makefiles have been read.  Makefiles use "?=" to provide
# defaults for variables that can be customised in environment variables.
#
#####################
#
# Special Notes for Special Systems:
#
# N.B.:  Some variants of BSD Make treat $MAKEOBJDIR as a sub-directory under
# /usr/obj, and others treat it as a sub-directory under ${.CURDIR}, even if it
# starts with a '/'!  You have been warned.  As with $MAKEOBJDIRPREFIX some
# older versions also only allow it to be set in the environment.  You should
# just use $MAKEOBJDIRPREFIX, set in the environment (except on OpenBSD since
# 5.5, where $MAKEOBJDIR is necessary, see below).
#
#
# NetBSD:
#
# On NetBSD you don't need to specify the "obj" or "depend" targets -- they are
# automatically always built.
#
# On NetBSD set LDSTATIC=-static to static-link the test and example programs.
#
# N.B.:  It is assumed that only NetBSD's Mk-files define _BSD_OWN_MK_ (and in
# particular it is only defined in <bsd.own.mk>, which is in general included by
# all the other Mk-files).
#
#
# MacOS vs. various BMakes:
#
# OSX, aka macOS, since the release of Xcode 10(?) doesn't have a working
# BSD make in the base system any longer.
#
# However the most recent version of BMake that can be installed from Homebrew
# does work, as does the version installed from pkgsrc.
#
# Manually installing sjg's BMake will also work, obviously.
#
#
# OpenBSD:
#
# So note OpenBSD's make since 5.5 (and before 2.1) does NOT support
# $MAKEOBJDIRPREFIX at all.  For recent OpenBSD, just use $MAKEOBJDIR instead.
# See below for how to do this.
#
#
# FreeBSD:
#
# Modern FreeBSD (since at least 14.x) with /usr/share/mk/auto.obj.mk needs only
# to have MAKEOBJDIRPREFIX set in the environment or on the command line.
#
# Older FreeBSD Mk-files don't work reliably, by default, without "obj" and
# "depend" being given explicitly on the command line.
#
# You don't even need to pre-create the build directory.
#
#	BUILD_DIR=build-$(uname -s)-$(uname -p)-$(uname -r)
#	export MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR}
#	export MK_AUTO_OBJ=yes
#	make all
#	make regress
#
# Note the separate "make regress" -- the dependency on "all" doesn't work.
# FreeBSD's SUBDIR and SUBDIR_TARGETS handling is sub-standard.
#
# Note on FreeBSD (at least since 14.0), static-linking requires setting
# NO_SHARED=yes in the environment.
#
# Installing sjg's BMake package will also work, obviously, but beware since
# about 14.x there's a /usr/bin/bmake link so make sure your PATH is what you
# want.
#
#
# Other BMake ports:
#
# These mostly work like NetBSD?
#
# See the further discussion in src/Makefile.inc.
#
#####################
#
# More about using $MAKEOBJDIRPREFIX:
#
# Using $MAKEOBJDIRPREFIX requires always invoking BSD Make with it set in the
# environment (or on the command line for newer BSD Makes).
#
# Because BSD Make tries to chdir immediately to the first object directory,
# i.e.  ${MAKEOBJDIRPREFIX}${.CURDIR}, aka $MAKEOBJDIRPREFIX/$(pwd -P) in shell
# terms, that directory must also exist prior to invoking make.
#
# While it is possible to craft things such that you can run make in just a
# sub-directory of the source tree AFTER you've done an initial full build (or
# at least after you've done an initial run of "make obj") this should probably
# be avoided -- always go to the root of the source tree and run the whole
# build.
#
# Note that setting $MAKEOBJDIRPREFIX in your shell's environment may risk
# mixing things up for different projects, though if your BSD Make does
# correctly set ${.CURDIR} to the canonical (physical) fully qualified current
# working directory where it was started from, and if you have set
# $MAKEOBJDIRPREFIX to a fully qualified pathname (possibly outside the source
# tree), then this could be a good way to share use of a fast scratch filesystem
# for builds of many different projects using BSD Makefiles.
#
# If you mess things up and end up with generated files in your source directory
# then run "make cleandir" to start over.  Note cleandir does not remove obj
# directories though.  See the 'find' below.
#
#####################
#
# How to do without $MAKEOBJDIRPREFIX:
#
# If you don't use $MAKEOBJDIRPREFIX then "obj.${MACHINE}" sub-directories will
# be created for each directory with products.  EXCEPT ON FreeBSD!!!  (where the
# default is always just "obj".
#
# If your BSD Make is new enough then this can be avoided by using MAKEOBJDIR
# (either as an environment variable or on the command line) as follows (which
# more or less works the way MAKEOBJDIRPREFIX does, but without burying all the
# object directories within another nested level of the current working
# directory path, thus making it better for in-tree build directories as opposed
# to some area shared by multiple projects):
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make
#
# (or in more simple terms:  export MAKEOBJDIR='${.CURDIR:S,${SRCTOP},${OBJTOP},}')
#
# (Note this way of setting MAKEOBJDIR is more or less necessary for any project
# with subdirectories, and it is derived from the related one in NetBSD's
# build.sh.)
#
# HOWEVER:  this won't work with older make's, including BMake until at least
# after 20200524, not even NetBSD-10, nor on FreeBSD, at least up to 14.x.  This
# is probably fixed first in NetBSD's Make as of 20230218.
#
# If you end up with "obj.*" sub-directories and you want to go back to using a
# 'build' directory (as would be sane to do) then you can remove all the obj.*
# detritus with this command (the trailing, escaped, semicolon is important!):
#
#	find . -type d -name .git -prune -o -type d ! -name .git ! -name 'obj*' -exec rm -rf {}/obj.\* \;
#
# XXX there really should be a way to remove all ${.OBJDIR} in <bsd.*.mk>!
#
#####################
#
# The history of MAKEOBJDIRPREFIX is a bit convoluted.
#
# The original BSD's "PMake" from CSRG (right up to 4.4BSD-Lite2) did not
# support MAKEOBJDIRPREFIX at all.  It did support MAKEOBJDIR, added 1990-03-19
# (usr.bin/make/main.c:5.13) by Keith Bostic as part of a slightly larger change
# with this part having the simple comment:
#
#    add object directory, as specified by MAKEOBJDIR/_PATH_OBJDIR, and CURDIR variable
#
# As far as I can tell that's the first time a BSD Make would chdir to an object
# directory before running any commands.  This is the origin of the practice of
# creating an "obj" directory in each source directory and putting all products
# there (by setting MAKEOBJDIR=obj in the environment).  Next came the magic
# "obj" targets in the various share/mk/*.mk files to create an object directory
# for each source directory in a mapped sub-directory of /usr/obj.  The initial
# version of these "obj" targets also forcibly removed the "obj" sub-directories
# in the source tree.  I'm not sure if/how they changed what MAKEOBJDIR would be
# set to for this to work (possibly as above).
#
# The first support for MAKEOBJDIRPREFIX appears to come into FreeBSD with
# r18339 (of usr.bin/make/main.c) by Steven Wallace on 1996-09-17.  This
# corresponds to about FreeBSD-2.2.1, I think.
#
# MAKEOBJDIRPREFIX first appeared in NetBSD usr.bin/make/main.c:1.31 when
# Christos Zoulas merged changes from both FreeBSD and Lite2, and clearly it
# must come from FreeBSD since it never appeared in 4.4BSD.
#
# It was fixed to actually be usable for the NetBSD system source in 2000-04-20
# by Simon Gerraty in usr.bin/make/main.c:1.56.
#
# However the initial implementation in FreeBSD's original make since 2.2.1 when
# it first gains support, and up to about May 2014, or in 9.0; and as merged in
# NetBSD's make since (effectively) 1.5 (literally since 1.3) and prior to 7.0
# (and so in Simon's BMake since its inception, and up to bmake-20140214); and
# in OpenBSD's make since 2.1 when they first gained support up until 5.5 (after
# which they removed all support for MAKEOBJDIRPREFIX!); were all extremely
# belligerent about having $MAKEOBJDIRPREFIX set in the environment and only in
# the environment -- they refused to even peek at it if it is only set on the
# command line, using only getenv(3) to access it.
#
# FreeBSD has switched to Simon's BMake by default in 10.0 (r250699 2013/05/16).
# This was optional since 2012.  The old FreeBSD Make was removed on 2015/06/16,
# prior to that it was optionally built as fmake since 2014/05/10.
#
# So in all but OpenBSD this has been fixed so that MAKEOBJDIRPREFIX can also be
# set on the command line.  I think.  I have had bad experiences with some
# versions (e.g. claiming to support setting it on the command line, but in fact
# not supporting that at all.  That's why the instructions above always show
# setting it in the environment.
#
#####################
#
#	Let's get started
#
# Note that normally a ../Makefile.inc is included via a <bsd.*.mk> file after
# the body of the main Makefile is read, but since we're outside of a BSD style
# source tree we need to include it here (in order to set BUILDTARGETS because
# it will be expanded immediately when the "all:" line below is read and
# parsed).
#
.include "${.CURDIR}/Makefile.inc"

# N.B.:  undo Makefile.inc's bmake_topdir (which is meant for subdirs!)
#
bmake_topdir =	.

# This ("all") must be the first target seen by make.
#
# Depending on "bmake-test-obj" is a workaround for versions of make which do
# not fully support MKOBJDIRS=auto (usually set with MK_AUTO_OBJ).
#
#.MAIN: all
all: .PHONY .MAKE bmake-test-obj .WAIT ${BUILDTARGETS}

#	Now, on with the show....
#
# include the "standard" Makefile
#
.include "${.CURDIR}/Makefile.main"

# N.B.:  re-undo Makefile.inc's bmake_topdir (which is meant for subdirs!)
#
bmake_topdir =	.

#	Final thoughts....
#
# This must be included after <bsd.prog.mk> or <bsd.lib.mk> or <bsd.subdir.mk>
# (one of which is normally included via Makefile.main)
#
.include "${.CURDIR}/Makefile.end"

# N.B.:  This setting of MAKEOBJDIR will also work for doing local builds in any
# sub-directory, even if it is expanded by a shell while in that sub-directory.
#
# On NetBSD MACHINE_ARCH is not a necessary distinction for userland programs...
#
# xxx there's no Make variable for "uname -r" so it need to be run by the shell.
#
#	MAKEOBJDIR='${.CURDIR:C,^'$(td=$(make -v bmake_topdir); cd $td; pwd -P)','$(td=$(make -v bmake_topdir); cd $td; pwd -P)'/build-${.MAKE.OS}-${MACHINE}-'$(uname -r)',}' make -j 8 LDSTATIC=-static
#
#	DESTDIR="dest-${BUILD_DIR#build-}"
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "BUILD_DIR=build-$(uname -s)-$(uname -m)-$(uname -r); cd $(make -v bmake_topdir) && mkdir -p ${BUILD_DIR}/$(pwd -P) && MAKEOBJDIRPREFIX=$(pwd -P)/${BUILD_DIR} " (default-value 'compile-command) " -j 8 LDSTATIC=-static")
# End:
#
