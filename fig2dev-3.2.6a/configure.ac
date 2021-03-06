#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

#
# Fig2dev: Translate fig code to various devices.
# Parts Copyright (c) 2016 by Thomas Loimer
#
# Any party obtaining a copy of these files is granted, free of charge, a
# full and unrestricted irrevocable, world-wide, paid up, royalty-free,
# nonexclusive right and license to deal in this software and
# documentation files (the "Software"), including without limitation the
# rights to use, copy, modify, merge, publish and/or distribute copies of
# the Software, and to permit persons who receive copies from any such
# party to do so, with the only requirement being that this copyright
# notice remain intact.
#

#
#	configure.ac: Autoconf script
#	Author: Thomas Loimer <thomas.loimer@tuwien.ac.at>, 2016
#

dnl TODO: Implement FST

dnl Define m4 variables for use in AC_INIT and AC_DEFINE below.
dnl VERS=3.2 and AC_INIT(...,$VERS..) does not work. (These variables
dnl are probably only expanded when ./configure is run.) See also
dnl info automake, section 16 "Rebuilding Makefiles".
dnl defines m4-variables FIGFILEVERSION, FIGPATCHLEVEL and RELEASEDATE
m4_include([version.m4])

dnl The minimum autoconf version to process this file. In version 2.60,
dnl the directory variables changed, see info autoconf, section 4.8.3.
AC_PREREQ([2.60])

#
# Initialize.
#
AC_INIT([fig2dev], [FIG_VERSION], [thomas.loimer@tuwien.ac.at],[],
	[https://sourceforge.net/projects/mcj])
dnl AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS],
dnl	[TARNAME], [URL])
dnl TARNAME defaults to FULL-PACKAGE-NAME, converted to lower case
dnl and all characters other than alphanumerics and _ changed to -.
dnl defines PACKAGE_NAME, PACKAGE_TARNAME, PACKAGE_VERSION, PACKAGE_STRING,
dnl PACKAGE_BUGREPORT and PACKAGE_URL

# Set automake options.
dnl The options are written to AUTOMAKE_OPTIONS. A minimum version
dnl of automake, e.g., 1.6.1, can also be put into AM_INIT_AUTOMAKE.
AM_INIT_AUTOMAKE([-Wall foreign dist-xz no-dist-gzip subdir-objects])
dnl AM_INIT_AUTOMAKE([-Wall -Werror foreign])
dnl  -Wall.....enable all (automake) warnings
dnl  -Werror...and report them as error
dnl  foreign...do no require the files ChangeLog COPYING AUTHORS NEWS
dnl		README INSTALL
dnl  subdir-objects...place objects into the dir where the corresponding
dnl		source file resides, not into the build tree

AC_CONFIG_MACRO_DIR([m4])

# Add unusual search paths here.
# Macports by default installs under /opt/local, fink under /sw.
m4_define([LIBPATHS], [/opt/local/lib /sw/lib])
m4_define([INCLUDEPATHS], [/opt/local/include /sw/include])
m4_define([X_LIBPATHS], [LIBPATHS /opt/X11/lib ])
m4_define([X_INC_PATHS], [INCLUDEPATHS /opt/X11/include])

AC_SUBST([PATH_CPPFLAGS])
AC_SUBST([PATH_LDFLAGS])

# Check whether ./configure resides in the correct directory
AC_CONFIG_SRCDIR([CHANGES])

# Write the variables determined by ./configure into config.h.
# Therefore, make sure to #include "config.h" in each source file.
AC_CONFIG_HEADERS([config.h])

AC_SUBST([RELEASE_DATE], "RELEASEDATE")

dnl Use the following if individual, fairly independent projects are to be
dnl configured and built with separate configure.ac files from this master
dnl configure.ac. Order is important! From walktrough/chap6.pdf
dnl	AC_CONFIG_SUBDIRS([ftk flaim sql xflaim])

#
# Checks for programs.
#
AC_PROG_CC
# Use ISO Standard C, currently C99.
AC_PROG_CC_STDC
# Needed to pack the convenience library fig2dev/dev/fig2dev.a
AC_PROG_RANLIB
AM_PROG_AR

# Define LN_S for use in the Makefiles
AC_PROG_LN_S
# Define FGREP, for use in the testsuite
AC_PROG_FGREP

#
# Checks for libraries.
#
AC_SEARCH_LIBS([cos], [m])

#
# Checks for header files.
#

