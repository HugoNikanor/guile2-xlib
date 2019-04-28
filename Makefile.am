GUILE_CFLAGS  = @GUILE_CFLAGS@
GUILE_LIBS = @GUILE_LIBS@

## Process this file with Automake to create Makefile.in

ACLOCAL_AMFLAGS = -I m4

AUTOMAKE_OPTIONS = gnu

lib_LTLIBRARIES = libguilexlib.la

BUILT_SOURCES = guile-xlib.x
libguilexlib_la_SOURCES = guile-xlib.c $(BUILT_SOURCES)
libguilexlib_la_LDFLAGS = -version-info 0:0 -export-dynamic 
libguilexlib_la_CFLAGS = $(GUILE_CFLAGS)
libguilexlib_la_LIBADD = $(X_LIBS) $(X_PRE_LIBS) -lX11 $(X_EXTRA_LIBS) $(GUILE_LIBS)
libguilexlib_ladir = $(includedir)
libguilexlib_la_HEADERS = guile-xlib.h

scmdatadir = $(datadir)/guile/site/xlib
scmdata_DATA = xlib.scm

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = guile2-xlib.pc

EXTRA_DIST = $(scmdata_DATA) autogen.sh

## We assume the user has already installed Guile.
SUFFIXES = .x
SNARF = guile-snarf
.c.x:
	$(SNARF) -o $@ $(INCLUDES) $(GUILE_CFLAGS) $(CPPFLAGS) $<

info_TEXINFOS = guile-xlib.texi
guile_xlib_TEXINFOS = xlib.texi

## Add -MG to make the .x magic work with auto-dep code.
MKDEP = gcc -M -MG $(DEFS) $(INCLUDES) $(CPPFLAGS) $(GUILE_CFLAGS) $(CFLAGS)

CLEANFILES = *.x
