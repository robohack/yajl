# -*-makefile-bsdmake-*-

YAJL_MAJOR =	2
YAJL_MINOR =	1
YAJL_MICRO =	1

.if ${USER} != "root"
BINOWN =	${USER}
BINGRP !=	id -g
.endif

# N.B.:  ${DESTDIR} if set on the command line, is prepended, so to
# install natively set DESTDIR=/usr, or to install locally set
# DESTDIR=/usr/local, or DESTDIR=$HOME for a private install, etc.
#
BINDIR = /bin
LIBDIR = /lib
LINTLIBDIR = /libdata

.if make(buildincludes) || make(installincludes)
# n.b.:  this must always be just a relative path
INCLUDEDIR = yajl
.elif make(install)
INCSDIR = /include/yajl
INCLUDEDIR = /include/yajl
.else
# n.b.:  these must always be just a relative path
INCSDIR = yajl
INCLUDEDIR = yajl
.endif
