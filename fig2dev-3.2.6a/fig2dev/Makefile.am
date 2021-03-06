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

SUBDIRS = dev bitmaps
if ENABLE_I18N
SUBDIRS += i18n
endif
SUBDIRS += tests

EXTRA_DIST = fig2ps2tex.csh

# Copy the rgb.txt file into, usually, ../share/fig2dev/rgb.txt
## The pkgdata has its argument be put into pkgdatadir
dist_pkgdata_DATA = rgb.txt

## Need to use \"$(DESTDIR)$(pkgdatadir)? ... no
AM_CPPFLAGS = $(PATH_CPPFLAGS) -DRGB_FILE=\"$(pkgdatadir)/rgb.txt\"
AM_CFLAGS = -Wno-parentheses
AM_LDFLAGS = $(PATH_LDFLAGS)

bin_PROGRAMS = fig2dev

fig2dev_SOURCES = alloc.h arrow.c bool.h bound.h bound.c colors.h colors.c \
    creationdate.h creationdate.c drivers.h fig2dev.h fig2dev.c free.h free.c \
    iso2tex.c localmath.h localmath.c object.h psfonts.c read1_3.c pathmax.h \
    read.h read.c trans_spline.h trans_spline.c pi.h getopt.c

# CONFIG_HEADER is config.h, which contains PACKAGE_VERSION. If that
# changes, fig2dev should take up the new version string.
# other files also depend on PACKAGE_VERSION, see dev/Makefile.am and transfig
# config.h anyhow depends on all .m4-files
fig2dev.$(OBJEXT): $(CONFIG_HEADER)

## LIBOBJS may contain, e.g., strstr.o, since configure.ac contains
## AC_REPLACE_FUNCS(strstr) -- but usually LIBOBJS will be empty
fig2dev_LDADD = $(LIBOBJS) dev/libdrivers.a

dist_bin_SCRIPTS = fig2ps2tex pic2tpic

uninstall-hook:
	rmdir "$(DESTDIR)$(pkgdatadir)"