# Check for iconv.h, if found, try to compile and link a custom-made
# test program. Darwin needs -liconv, but <iconv> might typedef iconv()
# to libiconv(), therefore one can not simply search for the iconv
# symbol. Create a test program that #includes <iconv.h>, to provide the
# typedef, or not.
TL_CHECK_HEADER_PATH([iconv.h], [INCLUDEPATHS])
AS_IF([test "$tl_cv_header_path_iconv_h" != no],
    [fig2dev_save_LDFLAGS="$LDFLAGS"
     fig2dev_save_CPPFLAGS="$CPPFLAGS"
     CPPFLAGS="$CPPFLAGS $PATH_CPPFLAGS"
     TL_SEARCH_LIBS_PATH([iconv], [iconv], [LIBPATHS], [],
	[#include<iconv.h> int main(void)
	 {char *a; return iconv(0, &a, (size_t*)a, &a, (size_t*)a);}])dnl
     AS_IF([test "$tl_cv_libs_path_iconv" != no],
	[AC_DEFINE([HAVE_ICONV], 1,
	    [Define to 1 if you have the <iconv.h> header and libiconv.])])
     CPPFLAGS="$fig2dev_save_CPPFLAGS"
     LDFLAGS="$fig2dev_save_LDFLAGS"])dnl

# Check for headers, but do not define preprocessor-variables in config.h.
# If the headers below are missing, the program will not compile.
AS_VAR_SET([headers_missing], [no])
dnl AC_CHECK_HEADER(header.h,
dnl	[ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [INCLUDES])
dnl sets ac_cv_header_header_h
AC_CHECK_HEADER([limits.h], [], [headers_missing=yes
    AC_MSG_WARN(limits.h is missing!)], [AC_INCLUDES_DEFAULT])
AC_CHECK_HEADER([netinet/in.h], [], [headers_missing=yes
    AC_MSG_WARN(netinet/in.h is missing!)], [AC_INCLUDES_DEFAULT])
AC_CHECK_HEADER([sys/file.h], [], [headers_missing=yes
    AC_MSG_WARN(sys/file.h is missing!)], [AC_INCLUDES_DEFAULT])
AC_CHECK_HEADER([sys/param.h], [], [headers_missing=yes
    AC_MSG_WARN(sys/param.h is missing!)], [AC_INCLUDES_DEFAULT])
# AC_INCLUDES_DEFAULT anyhow requires stdio.h and checks for
# sys/types.h, sys/stat.h, stdlib.h, string.h, strings.h, inttypes.h,
# stdint.h, unistd.h, and optionally stddef.h and memory.h. The
# variables HAVE_SYS_TYPES_H, HAVE_SYS_STAT_H, HAVE_STDLIB_H etc. are
# defined in config.h.

# Some headers are checked implicitly by testing for functions provided
# by them: <pwd.h> by getpwuid(), <errno.h> by strerror(). Really,
# strerror() is not defined in errno.h, but it is save to assume that
# errno.h exists if strerror() is found.

#
# Check user options.
#
AC_ARG_ENABLE(versioning, [AS_HELP_STRING([--enable-versioning],
	[enable changing the version number, only useful for \
	 hacking (default: disable)])],
    [],[enableval=no])dnl
AM_CONDITIONAL([ENABLE_VERSIONING], [test "x$enableval" = xyes])dnl

AC_ARG_ENABLE(transfig, [AS_HELP_STRING([--enable-transfig],
	[build the transfig program (default: do not build)])],
    [],[enableval=no])dnl
AM_CONDITIONAL([ENABLE_TRANSFIG],[test "x$enableval" = xyes])

AC_ARG_ENABLE(letterpaper, [AS_HELP_STRING([--enable-letterpaper],
	[set default papersize to letter for dxf \
		and ibmgl-drivers (default: A4)])],
    [],[enableval=no])dnl
AS_IF([test "x$enableval" = xno || test "x$enableval" = xA4 || \
	test "x$enableval" = xa4],dnl
    [AC_DEFINE([A4], 1,dnl
	[Define to 1 if the default papersize for the dxf- \
		and ibmgl-drivers is A4, not letter.])dnl
# Used in fig2dev.1.in
     AC_SUBST(DEFAULT_PAPERSIZE, ["ISO A4"])dnl
     AC_SUBST(ALTERNATE_PAPERSIZE, ["ANSI A"])],dnl
    [AC_SUBST(DEFAULT_PAPERSIZE, ["ANSI A"])dnl
     AC_SUBST(ALTERNATE_PAPERSIZE, ["ISO A4"])])dnl

AC_ARG_ENABLE([i18n], [AS_HELP_STRING([--disable-i18n],
	[disable internationalization of text input \
		for some drivers (default: enable)])],
    [i18nman="/dev/null"],
    [enableval=yes; i18nman="$srcdir/man/i18n.man"])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([I18N], 1,dnl
	[Define to 1 to use internationalization of text input \
		for some drivers.])dnl
# The directory where internationalization data is installed to.
# fig2dev/dev/Makefile.am defines -DI18N_DATADIR
# i18ndir is used in fig2dev/i18n/Makefile.am
     AC_SUBST([i18ndir],['$(pkgdatadir)/i18n'])])
