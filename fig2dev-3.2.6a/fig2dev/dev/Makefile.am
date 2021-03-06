## Run automake on this file to produce Makefile.in
# This file is part of fig2dev - Translate Fig code to various devices.
#
# Copyright (c) 2016 Thomas Loimer <thomas.loimer@tuwien.ac.at>
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.

MAINTAINERCLEANFILES = Makefile.in

## build a convenience library

## The default ar-flags are "cru", c .. create, r .. replace a member if found,
## u .. update, only replace a member if the object is newer
## Instead, use "cq", q .. quick, just append at the end
ARFLAGS = cq

## Use AM_PROG_AR and AC_PROG_RANLIB in configure.ac

noinst_LIBRARIES = libdrivers.a

## directory-wide CPPFLAGS;  -I../.. is set anyways
## if CPPFLAGS are set individually, like
##   libdrivers_a_CPPFLAGS = -I..
## objects like libdrivers_a-as85ec.o etc. are created
# i18ndir and bitmapsdir are defined and AC_SUBST'd in configure.ac
AM_CPPFLAGS = -I$(top_srcdir)/fig2dev $(PATH_CPPFLAGS) \
	      -DBITMAPDIR=\"$(bitmapsdir)\"
if ENABLE_I18N
AM_CPPFLAGS += -DI18N_DATADIR=\"$(i18ndir)\"
endif
AM_CFLAGS = -Wno-parentheses
## libraries ignore LDFLAGS - they need not to be linked
## AM_LDFLAGS = $(PATH_LDFLAGS)
## libdrivers_a_LIBADD is appended to the link-command

libdrivers_a_SOURCES = asc85ec.c genbitmaps.c genbox.c gencgm.c gendxf.c \
    genemf.h genemf.c genepic.c gengbx.c genge.c genibmgl.c genlatex.h \
    genlatex.c genmap.c genmf.c genmp.c genpdf.c genpic.c genpict2e.c \
    genpictex.c genps.c genps.h genpstex.c genpstricks.c genptk.c genshape.c \
    gensvg.c genvdx.c gentextyl.c gentikz.h gentikz.c gentk.c tpicfonts.h gentpic.c \
    picfonts.h picpsfonts.h psencode.h psencode.c psfonts.h psimage.h \
    psprolog.h readeps.c readgif.c readjpg.c readpcx.c readpics.c readppm.c \
    readtif.c readxbm.c texfonts.h setfigfont.h setfigfont.c

# These contain PACKAGE_VERSION, hence depend on $(CONFIG_HEADER) = config.h.
genps.o gensvg.o genvdx.o genemf.o gengbx.o genmp.o genpictex.o gentk.o: $(CONFIG_HEADER)

if WITH_PNG
libdrivers_a_SOURCES += readpng.c
endif

if WITH_XPM
libdrivers_a_SOURCES += readxpm.c
endif
