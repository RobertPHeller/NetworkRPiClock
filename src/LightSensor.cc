// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Sat Sep 9 11:41:14 2017
//  Last Modified : <170910.1952>
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

#include "LightSensor.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>

LightSensor::LightSensor() {
    char buffer[256];
    
    // Form the path to the value file for the GPIO pin.
    snprintf(buffer,sizeof(buffer),GPIOPINFORMAT,LIGHTSENSORPIN);
    fd = open(buffer,O_RDWR);
    // If open failure, raise an error.                                         
    if (fd < 0) {
        int err = errno;
        std::cerr << "Error opening GPIO pin "<<LIGHTSENSORPIN<<": "<<strerror(err)<<std::endl;
        exit(err);
    }
}

LightSensor::~LightSensor() {
    // Close the value file.
    close(fd);
}

bool LightSensor::DaytimeP() {
    char c ;
    
    // Get GPIO pin state.
    lseek(fd, 0L, SEEK_SET);
    read (fd, &c, 1);
    int state = (c == '0') ? LOW : HIGH ;
    // Convert the state to a boolean value.
    if (state == HIGH) {
        return true;
    } else {
        return false;
    }
}