AM_CONDITIONAL([ENABLE_I18N],[test "x$enableval" = xyes])
dnl Substitute the file fragment i18n.man into man/fig1dev.1 (or not)
AC_SUBST_FILE([i18nman])

AC_ARG_ENABLE([NFSS], [AS_HELP_STRING([--disable-NFSS],
	[disable LaTeX New Font Selection Scheme (default: enable)])],
    [nfssman="/dev/null"],[enableval=yes; nfssman="$srcdir/man/nfss.man"])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([NFSS], 1,dnl
	 [Define to 1 to use the New Font Selection Scheme for LaTeX.])])dnl
AC_SUBST_FILE([nfssman])

AC_ARG_ENABLE([latex2e_graphicx], [AS_HELP_STRING([--disable-latex2e_graphicx],
	[disable LaTeX2e-graphics commands in pstex- \
		and latex-drivers (default: enable)])],
    [],[enableval=yes])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([LATEX2E_GRAPHICS], 1,dnl
	[Define to 1 to use LaTeX2e-graphics in pstex- \
		and latex-drivers.])])dnl

AC_ARG_ENABLE([scale-pict2e], [AS_HELP_STRING([--enable-scale-pic2t2e],
	[enable scaling of pict2e-graphics, e.g., \
		with \XFigwidth (default: disable)])],
    [],[enableval=no])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([SCALE_PICT2E], 1,dnl
	 [Define to 1 if pict2e-graphics should honor \
		\XFigwidth or \XFigheight.])])dnl

AC_ARG_ENABLE([IBM-GEC], [AS_HELP_STRING([--disable-IBM-GEC],
	[disable instruction set for IBM Graphics Enhancement \
		Cartridge (default: enable)])],
    [],[enableval=yes])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([IBMGEC], 1,dnl
	[Define to 1 if the ibmgl-driver should create instructions \
		for the IBM Graphics Enhancement Cartridge.])dnl
     AC_SUBST([WITH_IBMGEC],[without])],dnl
    [AC_SUBST([WITH_IBMGEC],[with])])dnl

AC_ARG_ENABLE([baseline-shift], [AS_HELP_STRING([--disable-baseline-shift],
	[disable use of baseline-shift in the svg-driver (default: enable)])],
    [],[enableval=yes])dnl
AS_IF([test "x$enableval" != xyes],dnl
    [AC_DEFINE([NOSUPER], 1,dnl
	 [Define to 1 to not use baseline-shift sub/superscripts \
		in the svg-driver])])dnl

AC_ARG_ENABLE([round-box], [AS_HELP_STRING([--disable-round-box],
	[disable use of boxes with rounded corners \
		in the tpic-driver (default: enable)])],
    [],[enableval=yes])dnl
AS_IF([test "x$enableval" = xyes],dnl
    [AC_DEFINE([TPIC_ARC_BOX], 1,dnl
	 [Define to 1 to use boxes with rounded corners \
		in the tpic-driver.])])dnl

fig2dev_save_LDFLAGS="$LDFLAGS"
# fig2dev_save_CPPFLAGS=$CPPFLAGS # not needed
AC_ARG_WITH(png,
    [AS_HELP_STRING([--without-png],
		[disable reading png-images (default: enable)])],
    [],[with_png=try]dnl
)
AS_IF([test "x$with_png" != xno],
    [# -lz is not necessary for shared libraries
     # AC_SEARCH_LIBS([deflate], [z])dnl
     TL_SEARCH_LIBS_PATH([png_read_info], [png], [LIBPATHS])dnl
     AS_IF([test "$tl_cv_libs_path_png_read_info" != no],
	[TL_CHECK_HEADER_PATH([png.h], [INCLUDEPATHS])
	 AS_IF([test "$tl_cv_header_path_png_h" != no],
	    [AC_DEFINE([HAVE_PNG_H], 1,
		[Define to 1 if you have the <png.h> header file.])])])]dnl
)
AM_CONDITIONAL([WITH_PNG], [test "$tl_cv_header_path_png_h" = yes])

