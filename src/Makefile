# Makefile.in generated automatically by automake 1.5 from Makefile.am.

# Copyright 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001
# Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.



SHELL = /bin/sh

srcdir = .
top_srcdir = ..

prefix = /usr/local
exec_prefix = /usr/local

bindir = /usr/local/bin
sbindir = /usr/local/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = /usr/local/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = /usr/local/include
oldincludedir = /usr/include
pkgdatadir = $(datadir)/openhpi-subagent
pkglibdir = $(libdir)/openhpi-subagent
pkgincludedir = $(includedir)/openhpi-subagent
top_builddir = ..

ACLOCAL = ${SHELL} /home/konrad/hpi/openhpi-subagent/missing --run aclocal
AUTOCONF = ${SHELL} /home/konrad/hpi/openhpi-subagent/missing --run autoconf
AUTOMAKE = ${SHELL} /home/konrad/hpi/openhpi-subagent/missing --run automake
AUTOHEADER = ${SHELL} /home/konrad/hpi/openhpi-subagent/missing --run autoheader

INSTALL = /bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL}
INSTALL_HEADER = $(INSTALL_DATA)
transform = s,x,x,
NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
AMTAR = ${SHELL} /home/konrad/hpi/openhpi-subagent/missing --run tar
AWK = gawk
CC = gcc
DEPDIR = .deps
EXEEXT = 
INSTALL_STRIP_PROGRAM = ${SHELL} $(install_sh) -c -s
OBJEXT = o
PACKAGE = openhpi-subagent
VERSION = 0.1
am__include = include
am__quote = 
base_includedir = /usr/local/include
found_pkg_config = yes
install_sh = /home/konrad/hpi/openhpi-subagent/install-sh

MAINTAINERCLEANFILES = Makefile.in

INCLUDES = -I$(top_srcdir)/include 

AM_CFLAGS = -g -O2  -DINET6 -march=pentium3 -O3 -pipe -Dlinux -I. -I/usr/include -I/usr/local/include/openhpi -I/usr/include/glib-1.2 -I/usr/lib/glib/include   -Wall -Wmissing-prototypes 	-Wmissing-declarations 	-Wstrict-prototypes 	-Wpointer-arith 	-Wcast-qual -Wcast-align 

bin_PROGRAMS = hpiSubagent

hpiSubagent_SOURCES = saHpiHotSwapTable.c saHpiEventTable.c saHpiSystemEventLogTable.c saHpiWatchdogTable.c saHpiSensorTable.c saHpiInventoryTable.c saHpiCtrlTable.c hpiSubagent.c saHpiTable.c saHpiRdrTable.c alarm.c

hpiSubagent_LDADD = 
hpiSubagent_LDFLAGS = 
subdir = src
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = $(top_builddir)/config.h
CONFIG_CLEAN_FILES =
bin_PROGRAMS = hpiSubagent$(EXEEXT)
PROGRAMS = $(bin_PROGRAMS)

am_hpiSubagent_OBJECTS = saHpiHotSwapTable.$(OBJEXT) \
	saHpiEventTable.$(OBJEXT) saHpiSystemEventLogTable.$(OBJEXT) \
	saHpiWatchdogTable.$(OBJEXT) saHpiSensorTable.$(OBJEXT) \
	saHpiInventoryTable.$(OBJEXT) saHpiCtrlTable.$(OBJEXT) \
	hpiSubagent.$(OBJEXT) saHpiTable.$(OBJEXT) \
	saHpiRdrTable.$(OBJEXT) alarm.$(OBJEXT)
hpiSubagent_OBJECTS = $(am_hpiSubagent_OBJECTS)
hpiSubagent_DEPENDENCIES =

DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES =  -I. -I$(srcdir) -I$(top_builddir)
CPPFLAGS = 
LDFLAGS = -lglib
LIBS =  -L/usr/lib -lnetsnmpagent -lnetsnmpmibs -lnetsnmphelpers -lnetsnmp -L/usr/local/lib -lopenhpi   -lm
depcomp = $(SHELL) $(top_srcdir)/depcomp
DEP_FILES = $(DEPDIR)/alarm.Po $(DEPDIR)/hpiSubagent.Po \
	$(DEPDIR)/saHpiCtrlTable.Po \
	$(DEPDIR)/saHpiEventTable.Po \
	$(DEPDIR)/saHpiHotSwapTable.Po \
	$(DEPDIR)/saHpiInventoryTable.Po \
	$(DEPDIR)/saHpiRdrTable.Po \
	$(DEPDIR)/saHpiSensorTable.Po \
	$(DEPDIR)/saHpiSystemEventLogTable.Po \
	$(DEPDIR)/saHpiTable.Po \
	$(DEPDIR)/saHpiWatchdogTable.Po
