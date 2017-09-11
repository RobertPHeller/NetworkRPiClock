// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Fri Sep 8 13:38:48 2017
//  Last Modified : <170910.1037>
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

#ifndef __BUTTON_H
#define __BUTTON_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


// Input pins: (BCM#s)

/** @brief Joystick Left. */
#define L_pin 27
/** @brief Joystick Right. */
#define R_pin 23
/** @brief Joystick Center (in). */
#define C_pin 4 
/** @brief Joystick Up. */
#define U_pin 17
/** @brief Joystick Down. */
#define D_pin 22

/** @brief A button. */
#define A_pin 5 
/** @brief B button. */
#define B_pin 6 

/** @brief Input High. */
#define HIGH 1
/** @brief Input low. */
#define LOW  0

#ifndef GPIOPINFORMAT
/** @brief GPIO pin pathname. */
#define GPIOPINFORMAT "/sys/class/gpio/gpio%d/value"
#endif
#include <stdio.h>

/** @brief A Class that implements a debounced button on a GPIO pin.
 * 
 * This class (adapted from a class I wrote for an Arduino sketch) debounces
 * an input, typically connected to a push button.
 * 
 * @author Robert Heller \<heller\@deepsoft.com\>
 */
class Button {
private:
    /** File destriptor for the GPIO pin uder /sys/class/gpio/. */
    int fd;
    /** Current button state. */
    int buttonState;
    /** Last button state. */
    int lastButtonState;
    /** The last debounce time (in milliseconds). */
    long lastDebounceTime;
    /** The debounce delay. */
    long debounceDelay;
    /** Private member function to check the state of the button.
     * 
     * @returns -1 if the debounce input changed to HIGH (button released), 1 
     * if the changed to LOW (button pressed), or 0 if the button's state did
     * not change.
     */
    int checkstate();
protected:
    /** Protected default constructor -- this constructor is not used. */
    Button() {}
public:
    /** Constructor: construct a button instance.
     * 
     * @param pinno The GPIO pin for this button.
     */
    Button(int pinno);
    /** Destructor: deconstruct a button instance. */
    ~Button();
    /** Check if the button has been pushed.
     * 
     * @returns true if the button has been pushed and false if not.
     */
    bool Pushed();
    /** Check if the button has been released.
     * 
     * @returns true if the button has been released and false if not.
     */
    bool Released();
};
    


#endif // __BUTTON_H

