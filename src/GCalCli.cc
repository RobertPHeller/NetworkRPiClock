// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Sun Apr 14 19:39:44 2024
//  Last Modified : <240518.0902>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2024  Robert Heller D/B/A Deepwoods Software
//			51 Locke Hill Road
//			Wendell, MA 01379-9728
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// 
//
//////////////////////////////////////////////////////////////////////////////

static const char rcsid[] = "@(#) : $Id$";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <stdio.h>


#include <string>
#include <iostream>

#include <string.h>

#include "GCalCli.h"

namespace GCalCli {

Agenda::Agenda(const struct tm *starttime, int days)
{
    char buffer[256];
    
    std::string cmd = GCALCLIPROG;
    cmd += "--noauth_local_webserver --nocolor agenda --military --nostarted --tsv ";
    snprintf(buffer,sizeof(buffer),"-w %d %d/%d/%d:%02d:%02d +%dd",
             sizeof(buffer) - 5,
             starttime->tm_mon+1,starttime->tm_mday,
             starttime->tm_year+1900,starttime->tm_hour,
             starttime->tm_min,days);
    cmd += buffer;
    FILE *pipe = popen(cmd.c_str(),"r");
    if (pipe == NULL) {
        std::cerr << "*** Agenda: popen failed, errno is " << errno << std::endl;
        status_ = false;
        return;
    }
    body_ = "<?xml version=\"1.0\" ?>\n";
    body_ += "<calendar>\n";
    snprintf(buffer,sizeof(buffer),"  <days>%d</days>\n",days);
    body_ += buffer;
    while (fgets(buffer,sizeof(buffer),pipe) != NULL)
    {
        char date[16];
        int year, month, day;
        body_ += "  <item>\n";
        char *p = strchr(buffer,'\t');
        *p++ = '\0';
        body_ += "    <date>";
        if (sscanf(buffer,"%4d-%02d-%02d",&year,&month,&day) != 3) {
            pclose(pipe);
            status_ = false;
            return;
        }
        snprintf(date,sizeof(date),"%02d/%02d/%04d",month,day,year);
        body_ += date;
        body_ += "</date>\n";
        body_ += "    <starttime>";
        char *q = strchr(p,'\t');
        *q++ = '\0';
        body_ += p;
        body_ += "</starttime>\n";
        q = strchr(q,'\t'); q++;
        q = strchr(q,'\t'); q++;
        p = strchr(q,'\n');
        *p = '\0';
        body_ += "    <text>";
        body_ += q;
        body_ += "</text>\n";
        body_ += "  </item>\n";
    }
    body_ += "</calendar>\n";
    std::cerr << "*** Agenda: body_ is |" << body_ << "|" << std::endl;
    if (pclose(pipe) < 0) {
        std::cerr << "*** Agenda: pclose failed, errno is " << errno << std::endl;
        status_ = false;
    } else {
        status_ = true;
    }           
}
};