COMPILE = $(CC) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(AM_CPPFLAGS) \
	$(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)
CCLD = $(CC)
LINK = $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(AM_LDFLAGS) $(LDFLAGS) -o $@
CFLAGS = -g -O2  -DINET6 -march=pentium3 -O3 -pipe -Dlinux -I. -I/usr/include -I/usr/local/include/openhpi -I/usr/include/glib-1.2 -I/usr/lib/glib/include   -Wall -Wmissing-prototypes 	-Wmissing-declarations 	-Wstrict-prototypes 	-Wpointer-arith 	-Wcast-qual -Wcast-align 
DIST_SOURCES = $(hpiSubagent_SOURCES)
DIST_COMMON = Makefile.am Makefile.in
SOURCES = $(hpiSubagent_SOURCES)

all: all-am

.SUFFIXES:
.SUFFIXES: .c .o .obj
$(srcdir)/Makefile.in:  Makefile.am  $(top_srcdir)/configure.ac $(ACLOCAL_M4)
	cd $(top_srcdir) && \
	  $(AUTOMAKE) --foreign  src/Makefile
Makefile:  $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) && \
	  CONFIG_HEADERS= CONFIG_LINKS= \
	  CONFIG_FILES=$(subdir)/$@ $(SHELL) ./config.status
