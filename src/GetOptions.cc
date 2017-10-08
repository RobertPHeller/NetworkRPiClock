// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Tue Sep 5 17:16:49 2017
//  Last Modified : <170912.1256>
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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_REGEX_H
#include <regex.h>
#endif

#include <stdlib.h>
#include <string.h>

#include "GetOptions.h"

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#include <iostream>
#include <ostream>

// Initialize the long options structure.
const struct option GetOptions::longoptions[] = {
    {"background",no_argument, 0, 'b'},
    {"soundlib", required_argument, 0, 's'},
    {"url", required_argument, 0, 'u'},
    {"days", required_argument, 0, 'd'},
    {"help",no_argument, 0, 'h'},
    {0,0,0,0}   
};

// Initialize the short options string.
const char GetOptions::optstring[] = "s:u:d:hb";

GetOptions::GetOptions(int argc, char * const argv[]) {
    int c;
    int digit_optind = 0;
    opterr = 0;
    
    // Initialize the options to their default values.
    baseURL = BASEURL;   // Defined in config.h from configure.ac
    soundLib = SOUNDLIB; // Defined in config.h from configure.ac
    background = false;
    days = 1;
    help = false;
    parseerror = false;
    
    // Loop over all of the words on the command line...
    while (1) {
        // Initialize the option index loop control variables.
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        // Get the next option
        c = getopt_long(argc, argv,optstring,longoptions,&option_index);
        // If at the end of the command line options, exit the loop.
        if (c == -1) break;
        // Branch out over the option letter.
        switch (c) {
        case 'b': // --background, -b
            background = true;
            break;
        case 's': // --soundlib, -s
            soundLib = optarg;
            break;
        case 'u':  // --url, -u
            baseURL = optarg;
            break;
        case 'd': // --days, -d
            days = atoi(optarg);
            break;
        case 'h': // --help, -h
            help = true;
            break;
        case '?': // Undefined option.
            if (optopt == 0) {
                std::cerr << argv[0] << ": Unknown option: " << argv[optind-1] << std::endl;
            } else {
                std::cerr << argv[0] << ": Unknown option: " << (char)optopt << std::endl;
            }
            parseerror = true;
        }
    }
    //if (optind < argc) {
    //    logfile = argv[optind];
    //} else {
    //    std::cerr << argv[0] << ": Logfile missing." << std::endl;
    //    logfile = "";
    //    parseerror = true;
    //}
}

void GetOptions::Usage(const char *program) const {
    int iopt;
    
    // Display usage on stderr.
    std::cerr << "Usage: "<<program;
    for (iopt = 0; iopt < sizeof(longoptions)/sizeof(struct option); iopt++) {
        if (longoptions[iopt].name == NULL) break;
        std::cerr << " [--" << longoptions[iopt].name << ", -" << (char)longoptions[iopt].val;
        switch (longoptions[iopt].has_arg) {
        case no_argument: break;
        case required_argument: std::cerr << " value"; break;
        case optional_argument: std::cerr << " [value]"; break;
        }
        std::cerr << "]";
    }
    std::cerr /*<< " logfile"*/ << std::endl;
    std::cerr << std::endl;
    std::cerr << "Where: " << std::endl;
    std::cerr << "    --background, -b  Fade into the background: fork() and close channels 0, 1, 2, and disconnect from the controlling tty (if any)." << std::endl;
    std::cerr << "    --soundlib, -s    The directory where the sound WAV files are." << std::endl;
    std::cerr << "    --url, -u         The base URL to use." << std::endl;
    std::cerr << "    --days, -d        The number of days of advanced notice (default: 1)." << std::endl;
    std::cerr << "    --help, -h        Display this help." << std::endl;
    std::cerr << std::endl;
    //std::cerr << " and" << std::endl;
    //std::cerr << "    logfile is the log file to process." << std::endl;
    //std::cerr << std::endl;
}


