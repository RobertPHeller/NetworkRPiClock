// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Thu Sep 7 10:17:48 2017
//  Last Modified : <181001.0719>
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

#ifndef __CLOCKDISPLAY_H
#define __CLOCKDISPLAY_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cairo.h>

#include "_0.xbm"
#include "_1.xbm"
#include "_2.xbm"
#include "_3.xbm"
#include "_4.xbm"
#include "_5.xbm"
#include "_6.xbm"
#include "_7.xbm"
#include "_8.xbm"
#include "_9.xbm"
#include "colon.xbm"

/** Struct to hold an XBitmap. */
typedef struct {
    /** The bitmap itself. */
    unsigned char *bits;
    /** The width of the bitmap. */
    int  width;
    /** The height of the bitmap. */
    int  height; 
} digitBitmapMap;

#define MUTESIZE 4

/** @brief Class to display the time as a "traditional" 7-Segment display. */
class ClockDisplay {
private:
    /** The Cairo surface object containing the image bitmap.*/
    cairo_surface_t *surface;
    /** @brief Method to draw a digit.
     *  The digit is drawn from a bitmap of a "traditional" 7-Segment display
     * digit.
     * 
     * @param v The value of the digit (0 to 9).
     * @param xoff The X offset to draw the digit.
     */
    void drawdigit(int v,int xoff);
    /** @brief Method to draw the colon between the hours and minutes.
     */
    void drawcolon();
    /** @brief Method to draw a bitmap.
     * 
     * @param bits The array of bits to draw.
     * @param w The width of the bitmap.
     * @param h The height of the bitmap.
     * @param xoff the X Offset to draw the bitmap.
     */
    void drawbitmap(unsigned char *bits,int w,int h,int xoff);
    /** @brief draw mute flag.
     */
    void drawmute();
    /** The "traditional" 7-Segment lookup table. */
    static digitBitmapMap digitBitmaps[10];
protected:
    /** The default constructor is not used. */
    ClockDisplay() {}
public:
    /** The constructor takes the Cairo surface to draw to.
     * @param s The Cairo surface.
     */
    ClockDisplay(cairo_surface_t *s) {
        surface = s;
    }
    /** The destructor is a noop. */
    ~ClockDisplay() {}
    /** This method displays the time.
     * 
     * @param hours The hours to display.
     * @param minutes The minutes to display.
     */
    void DisplayTime(int hours, int minutes, bool muteflag, bool colonflag);
};

#endif // __CLOCKDISPLAY_H

