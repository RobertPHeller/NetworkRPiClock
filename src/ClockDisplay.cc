// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Thu Sep 7 10:23:25 2017
//  Last Modified : <171008.0841>
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

#include <cairo.h>

#include <string.h>

#include "ClockDisplay.h"

#include <iostream>
#include <iomanip>

// The "traditional" 7-Segment display lookup table.
digitBitmapMap ClockDisplay::digitBitmaps[] = {
    {_0_bits, _0_width, _0_height},
    {_1_bits, _1_width, _1_height},
    {_2_bits, _2_width, _2_height},
    {_3_bits, _3_width, _3_height},
    {_4_bits, _4_width, _4_height},
    {_5_bits, _5_width, _5_height},
    {_6_bits, _6_width, _6_height},
    {_7_bits, _7_width, _7_height},
    {_8_bits, _8_width, _8_height},
    {_9_bits, _9_width, _9_height}
};



void ClockDisplay::DisplayTime(int hours, int minutes, bool muteflag) {
    // Computer each of the digits.
    int hHigh = hours / 10;
    int hLow  = hours % 10;
    int mHigh = minutes / 10;
    int mLow  = minutes % 10;
    
    // Get the Cairo image and its parameters.
    int stride = cairo_image_surface_get_stride(surface);
    int height = cairo_image_surface_get_height(surface);
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    // Clear the image.
    memset(imgbits,0,stride*height);
    
    // If the high digit of hours is zero, don't display it.
    // If it is not zero, display.
    if (hHigh != 0) {
        drawdigit(hHigh,0);
    }
    // Display low hour digit.
    drawdigit(hLow,32);
    // Display the colon.
    drawcolon();
    // Display high minute digit.
    drawdigit(mHigh,64);
    // Display low minute digit.
    drawdigit(mLow,96);
    // Display mute flag
    if (muteflag) drawmute();
}

void ClockDisplay::drawdigit(int v, int xoff) {
    //std::cerr << "*** ClockDisplay::drawdigit(" << v << "," << xoff << ")" << std::endl;
    //std::cerr << "*** ClockDisplay::drawdigit() digitBitmaps["<<v<<"].bits = " << std::hex << ((unsigned long long int)digitBitmaps[v].bits) << std::dec << std::endl;
    // Display the digit bitmap.
    drawbitmap(digitBitmaps[v].bits,digitBitmaps[v].width,digitBitmaps[v].height,xoff);
}

void ClockDisplay::drawcolon() {
    // Draw the colon bitmap.
    drawbitmap(colon_bits,colon_width,colon_height,58);
}

void ClockDisplay::drawbitmap(unsigned char *bits,int w,int h,int xoff) {
    // Compute byte and bit offsets.
    int byteoff = xoff / 8;
    int bitoff  = xoff % 8;
    // Compute bitmap bitwidth.
    int bitwidth = ((w+7)/8);
    // Compute vertical offset.
    int voff = (64-46)/2;
    //std::cerr << "*** ClockDisplay::drawbitmap(): byteoff = " << byteoff << ", bitoff = " << bitoff << std::endl;
    // Fetch Cairo image parameters.
    int stride = cairo_image_surface_get_stride(surface);
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    //std::cerr << "*** ClockDisplay::drawbitmap(): imgbits = " << std::hex << ((unsigned long long int) imgbits) << std::dec << std::endl;
    // For every bit row.
    for (int ibr = 0; ibr < h; ibr++) {
        //std::cerr << "*** ClockDisplay::drawbitmap(): ibr = " << ibr << std::endl;
        // Get the row byte address.
        unsigned char *rbyte = &imgbits[(stride*(ibr+voff))+byteoff];
        //std::cerr << "*** ClockDisplay::drawbitmap(): rbyte is " << std::hex << ((unsigned long long int) rbyte) << std::dec << std::endl;
        // For every byte column (every 8 bits).
        for (int ibc = 0; ibc < w; ibc+=8) {
            //std::cerr << "*** ClockDisplay::drawbitmap(): ibc = " << ibc << std::endl;
            // Compute the byte index into the X11 bitmap.
            int bitindex = (ibr*bitwidth)+(ibc/8);
            //std::cerr << "*** ClockDisplay::drawbitmap(): bitindex = " << bitindex << std::endl;
            // Fetch the bitmap byte.
            unsigned short byte = bits[bitindex]; 
            // Compute the first byte.
            unsigned char byte0 = (byte<<bitoff) & 0x0ff;
            // Computer the second byte.
            unsigned char byte1 = ((byte<<bitoff)>>8) & 0x0ff;
            //std::cerr << "*** ClockDisplay::drawbitmap(): byte0 = " << 
            //      std::setfill('0') << std::setw(2) << std::hex << 
            //      ((unsigned int)byte0) << ", byte1 = " << 
            //      ((unsigned int)byte1) << std::dec << std::endl;
            // Or in the first byte.
            rbyte[ibc/8] |= byte0;
            // If there are additional bits, or them into the next byte.
            if (bitoff > 0) {
                rbyte[(ibc/8)+1] |= byte1;
            }

        }
    }
}


void ClockDisplay::drawmute() {
    int xx, yy, sx, xbyte, sy;
    // Fetch Cairo image parameters.
    int height = cairo_image_surface_get_height(surface);
    int width  = cairo_image_surface_get_width(surface);
    int stride = cairo_image_surface_get_stride(surface);
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    sy = height - MUTESIZE;
    for (yy = sy; yy < height; yy++) {
        sx = width - (yy-sy+1);
        for (xx = sx; xx < width; xx++) {
            int byteoff = xx / 8;
            int bitoff  = xx % 8;
            unsigned char *rbyte = &imgbits[(stride*yy)+byteoff];
            *rbyte |= (1 << bitoff);
        }
    }
}

