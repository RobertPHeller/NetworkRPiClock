// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Sat Sep 9 15:25:34 2017
//  Last Modified : <170911.1050>
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

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "SSD1306.h"

#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

// Heavily copied from wiringPiI2C and Adafruit's SSD1306.py.

static inline int i2c_smbus_access (int fd, char rw, uint8_t command, int size, union i2c_smbus_data *data)
{
    struct i2c_smbus_ioctl_data args ;
    
    args.read_write = rw ;
    args.command    = command ;
    args.size       = size ;
    args.data       = data ;
    return ioctl (fd, I2C_SMBUS, &args) ;
}

SSD1306::SSD1306(int w, int h) {
    width = w;
    height = h;
    pages = h/8;
    buffer = new unsigned char[width*pages];
    fd = open (I2CDEVICE,O_RDWR);
    if (fd < 0) {
        int err = errno;
        std::cerr << "Unable to open I2C device: "<<I2CDEVICE<<", because "<<strerror (err)<<std::endl;
        exit(err);
    }
    if (ioctl (fd, I2C_SLAVE, SSD1306_I2C_ADDRESS) < 0) {
        int err = errno;
        std::cerr << "Unable to select I2C device because "<<strerror (err)<<std::endl;
        exit(err);
    }
}

SSD1306::~SSD1306() {
    delete buffer;
    close(fd);
}

int SSD1306::command(unsigned char c) {
    unsigned char control = 0x00;
    union i2c_smbus_data data ;
    data.byte = c ;
    return i2c_smbus_access (fd, I2C_SMBUS_WRITE, control, I2C_SMBUS_BYTE_DATA, &data);
}

int SSD1306::data(unsigned char c) {
    unsigned char control = 0x40;
    union i2c_smbus_data data ;
    data.byte = c ;
    return i2c_smbus_access (fd, I2C_SMBUS_WRITE, control, I2C_SMBUS_BYTE_DATA, &data);
}

void SSD1306::initialize() {
    command(SSD1306_DISPLAYOFF);            // 0xA
    command(SSD1306_SETDISPLAYCLOCKDIV);    // 0xD5
    command(0x80);                          // the suggested ratio 0x80
    command(SSD1306_SETMULTIPLEX);          // 0xA8
    command(0x3F);
    command(SSD1306_SETDISPLAYOFFSET);      // 0xD3
    command(0x0);                           // no offset
    command(SSD1306_SETSTARTLINE | 0x0);    // line #0
    command(SSD1306_CHARGEPUMP);            // 0x8D
    if (_vccstate == SSD1306_EXTERNALVCC) {
        command(0x10);
    } else {
        command(0x14);
    }
    command(SSD1306_MEMORYMODE);            // 0x20
    command(0x00);                          // 0x0 act like ks0108
    command(SSD1306_SEGREMAP | 0x1);
    command(SSD1306_COMSCANDEC);
    command(SSD1306_SETCOMPINS);            // 0xDA
    command(0x12);
    command(SSD1306_SETCONTRAST);          // 0x81
    if (_vccstate == SSD1306_EXTERNALVCC) {
        command(0x9F);
    } else {
        command(0xCF);
    }
    command(SSD1306_SETPRECHARGE);         // 0xd9
    if (_vccstate == SSD1306_EXTERNALVCC) {
        command(0x22);
    } else {
        command(0xF1);
    }
    command(SSD1306_SETVCOMDETECT);        // 0xDB
    command(0x40);
    command(SSD1306_DISPLAYALLON_RESUME);  // 0xA4
    command(SSD1306_NORMALDISPLAY);        // 0xA6
}

void SSD1306::begin(int vccstate) {
    _vccstate = vccstate;
    initialize();
    command(SSD1306_DISPLAYON);
}

void SSD1306::image(cairo_surface_t *surface) {
    int stride = cairo_image_surface_get_stride(surface);
    int imwidth = cairo_image_surface_get_width(surface);cairo_image_surface_get_width(surface);
    int imheight = cairo_image_surface_get_height(surface);
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    if (imwidth != width || imheight != height) {
        std::cerr << "Image must be same dimensions as display!" << std::endl;
        return;
    }
    int index = 0;
    for (int page = 0; page < pages; page++) {
        for (int x = 0; x < width; x++) {
            unsigned char bits = 0;
            for (int bit = 0; bit < 8; bit++) {
                bits <<= 1;
                int rowindex = page*8+7-bit;
                int byteoff = rowindex*stride + x/8;
                int pix = (imgbits[byteoff] >> (x%8)) & 1;
                bits |= pix;
            }
            buffer[index++] = bits;
        }
    }
}

int SSD1306::display() {
    command(SSD1306_COLUMNADDR);
    command(0);              // Column start address. (0 = reset)
    command(width-1);        // Column end address.
    command(SSD1306_PAGEADDR);
    command(0);              // Page start address. (0 = reset)
    command(pages-1);        // Page end address.
    // Write buffer data.
    for (int i=0; i < (width*pages); i += 16) {
        int control = 0x40;
        union i2c_smbus_data data ;
        data.block[0] = 16;
        memcpy(&data.block[1],buffer+i,16);
        int status = i2c_smbus_access (fd, I2C_SMBUS_WRITE, control, I2C_SMBUS_I2C_BLOCK_DATA, &data);
        if (status < 0) return errno;
    }
    return 0;
}

void SSD1306::clear() {
    memset(buffer,0,width*pages);
}

void SSD1306::set_contrast(unsigned char contrast) {
    command(SSD1306_SETCONTRAST);
    command(contrast);
}