# If with-xpm is true, search for the X11 include and library paths,
# then search for libXpm, then search for X11/xpm.h. If all are found,
# regardless whether --x-includes and --x-libraries are set on the
# commandline or not, use xpm.
# Note: Under cygwin, xpm may be provided by /lib/noX/libXpm,
# /usr/include/noX/xpm.h and /usr/include/noX/X11/xpm.h
# AC_PATH_X finds the location of the X header files and libraries. Sets
# x_libraries and x_includes to NONE, not the empty string, if none are
# found. Otherwise, sets x_includes to the parent of the X11 header
# dir, i.e., the headers must be included with, e.g, #include
# <X11/xpm.h>. A simple #include <xpm.h> would no work. Sets x_libraries
# to the library dir. The X library path is appended, not prepended to
# $LIBS. AC_PATH_XTRA is an extended version that sets additional
# variables.
AC_ARG_WITH(xpm, [AS_HELP_STRING([--without-xpm],
	[disable reading xpm-images (default: enable)])],
    [],[with_xpm=try])
AS_IF([test "x$with_xpm" != xno],
    [fig2dev_save_CPPFLAGS="$CPPFLAGS"
     CPPFLAGS="$CPPFLAGS $PATH_CPPFLAGS"
     AC_PATH_X
     AS_IF([test "x$no_x" != xyes],
	[#LDFLAGS="-L$x_libraries $LDFLAGS"
	 #PATH_LDFLAGS="-L$x_libraries $PATH_LDFLAGS"
	 AS_IF([test -n "$x_libraries"],
	    [AS_IF([test -n "$LDFLAGS"],
		[LDFLAGS="-L$x_libraries $LDFLAGS"],
		[LDFLAGS="-L$x_libraries"])
	     AS_IF([test -n "$PATH_LDFLAGS"],
		[PATH_LDFLAGS="-L$x_libraries $PATH_LDFLAGS"],
		[PATH_LDFLAGS="-L$x_libraries"])])])
     # (1) Add -lXpm to LIBS
     # (2) Check in X_LIBPATHS, if not found.
     TL_SEARCH_LIBS_PATH([XpmReadFileToXpmImage], [Xpm], [X_LIBPATHS])dnl
     AS_IF([test "$tl_cv_libs_path_XpmReadFileToXpmImage" != no],
	[# PATH_CPPFLAGS="$PATH_CPPFLAGS -I$x_includes"
	 AS_IF([test -n "$x_includes"],
	    [AS_IF([test -n "$PATH_CPPFLAGS"],
		[PATH_CPPFLAGS="$PATH_CPPFLAGS -I$x_includes"],
		[PATH_CPPFLAGS="-I$x_includes"])])
	 TL_CHECK_HEADER_PATH([X11/xpm.h], [X_INC_PATHS])
	 AS_IF([test "x$tl_cv_header_path_x11_xpm_h" != xno],
	    [AC_DEFINE([HAVE_X11_XPM_H], 1,
		    [Define to 1 if you have the <X11/xpm.h> header file.])])])
    CPPFLAGS="$fig2dev_save_CPPFLAGS"])
AM_CONDITIONAL([WITH_XPM], [test "x$ac_cv_header_X11_xpm_h" = xyes])

#
# Checks for typedefs, structures, and compiler characteristics.
#
AC_C_CONST
AC_C_INLINE
dnl to silence a warning on calling iconv() from genemf.c
dnl see the commit after 2b3711d
dnl AC_C_RESTRICT
# AC_C_BIGENDIAN([ACTION-IF-TRUE], [ACTION-IF-FALSE],
#	[ACTION-IF-UNKNOWN], [ACTION-IF-UNIVERSAL])
AC_C_BIGENDIAN

