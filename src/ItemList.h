// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Wed Sep 6 12:08:17 2017
//  Last Modified : <180911.1138>
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

#ifndef __ITEMLIST_H
#define __ITEMLIST_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_TIME_H
#include <time.h>
#endif

#include <cairo.h>
#include <pango/pangocairo.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <string>
#include <vector>


/** @brief The Date entry structure.
 * 
 * This is the structure that holds one date item.
 */
struct Date {
    /** The month. */
    int month;
    /** The date. */
    int date;
    /** The year. */
    int year;
    /** The flag indicating if there is a start time. */
    bool hasstarttime;
    /** The start time hour. */
    int starthour;
    /** The start time minute. */
    int startminute;
    /** The string containing the item's text. */
    std::string text;
    /** @brief The method to display the date on the display.
     * 
     * This method displays the date on the display.
     * 
     * @param surface The Cairo surface containing the image used for the OLed
     * display.
     */
    void DisplayDate(cairo_surface_t *surface) const;
    operator std::string() const
    {
        char buffer[256];
        if (hasstarttime) {
            snprintf(buffer,sizeof(buffer),"%d/%d/%d %2d:%02d",month,date,year,starthour,startminute);
        } else {
            snprintf(buffer,sizeof(buffer),"%d/%d/%d",month,date,year);
        }
        return std::string(buffer)+" "+text;
    }
    bool operator ==(const Date &other) const
    {
        //fprintf(stderr,"*** %s == %s\n",((std::string)*this).c_str(),((std::string)other).c_str());
        if (year == other.year &&
            month == other.month &&
            date == other.date) {
            //fprintf(stderr,"*** operator ==(): Date match: %s and %s\n",((std::string)*this).c_str(),((std::string)other).c_str());
            bool timematch = false; 
            if (hasstarttime && other.hasstarttime) {
                if (starthour == other.starthour &&
                    startminute == other.startminute) {
                    timematch = true;
                    //fprintf(stderr,"*** operator ==(): Time match: %s and %s\n",((std::string)*this).c_str(),((std::string)other).c_str());
                }
            } else {
                timematch = true;
                //fprintf(stderr,"*** operator ==(): Time match: %s and %s\n",((std::string)*this).c_str(),((std::string)other).c_str());
            }
            if (timematch && text == other.text) {
                //fprintf(stderr,"*** operator ==(): Text match: %s and %s\n",((std::string)*this).c_str(),((std::string)other).c_str());
                return true;
            }
        }
        return false;
    }
    bool ExpiredP(const struct tm *tm_now) const
    {
        return (year < (tm_now->tm_year+1900)) ||
              (month < (tm_now->tm_mon+1)) ||
              (date < tm_now->tm_mday);
    }
};

/** A vector of dates. */
typedef std::vector<Date> DateVector;

void FlushOldDates(struct tm *tm_now,DateVector &dates);
bool FindDate(DateVector::const_iterator item,const DateVector &dates);

/** @brief Class to hold a collection of dates returned from the ical server.
 *
 * A class to hold the collection of dates returned from the ical server.
 * 
 * This class gets the XML from the ical server and parses it into its instance
 * variables.
 */
class ItemList {
private:
    /** The number of days. */
    int days;
    /** The list of items returned from the ical server. */
    DateVector items;
    /** The XML tree structure. */
    xmlDoc *tree;
    /** A method to process one XML node.
     * @param node An XML node.
     */
    void processAndAddItem(xmlNode *node);
protected:
    /** The default constructor is not used. */
    ItemList() {}
public:
    /** @brief The constructor processes the XML body returned by the ical 
     * server.
     * 
     * @param xmltree The string holding the XML body returned by the ical 
     * server.
     */
    ItemList(std::string xmltree);
    /** @brief The destructor frees up any memory used by the class instance.
     */
    ~ItemList();
    /** Method to return the days field.
     * 
     * @returns the days.
     */
    int Days() const {return days;}
    /** Method to return the number of date items.
     * 
     * @returns the number of date items.
     */
    int ItemCount() const {return items.size();}
    /** Method return a const iterator at the beginning of the date item 
     * vector.
     * 
     * @returns a const iterator at the beginning of the date item vector.
     */
    DateVector::const_iterator begin() const {return items.begin();}
    /** Method return a const iterator at the end of the date item 
     * vector.
     * 
     * @returns a const iterator at the end of the date item vector.
     */
    DateVector::const_iterator end()   const {return items.end();}
    /** Method return a iterator at the beginning of the date item 
     * vector.
     * 
     * @returns a iterator at the beginning of the date item vector.
     */
    DateVector::iterator begin()            {return items.begin();}
    /** Method return a iterator at the end of the date item 
     * vector.
     * 
     * @returns a iterator at the end of the date item vector.
     */
    DateVector::iterator end()              {return items.end();}
    /** Operator method to randomly access a const date in a date item vector.
     * 
     * @param index The date index to access.
     * @returns a const date item.
     */
    const Date & operator [] (int index) const {return items[index];}
    /** Operator method to randomly access a date in a date item vector.
     * 
     * @param index The date index to access.
     * @returns a date item.
     */
    Date & operator [] (int index)             {return items[index];}
};
    

#endif // __ITEMLIST_H

