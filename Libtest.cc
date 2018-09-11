// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue Sep 11 11:15:20 2018
//  Last Modified : <180911.1139>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2018  Robert Heller D/B/A Deepwoods Software
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

#include "ItemList.h"
//#include "Sounds.h"
//#include "ClockDisplay.h"
//#include "Button.h"
//#include "LightSensor.h"
//#include "SSD1306.h"
 
#include <iostream>
#include <iomanip>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int main(int argc, char *argv[]) {
    time_t now;
    struct tm tm_now; 
    char buffer[256];
    Date newDate;
    DateVector AckedDates;
    DateVector::const_iterator curitem;
    bool havevaliditem = false;
    
    time(&now);
    localtime_r(&now,&tm_now);
    
    newDate.month = tm_now.tm_mon+1;
    newDate.year  = tm_now.tm_year+1900;
    newDate.date  = tm_now.tm_mday;
    newDate.hasstarttime = false;
    newDate.text = "Today";
    AckedDates.push_back(newDate);
    
    newDate.date  = tm_now.tm_mday-1;
    newDate.text = "Yesterday";
    AckedDates.push_back(newDate); 
    
    while (true) {
        time(&now);
        localtime_r(&now,&tm_now);
        if (tm_now.tm_sec == 0) {
            /*if (tm_now.tm_min == 0)*/ FlushOldDates(&tm_now,AckedDates);
            std::cerr << "*** main(): Top of 1 min loop, after FlushOldDates(): AckedDates now has " << AckedDates.size() << " items" << std::endl;
        }
        sleep(1);
    }
}

