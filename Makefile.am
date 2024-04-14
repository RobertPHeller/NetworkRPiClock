##-*- makefile -*-############################################################
#
#  System        : 
#  Module        : 
#  Object Name   : $RCSfile$
#  Revision      : $Revision$
#  Date          : $Date$
#  Author        : $Author$
#  Created By    : Robert Heller
#  Created       : Tue Sep 5 15:29:32 2017
#  Last Modified : <240414.1845>
#
#  Description	
#
#  Notes
#
#  History
#	
#  $Log$
#
##############################################################################
#
#    Copyright (C) 2017  Robert Heller D/B/A Deepwoods Software
#			51 Locke Hill Road
#			Wendell, MA 01379-9728
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
# 
#
##############################################################################


ACLOCAL_AMFLAGS = -I m4
SUBDIRS = src sounds

EXTRA_DIST = bootstrap Doxyfile.in LightSensor.png GPIOReset.sh  GPIOSetup.sh

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src -I. -I./src \
	$(CAIRO_CFLAGS) $(PANGO_CAIRO_CFLAGS) $(LIBXML2_CFLAGS) \
	$(ALSA_CFLAGS)
	
sbin_SCRIPTS  = GPIOReset.sh  GPIOSetup.sh
bin_PROGRAMS = NetworkRPiClock
NetworkRPiClock_SOURCES = NetworkRPiClock.cc
NetworkRPiClock_LDADD = src/librpinetclock.a \
        $(CAIRO_LIBS) $(PANGO_CAIRO_LIBS) $(LIBXML2_LIBS) \
        $(ALSA_LIBS)

man_MANS = man/man8/NetworkRPiClock.8
pkgdatadir = $(datadir)/$(PACKAGE)/Doc
noinst_DATA=html/index.html
pkgdata_DATA = UserMan.pdf
man8dir = $(mandir)/man8

noinst_PROGRAMS = Libtest
Libtest_SOURCES = Libtest.cc
Libtest_LDADD = src/librpinetclock.a $(CAIRO_LIBS) \
	$(PANGO_CAIRO_LIBS) $(LIBXML2_LIBS)

src/librpinetclock.a: FORCE
	$(MAKE) -C src librpinetclock.a

FORCE:

html/index.html latex/refman.tex man/man8/NetworkRPiClock.8: NetworkRPiClock.cc Doxyfile
	-rm -rf man
	$(DOXYGEN)
	
UserMan.pdf: latex/refman.tex
	$(MAKE) -C latex
	mv latex/refman.pdf UserMan.pdf
                
clean-local:
	-rm -rf man
	
install-data-hook: html/index.html man/man8
	$(INSTALL) -d $(DESTDIR)$(pkgdatadir)/UserManHTML
	for p in html/*; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=`echo $$p | sed -e 's|^.*/||'`; \
	  echo " $(INSTALL_DATA) '$$d$$p' '$(DESTDIR)$(pkgdatadir)/UserManHTML/$$f'"; \
	  $(INSTALL_DATA) "$$d$$p" "$(DESTDIR)$(pkgdatadir)/UserManHTML/$$f"; \
	done
	test -z "$(man8dir)" || $(mkdir_p) "$(DESTDIR)$(man8dir)"
	@list=man/man8/* ; \
	for i in $$list; do \
	  file=$$i; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    8*) ;; \
	    *) ext='8' ;; \
          esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(man8dir)/$$inst'"; \
	  $(INSTALL_DATA) "$$file" "$(DESTDIR)$(man8dir)/$$inst"; \
        done

uninstall-hook: 
	for p in $(DESTDIR)$(pkgdatadir)/UserManHTML/*; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=`echo $$p | sed -e 's|^.*/||'`; \
	  echo " rm -f '$(DESTDIR)$(pkgdatadir)/UserManHTML/$$f'"; \
	  rm -f "$(DESTDIR)$(pkgdatadir)/UserManHTML/$$f"; \
	done
	@list=man/man8/* ; \
	for i in $$list; do \
	  file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    8*) ;; \
	    *) ext='8' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " rm -f '$(DESTDIR)$(man8dir)/$$inst'"; \
	  rm -f "$(DESTDIR)$(man8dir)/$$inst"; \
	done
                                                                                                                       
