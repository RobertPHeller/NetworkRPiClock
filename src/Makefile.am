##-*- makefile -*-############################################################
#
#  System        : 
#  Module        : 
#  Object Name   : $RCSfile$
#  Revision      : $Revision$
#  Date          : $Date$
#  Author        : $Author$
#  Created By    : Robert Heller
#  Created       : Tue Sep 5 16:03:44 2017
#  Last Modified : <240414.1939>
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

EXTRA_DIST = Doxyfile.in

noinst_LIBRARIES = librpinetclock.a
HEADERFILES = GetOptions.h ItemList.h Sounds.h ClockDisplay.h Button.h \
		LightSensor.h SSD1306.h GCalCli.h
librpinetclock_a_SOURCES = $(HEADERFILES) GetOptions.cc ItemList.cc Sounds.cc \
		ClockDisplay.cc _0.xbm _1.xbm _2.xbm _3.xbm _4.xbm _5.xbm \
		_6.xbm _7.xbm _8.xbm _9.xbm colon.xbm Button.cc LightSensor.cc \
		SSD1306.cc GCalCli.cc
AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src -I. -I./src \
	$(CAIRO_CFLAGS) $(PANGO_CAIRO_CFLAGS) $(LIBXML2_CFLAGS) $(ALSA_CFLAGS)

pkgdatadir = $(datadir)/$(PACKAGE)/Doc
noinst_DATA=html/index.html
pkgdata_DATA = internals.pdf
man3dir = $(mandir)/man3

html/index.html latex/refman.tex man/man3 : Doxyfile $(HEADERFILES)
	-rm -rf html latex man
	$(DOXYGEN)
	
internals.pdf: latex/refman.tex
	$(MAKE) -C latex
	mv latex/refman.pdf internals.pdf

clean-local:
	-rm -rf html latex man
	-rm -f doxygen.log  internals.pdf
	

install-data-hook: html/index.html man/man3
	$(INSTALL) -d $(DESTDIR)$(pkgdatadir)/InternalsHTML
	for p in html/*; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=`echo $$p | sed -e 's|^.*/||'`; \
	  echo " $(INSTALL_DATA) '$$d$$p' '$(DESTDIR)$(pkgdatadir)/InternalsHTML/$$f'"; \
	  $(INSTALL_DATA) "$$d$$p" "$(DESTDIR)$(pkgdatadir)/InternalsHTML/$$f"; \
	done
	test -z "$(man3dir)" || $(mkdir_p) "$(DESTDIR)$(man3dir)"
	@list=man/man3/* ; \
	for i in $$list; do \
	  file=$$i; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    3*) ;; \
	    *) ext='3' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " $(INSTALL_DATA) '$$file' '$(DESTDIR)$(man3dir)/$$inst'"; \
	  $(INSTALL_DATA) "$$file" "$(DESTDIR)$(man3dir)/$$inst"; \
	done
	
uninstall-hook: 
	for p in $(DESTDIR)$(pkgdatadir)/InternalsHTML/*; do \
	  if test -f "$$p"; then d=; else d="$(srcdir)/"; fi; \
	  f=`echo $$p | sed -e 's|^.*/||'`; \
	  echo " rm -f '$(DESTDIR)$(pkgdatadir)/InternalsHTML/$$f'"; \
	  rm -f "$(DESTDIR)$(pkgdatadir)/InternalsHTML/$$f"; \
	done
	@list=man/man3/* ; \
	for i in $$list; do \
	  file=$$i; fi; \
	  ext=`echo $$i | sed -e 's/^.*\\.//'`; \
	  case "$$ext" in \
	    3*) ;; \
	    *) ext='3' ;; \
	  esac; \
	  inst=`echo $$i | sed -e 's/\\.[0-9a-z]*$$//'`; \
	  inst=`echo $$inst | sed -e 's/^.*\///'`; \
	  inst=`echo $$inst | sed '$(transform)'`.$$ext; \
	  echo " rm -f '$(DESTDIR)$(man3dir)/$$inst'"; \
	  rm -f "$(DESTDIR)$(man3dir)/$$inst"; \
	done
	

