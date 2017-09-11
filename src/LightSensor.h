// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Sat Sep 9 11:38:51 2017
//  Last Modified : <170910.1946>
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

#ifndef __LIGHTSENSOR_H
#define __LIGHTSENSOR_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Input pins: (BCM#s)

/** @brief Light Sensor GPIO. */
#define LIGHTSENSORPIN 21

/** @brief Input High. */
#define HIGH 1
/** @brief Input low. */
#define LOW  0

#ifndef GPIOPINFORMAT
/** @brief GPIO pin pathname. */
#define GPIOPINFORMAT "/sys/class/gpio/gpio%d/value"
#endif
#include <stdio.h>

/** @brief Light Sensor class.
 */
class LightSensor {
private:
    /** @brief File descriptor for the GPIO value. */
    int fd;
public:
    /** @brief Constructor: connect to the GPIO value file. */
    LightSensor();
    /** @brief Descriptor: disconnect to the GPIO value file. */
    ~LightSensor();
    /** @brief Return the state of GPIO pin. 
     * @returns the state of GPIO pin.
     */
    bool DaytimeP();
};

    


#endif // __LIGHTSENSOR_H