install-binPROGRAMS: $(bin_PROGRAMS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  p1=`echo $$p|sed 's/$(EXEEXT)$$//'`; \
	  if test -f $$p \
	  ; then \
	    f=`echo $$p1|sed '$(transform);s/$$/$(EXEEXT)/'`; \
	   echo " $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$f"; \
	   $(INSTALL_PROGRAM_ENV) $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$f; \
	  else :; fi; \
	done

uninstall-binPROGRAMS:
	@$(NORMAL_UNINSTALL)
	@list='$(bin_PROGRAMS)'; for p in $$list; do \
	  f=`echo $$p|sed 's/$(EXEEXT)$$//;$(transform);s/$$/$(EXEEXT)/'`; \
	  echo " rm -f $(DESTDIR)$(bindir)/$$f"; \
	  rm -f $(DESTDIR)$(bindir)/$$f; \
	done

clean-binPROGRAMS:
	-test -z "$(bin_PROGRAMS)" || rm -f $(bin_PROGRAMS)
hpiSubagent$(EXEEXT): $(hpiSubagent_OBJECTS) $(hpiSubagent_DEPENDENCIES) 
	@rm -f hpiSubagent$(EXEEXT)
	$(LINK) $(hpiSubagent_LDFLAGS) $(hpiSubagent_OBJECTS) $(hpiSubagent_LDADD) $(LIBS)

mostlyclean-compile:
	-rm -f *.$(OBJEXT) core *.core

distclean-compile:
	-rm -f *.tab.c

include $(DEPDIR)/alarm.Po
include $(DEPDIR)/hpiSubagent.Po
include $(DEPDIR)/saHpiCtrlTable.Po
include $(DEPDIR)/saHpiEventTable.Po
include $(DEPDIR)/saHpiHotSwapTable.Po
include $(DEPDIR)/saHpiInventoryTable.Po
include $(DEPDIR)/saHpiRdrTable.Po
include $(DEPDIR)/saHpiSensorTable.Po
include $(DEPDIR)/saHpiSystemEventLogTable.Po
include $(DEPDIR)/saHpiTable.Po
include $(DEPDIR)/saHpiWatchdogTable.Po

distclean-depend:
	-rm -rf $(DEPDIR)

.c.o:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `test -f $< || echo '$(srcdir)/'`$<

.c.obj:
	source='$<' object='$@' libtool=no \
	depfile='$(DEPDIR)/$*.Po' tmpdepfile='$(DEPDIR)/$*.TPo' \
	$(CCDEPMODE) $(depcomp) \
	$(COMPILE) -c `cygpath -w $<`
CCDEPMODE = depmode=gcc3
uninstall-info-am:

tags: TAGS

ID: $(HEADERS) $(SOURCES) $(LISP) $(TAGS_FILES)
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	mkid -fID $$unique $(LISP)

TAGS:  $(HEADERS) $(SOURCES)  $(TAGS_DEPENDENCIES) \
		$(TAGS_FILES) $(LISP)
	tags=; \
	here=`pwd`; \
	list='$(SOURCES) $(HEADERS) $(TAGS_FILES)'; \
	unique=`for i in $$list; do \
	    if test -f "$$i"; then echo $$i; else echo $(srcdir)/$$i; fi; \
	  done | \
	  $(AWK) '    { files[$$0] = 1; } \
	       END { for (i in files) print i; }'`; \
	test -z "$(ETAGS_ARGS)$$unique$(LISP)$$tags" \
	  || etags $(ETAGS_ARGS) $$tags  $$unique $(LISP)

GTAGS:
	here=`CDPATH=: && cd $(top_builddir) && pwd` \
	  && cd $(top_srcdir) \
	  && gtags -i $(GTAGS_ARGS) $$here

distclean-tags:
	-rm -f TAGS ID GTAGS GRTAGS GSYMS GPATH

DISTFILES = $(DIST_COMMON) $(DIST_SOURCES) $(TEXINFOS) $(EXTRA_DIST)

top_distdir = ..
distdir = $(top_distdir)/$(PACKAGE)-$(VERSION)

distdir: $(DISTFILES)
	@for file in $(DISTFILES); do \
	  if test -f $$file; then d=.; else d=$(srcdir); fi; \
	  dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	  if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	    $(mkinstalldirs) "$(distdir)/$$dir"; \
	  fi; \
	  if test -d $$d/$$file; then \
	    cp -pR $$d/$$file $(distdir) \
	    || exit 1; \
	  else \
	    test -f $(distdir)/$$file \
	    || cp -p $$d/$$file $(distdir)/$$file \
	    || exit 1; \
	  fi; \
	done
check-am: all-am
check: check-am
all-am: Makefile $(PROGRAMS)

installdirs:
	$(mkinstalldirs) $(DESTDIR)$(bindir)

install: install-am
install-exec: install-exec-am
install-data: install-data-am
uninstall: uninstall-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am

installcheck: installcheck-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) INSTALL_PROGRAM="$(INSTALL_STRIP_PROGRAM)" \
	  `test -z '$(STRIP)' || \
	    echo "INSTALL_PROGRAM_ENV=STRIPPROG='$(STRIP)'"` install
mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES) stamp-h stamp-h[0-9]*

maintainer-clean-generic:
	@echo "This command is intended for maintainers to use"
	@echo "it deletes files that may require special tools to rebuild."
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)
clean: clean-am

clean-am: clean-binPROGRAMS clean-generic mostlyclean-am

distclean: distclean-am

distclean-am: clean-am distclean-compile distclean-depend \
	distclean-generic distclean-tags

dvi: dvi-am

dvi-am:

info: info-am

info-am:

install-data-am:

install-exec-am: install-binPROGRAMS

install-info: install-info-am

install-man:

installcheck-am:

maintainer-clean: maintainer-clean-am

maintainer-clean-am: distclean-am maintainer-clean-generic

mostlyclean: mostlyclean-am

mostlyclean-am: mostlyclean-compile mostlyclean-generic

uninstall-am: uninstall-binPROGRAMS uninstall-info-am

.PHONY: GTAGS all all-am check check-am clean clean-binPROGRAMS \
	clean-generic distclean distclean-compile distclean-depend \
	distclean-generic distclean-tags distdir dvi dvi-am info \
	info-am install install-am install-binPROGRAMS install-data \
	install-data-am install-exec install-exec-am install-info \
	install-info-am install-man install-strip installcheck \
	installcheck-am installdirs maintainer-clean \
	maintainer-clean-generic mostlyclean mostlyclean-compile \
	mostlyclean-generic tags uninstall uninstall-am \
	uninstall-binPROGRAMS uninstall-info-am


clean:
	rm -f *~
	rm -f *.o
# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
