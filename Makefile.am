AM_CPPFLAGS = $(MBTERM_CFLAGS)
AM_CFLAGS = -Wall
LDADD = $(MBTERM_LIBS)

bin_PROGRAMS = gtkapp
gtkapp_SOURCES = main.c

desktopdir = $(datadir)/applications
dist_desktop_DATA = gtkapp.desktop
#dist_desktop_DATA = gtkapp.desktop
# make maintainer-clean should remove *everything* 
MAINTAINERCLEANFILES = \
	aclocal.m4 \
	compile \
	config.guess \
	config.h.in \
	configure \
	depcomp \
	install-sh \
	Makefile.in \
	missing
