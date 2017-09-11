// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue Sep 5 16:57:58 2017
//  Last Modified : <170910.1000>
//
//  Description	
//
//  Notes
//
//  History
//	
/////////////////////////////////////////////////////////////////////////////
//
//    Copyright (C) 2017  Robert Heller D/B/A Deepwoods Software
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

#include <cairo.h>

#include "GetOptions.h"
#include "restclient-cpp/restclient.h"
#include "ItemList.h"
#include "Sounds.h"
#include "ClockDisplay.h"
#include "Button.h"
#include "LightSensor.h"
#include "SSD1306.h"

#include <iostream>
#include <iomanip>

/**
 * @mainpage Preface
 * @htmlonly
 * <div class="contents">
 * <div class="textblock"><ol type="1">
 * <li><a class="el" href="Inroduction.html">Inroduction.</a></li>
 * <li><a class="el" href="Hardware.html">Hardware used.</a></li>
 * <li><a class="el" href="NetworkRPiClock.html">Deamon program (NetworkRPiClock).</a><ol type="1">
 * <li><a class="el" href="NetworkRPiClock.html#SYNOPSIS">SYNOPSIS</a></li>
 * <li><a class="el" href="NetworkRPiClock.html#DESCRIPTION">DESCRIPTION</a></li>
 * <li><a class="el" href="NetworkRPiClock.html#OPTIONS">OPTIONS</a></li>
 * <li><a class="el" href="NetworkRPiClock.html#PARAMETERS">PARAMETERS</a></li>
 * <li><a class="el" href="NetworkRPiClock.html#FILES">FILES</a></li>
 * <li><a class="el" href="NetworkRPiClock.html#AUTHOR">AUTHOR</a></li>
 * </ol></li></ol></div></div>
 * @endhtmlonly
 * @page Inroduction Inroduction.
 * I wanted to have a clock in my bedroom and wanted to also have a way of 
 * knowing what I needed to do during the day when I get up in the morning.  I 
 * have been using an old Tcl/Tk program named ical written by Sanjay Ghemawat
 * to manage my appointments, meetings, and todo list.  I adapted one of the 
 * utilites that comes with ical, along with a simple web server module that is
 * part of SDX to create a simple REST web app to serve calendar items in my
 * ical database.  I then devised the idea of using a Raspberry Pi with 
 * Adafruit's OLed 128x64 "Bonnet" to implement a network clock.  It would
 * normally just display the time, using a software implemented 7-segment 
 * display, but will also be able to display my appointments, meetings, and 
 * daily todo list, using the joystick on the Bonnet.  I also added a speaker
 * so the Raspberry Pi can provide audio "warnings" of things like upcoming
 * meetings and appointments.
 * @page Hardware Hardware used.
 * The hardware consists of:
 * - A Raspberry Pi, model 3 w/ power supply, micro SD card and case.
 * - An Adafruit PermaPro HAT board without eeprom.
 * - A stacking header for the HAT board.
 * - A DS1307 RTC module w/ battery
 * - A HW5P-1 Light Sensor.
 * - A tlv3701 Comparator, with some resistors and a LED to go with the HW5P-1 
 *   Light Sensor, to provide a clean logic input.
 * - A small speaker and 3.5mm plug.
 * - And Adafruit's OLed 128x64 "Bonnet".
 * 
 * I mounted the DS1307 RTC module on the PermaPro HAT board, along with the 
 * simple comparator circuit shown here:
 * @image latex LightSensor.png "Light Sensor Comparator circuit" width=4in
 * @image html  LightSensor.png
 * 
 * @page NetworkRPiClock Deamon program (NetworkRPiClock).
 * @section SYNOPSIS SYNOPSIS
 * 
 * NetworkRPiClock [--days, -d value] [--help, -h] [--url, -u URL] [--soundlib, -s path]
 * 
 * @section DESCRIPTION DESCRIPTION
 * 
 * This program, run as a daemon on the 'Pi, implements the clock.  It 
 * displays the current time (hours and minutes) on the Adafruit OLed 128x64 
 * "Bonnet", using a software implementation of a 7-segment display, simulating
 * the familar display of digital clocks.  It also using the light sensor to
 * enable or disable the audio alerts -- the alerts are only produced when the
 * room lights are on.  Also the joystick is used to control the display of
 * the days meetings, appointments, and daily to-do items.  The up and down
 * positions scroll through the available daily items.  Pressing straight in
 * toggles between clock and item display modes.
 * 
 * @section OPTIONS OPTIONS
 * 
 * - --soundlib, -s    The directory where the sound WAV files are stored.
 * - --url, -u         The base URL to use.
 * - --days, -d        The number of days of advanced notice (default: 1).
 * - --help, -h        Display help.
 * 
 * @section PARAMETERS PARAMETERS
 * 
 * None.
 * 
 * @section FILES FILES
 * 
 * There is an alert sound library.
 * 
 * @section AUTHOR AUTHOR
 *  Robert Heller \<heller\@deepsoft.com\>
 * 
 */

static void PrintDisplay(cairo_surface_t *surface) {
    int stride = cairo_image_surface_get_stride(surface);
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);
    //std::cerr << "*** PrintDisplay(): stride = " << stride << ", width = " << width << ", height = " << height << std::endl;
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    //std::cerr << "*** PrintDisplay(): imgbits is " << std::hex << ((unsigned long long int) imgbits) << std::dec << std::endl;
    //return;
    std::cout << "|";
    for (int icol = 0; icol < width; icol++) {
        std::cout << "-";
    }
    std::cout << "|" << std::endl;
    for (int irow = 0; irow < height; irow++) {
        unsigned char *imgrow = &imgbits[(stride*irow)];
        std::cout << "|";
        for (int icol = 0; icol < width; icol++) {
            int byteindex = icol / 8;
            int bitindex  = icol % 8;
            unsigned char byte = imgrow[byteindex];
            if ((byte >> bitindex) & 0x01) {
                std::cout << "*";
            } else {
                std::cout << " ";
            }
        }
        std::cout << "|" << std::endl;
    }
    std::cout << "|";
    for (int icol = 0; icol < width; icol++) {
        std::cout << "-";
    }
    std::cout << "|" << std::endl;
}

    