dnl  Check for declarations. In any case defines, e.g., HAVE_DECL_M_PI.
dnl  Sets it to 0 if not found, to 1 if found.
dnl AC_CHECK_DECLS([M_PI, M_PI_2, M_PI_4], [], [], [#include <math.h>])
dnl Just provide our own pi
dnl Old BSD had MAXPATHLEN in sys/param.h. See fig2dev.h.
dnl AC_CHECK_DECLS([PATH_MAX], [], [], [#include <limits.h>])
dnl Trust limits.h to provide PATH_MAX

# Check, whether sdtbool.h exists and whether the type _Bool is defined.
# Defines HAVE_STDBOOL_H and HAVE__BOOL. See info autoconf for a usage
# example.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

#
# Checks for library functions.
#
# Provide malloc.c in LIBOBJ_DIR below, otherwise AC_FUNC_MALLOC fails.
AC_FUNC_MALLOC
AC_FUNC_REALLOC

# Check for functions and set, e.g., HAVE_FLOOR if found.
# The code checks the results for gethostname, getpwuid, strerror or
# setmode and acts accordingly. The other functions _must_ exist!
# Assume that errno.h exists if strerror() is available. Otherwise, do
# not use strerror() at all. Do not try to replace strerror() with the
# obsolete sys_nerr, sys_errlist and the declaration "int errno".
# isascii only occurs in readxbm.c. isascii is considered obsolete.
# memmove: genps.c
# memset: read.c, genemf.c, genpstricks.c, readpcx.c
# genpstricks defines private is_alpha, to_lower, xstrdup, xmalloc
# strtol: fig2dev.c
# The setmode function is defined for windows in io.h, and accepts two
# arguments; The bsd-variant of setmode accepts one argument and is
# defined in bsd/unistd.h. The windows-variant sets the file to be read
# or written in O_BINARY or O_TEXT mode, which are defined in fcntl.h.
# Here, if setmode exists, it is assumed that io.h and fcntl.h exist and
# that O_BINARY is defined by fcntl.h.
# If getpwuid() is found, <pwd.h> is assumed to exist.
AC_CHECK_FUNCS_ONCE([gethostname strerror getpwuid _setmode])
dnl TODO:  Clean src from legacy code, replacing
dnl probably xmalloc, xstrdup, etc.

# Check for functions and, if not found, use the corresponding
# replacement, e.g., strstr.c, in the top srcdir. Sets, e.g.,
# HAVE_STRSTR. Any modern system should have these functions, so
# this are obsolete checks.
AC_REPLACE_FUNCS([isascii strstr strchr strrchr strcasecmp strncasecmp \
	strdup strndup])

LDFLAGS="$fig2dev_save_LDFLAGS"

# Place the replacement functions into this dir.
AC_CONFIG_LIBOBJ_DIR([fig2dev/lib])

# The directory were bitmaps are installed to.
# fig2dev/dev/Makefile.am defines -DBITMAPDIR and -DI18N_DATADIR
# bitmapsdir is used in fig2dev/bitmaps/Makefile.am.
# Bitmaps are used by the tk- and ptk-driver.
AC_SUBST([bitmapsdir],['$(pkgdatadir)/bitmaps'])

# For uninstall, we want to remove directories which were possibly
# created at install-time and may be empty at uninstall-time
# test rmdir --ignore-fail-on-non-empty

#
# Autotest infrastructure.
#
AC_CONFIG_TESTDIR([fig2dev/tests]) #, [TEST_PATH = $(srcdir)/fig2dev)])
AM_MISSING_PROG([AUTOM4TE], [autom4te])

# Create these files from the Makefile.in templates. The Makefile.in, in
# turn, is created from Makefile.am with automake or just autoreconf.
AC_CONFIG_FILES([Makefile
		 man/Makefile
		 man/fig2dev.1
		 fig2dev/Makefile
		 fig2dev/dev/Makefile
		 fig2dev/bitmaps/Makefile
		 fig2dev/i18n/Makefile
		 fig2dev/tests/Makefile
		 fig2dev/tests/atlocal
		 transfig/Makefile])

AC_OUTPUT

#
# Diagnostic output
#
AS_IF([test "$headers_missing" = yes],dnl
	[AC_MSG_WARN([headers are missing!
	*** The program will not compile.    ***
	*** Look for WARNING-messages above. ***])])
AC_MSG_NOTICE([Results:
  preprocessor		CPP:		$CPP
  preprocessor flags	CPPFLAGS:	${CPPFLAGS:-<empty>}
  compiler		CC:		$CC
  compiler flags	CFLAGS:		${CFLAGS:-<empty>}
  linker flags		LDFLAGS:	${LDFLAGS:-<empty>}
  libraries		LIBS:		${LIBS:-<empty>}
  install location	prefix:		$prefix

Files will be installed under $prefix.])
