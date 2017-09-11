// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Wed Sep 6 17:07:28 2017
//  Last Modified : <170910.2124>
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

#ifndef __SOUNDS_H
#define __SOUNDS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <map>
#include <string>

/** Alert types. */
typedef enum {
    /** Todo today alert sound. */
    ToDoToday, 
    /** Event today alert sound. */
    EventToDay,
    /** Event soon alert sound. */
    EventSoon} 
AlertType;

/** Alert sound file map type. */
typedef std::map<AlertType, std::string> AlertSoundFileMap;


/** @brief Class that implements an alert sound. */
class AlertSound {
private:
    /** Alert Sound File Map. */
    static AlertSoundFileMap soundFileMap;
    /** The alert sound file for this instance. */
    std::string alertSoundFile;
protected:
    /** The defaut constructor is not used. */
    AlertSound() {}
public:
    /** Initialize the alert sound file map.
     * @param soundPath The path to the sound file library.
     */
    static void InitializeAlertSoundFileMap(std::string soundPath);
    /** Constructor: create an alert sound instance of the specificed type.
     * @param alert The alert type for this alert sound instance.
     */
    AlertSound(AlertType alert);
    /** Destructor is a noop. */
    ~AlertSound() {}
    /** Play the alert sound. */
    void Play();
};

#endif // __SOUNDS_H