int main(int argc, char *argv[]) {
    
    time_t now;
    struct tm tm_now;
    char buffer[256];
    
    const struct timespec nsleep = {0,100000000};
    LIBXML_TEST_VERSION
    
    GetOptions ProgramOptions(argc,argv);
    if (ProgramOptions.Help()) {
        ProgramOptions.Usage(argv[0]);
        return 1;
    }
    if (ProgramOptions.ParseError()) {
        ProgramOptions.Usage(argv[0]);
        return 1;
    }                                
    
    AlertSound::InitializeAlertSoundFileMap(ProgramOptions.SoundLib());
    AlertSound todotoday(ToDoToday);
    AlertSound eventtoday(EventToDay);
    AlertSound eventsoon(EventSoon);
    Button Left(L_pin);
    Button Right(R_pin);
    Button Center(C_pin);
    Button Up(U_pin);
    Button Down(D_pin);
    Button AButton(A_pin);
    Button BButton(B_pin);
    LightSensor Light;
    enum {DisplayClock, DisplayItem} mode;

    std::string URL = ProgramOptions.BaseURL();
    URL += "?days=";
    snprintf(buffer,sizeof(buffer),"%d",ProgramOptions.Days());
    URL += buffer;
    ItemList *iList = NULL;
    DateVector::const_iterator curitem;
    bool havevaliditem = false;
    cairo_surface_t *display = cairo_image_surface_create(CAIRO_FORMAT_A1,128,64);
    ClockDisplay clockdpy(display);
    mode = DisplayClock;
    SSD1306 OLed(128,64);
    OLed.begin();
    while (true) {
        time(&now);
        localtime_r(&now,&tm_now);
        if (tm_now.tm_sec == 0) {
            if (!havevaliditem) mode = DisplayClock;
            if (mode == DisplayClock) {
                clockdpy.DisplayTime(tm_now.tm_hour,tm_now.tm_min);
            } else {
                curitem->DisplayDate(display);
            }
            OLed.WriteDisplay(display);
            //PrintDisplay(display);
            //std::cout << std::setfill('0') << std::setw(2) << tm_now.tm_hour << ":" << std::setfill('0') << std::setw(2) << tm_now.tm_min << std::endl;
            if ((tm_now.tm_min % 5) == 0) {
                RestClient::Response r = RestClient::get(URL);
                if (r.code == 200) {
                    if (iList != NULL) {
                        delete iList;
                        iList = NULL;
                    }
                    iList = new ItemList(r.body);
                    havevaliditem = false;
                    if (Light.DaytimeP()) {
                        for (DateVector::const_iterator item = iList->begin();
                             item != iList->end();
                             item++) {
                            if (item->month == (tm_now.tm_mon+1) && 
                                item->date  == tm_now.tm_mday &&
                                item->year  == (tm_now.tm_year+1900)) {
                                if (item->hasstarttime) {
                                    int stime = item->starthour*60+item->startminute;
                                    int now_minutes = tm_now.tm_hour*60+tm_now.tm_min;
                                    int minutesremain = stime - now_minutes;
                                    if (minutesremain > 0 && minutesremain <= 15) {
                                        eventsoon.Play();
                                    } else {
                                        eventtoday.Play();
                                    }
                                } else {
                                    todotoday.Play();
                                }
                            }
                        }
                    }
                }
            }
            sleep(1);
        }
        nanosleep(&nsleep,NULL);
        if (iList != NULL) {
            bool update = false;
            if (Up.Pushed()) {
                if (!havevaliditem) {
                    curitem = iList->begin();
                    havevaliditem = (curitem != iList->end());
                } else {
                    curitem++;
                    if (curitem == iList->end()) {
                        curitem = iList->begin();
                        havevaliditem = (curitem != iList->end());
                    }
                }
                if (havevaliditem) {
                    mode = DisplayItem;
                    update = true;
                }
            } else if (Down.Pushed()) {
                if (!havevaliditem) {
                    curitem = iList->end();
                    if (curitem != iList->begin()) {
                        curitem--;
                    } else {
                        havevaliditem = false;
                    }
                } else {
                    if (curitem == iList->begin()) {
                        curitem = iList->end();
                        if (curitem == iList->begin()) {
                            havevaliditem = false;
                        } else {
                            curitem--;
                            havevaliditem = true;
                        }
                    } else {
                        curitem--;
                    }
                }
                if (havevaliditem) {
                    mode = DisplayItem;
                    update = true;
                }
            } else if (Center.Pushed()) {
                if (mode == DisplayItem) {
                    mode = DisplayClock;
                } else if (havevaliditem) {
                    mode = DisplayItem;
                }
                update = true;
            }
            if (update) {
                if (mode == DisplayClock) {
                    clockdpy.DisplayTime(tm_now.tm_hour,tm_now.tm_min);
                } else {
                    curitem->DisplayDate(display);
                }
                OLed.WriteDisplay(display);
                //PrintDisplay(display);
                sleep(1);
            }
        }
    }
    
    xmlCleanupParser();

    return 0;
}
    

