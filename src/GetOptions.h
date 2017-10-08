// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue Sep 5 17:14:44 2017
//  Last Modified : <170912.1244>
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

#ifndef __GETOPTIONS_H
#define __GETOPTIONS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <iostream>
#include <string>

/** @brief Class to process the command line.
 * 
 * This class getss and holds the command like options and parameters.
 * The command line is processed by the getopt_long library function.
 * 
 * The options processed are:
 * 
 * - --soundlib, -s    The directory where the sound WAV files are.
 * - --url, -u         The base URL to use.
 * - --days, -d        The number of days of advanced notice (default: 1).
 * - --help, -h        Display this help.
 * 
 */
class GetOptions {
public:
private:
    /** Structure holding the long option information. */
    static const struct option longoptions[];
    /** String to hold the short options. */
    static const char optstring[];
    /** The days option value. */
    int days;
    /** The base URL option value. */
    std::string baseURL;
    /** The sound library option value. */
    std::string soundLib;
    /** The help option value. */
    bool help;
    /** The background flag. */
    bool background;
    /** Parse error flag. */
    bool parseerror;
protected:
    /** The default constructor is not used. */
    GetOptions() {}
public:
    /** The constructor parses the command line.
     * 
     * @param argc The count of command line words.
     * @param argv The vector of command line words.
     */
    GetOptions(int argc, char * const argv[]); 
    /** The destructor is a noop. */
    ~GetOptions() {}
    /** Method to return the base URL option.
     * @returns the base URL option value.
     */
    const std::string BaseURL() const {return baseURL;}
    /** Method to return the days option.
     * @returns the days option value.
     */
    int Days() const {return days;}
    /** Method to return the sound Lib option.
     * @returns the sound Lib option.
     */
    const std::string SoundLib() const {return soundLib;}
    /** Method to return the help option.
     * @returns the help option value.
     */
    bool Help() const {return help;} 
    /** Method to return the background option.
     * @returns the background option value.
     */
    bool Background() const {return background;}
    /** @returns the parse error flag.
     */
    bool ParseError() const {return parseerror;}
    /** Method to display the usage message.
     * @param program The name of the program.
     */
    void Usage(const char *program) const;
};

#endif // __GETOPTIONS_H
