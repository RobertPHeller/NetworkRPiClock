// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Fri Sep 8 14:14:26 2017
//  Last Modified : <170912.1643>
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

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>

#include "Button.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

Button::Button(int pinno) {
    char buffer[256];
    
    // Form the path to the value file for the GPIO pin.
    snprintf(buffer,sizeof(buffer),GPIOPINFORMAT,pinno);
    // Open the value file.
    fd = open(buffer,O_RDWR);
    // If open failure, raise an error.
    if (fd < 0) {
        int err = errno;
        std::cerr << "Error opening GPIO pin "<<pinno<<": "<<strerror(err)<<std::endl;
        exit(err);
    }
    // Initialize instance variable.
    lastButtonState = HIGH;
    lastDebounceTime = 0;
    debounceDelay = 200;
}

Button::~Button() {
    // Close the value file.
    close(fd);
}

bool Button::Pushed()
{
    // Check button state..
    switch (checkstate()) {
        // Released
    case -1:
        // No change
    case 0: return false;
        // Pressed
    case 1: return true;
    }
    return false;
}

bool Button::Released()
{
    // Check button state..
    switch (checkstate()) {
        // Pressed
    case 1:
        // No change
    case 0: return false;
        // Released
    case -1: return true;
    }
    return false;
}

// Replace missing Arduino millis() function.
static unsigned long millis() {
    struct timespec systime;

    // Get high res time
    clock_gettime(CLOCK_REALTIME, &systime);
    // Compute milliseconds from nanoseconds.
    return ((systime.tv_sec*1000)+(systime.tv_nsec/1000000));
}

int Button::checkstate()
{
    // Assume no state change.
    int result = 0;
    int reading; // = digitalRead(buttonPin);
    char c ;
    
    // Get GPIO pin state.
    lseek(fd, 0L, SEEK_SET);
    read (fd, &c, 1);
    reading = (c == '0') ? LOW : HIGH ;
    
    // If there was a state change, get debouse time.
    if (reading != lastButtonState) {
        lastDebounceTime = millis();
    }
    // If there was enough delay, confirm the button state.
    if ((millis() - lastDebounceTime) > debounceDelay) {
        // If the button state changed, update the button state and return
        // the state change.
        if (reading != buttonState) {
            buttonState = reading;
            if (buttonState == HIGH) {
                result = -1;
            } else {
                result = 1;
            }
        }
    }
    // Save pending button state.
    lastButtonState = reading;
    // And return state change status.
    return result;
}

