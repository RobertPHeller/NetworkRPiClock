// -!- c++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Sat Sep 9 15:23:25 2017
//  Last Modified : <170910.2107>
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

#ifndef __SSD1306_H
#define __SSD1306_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cairo.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <asm/ioctl.h>

// I2C definitions (lifted from wiringPiI2C.h).

/** I2C Slave. */
#define I2C_SLAVE       0x0703
/** I2C SMBUS. */
#define I2C_SMBUS       0x0720  /* SMBus-level access */

/** I2C SMBUS Read. */
#define I2C_SMBUS_READ  1
/** I2C SMBUS Write. */
#define I2C_SMBUS_WRITE 0

// SMBus transaction types

/** I2C SMBUS QUICK. */
#define I2C_SMBUS_QUICK             0
/** I2C SMBUS Byte (no data). */
#define I2C_SMBUS_BYTE              1
/** I2C SMBUS Byte data. */
#define I2C_SMBUS_BYTE_DATA         2 
/** I2C SMBUS Word data. */
#define I2C_SMBUS_WORD_DATA         3
/** I2C SMBUS Proc Call. */
#define I2C_SMBUS_PROC_CALL         4
/** I2C SMBUS Block data. */
#define I2C_SMBUS_BLOCK_DATA        5
/** I2C SMBUS I2C Block Broken. */
#define I2C_SMBUS_I2C_BLOCK_BROKEN  6
/** I2C SMBUS Block proc call. */
#define I2C_SMBUS_BLOCK_PROC_CALL   7           /* SMBus 2.0 */
/** I2C SMBUS I2C block data. */
#define I2C_SMBUS_I2C_BLOCK_DATA    8

// SMBus messages

/** Max size of SMBUS Block Data. */
#define I2C_SMBUS_BLOCK_MAX     32      /* As specified in SMBus standard */    
/** Max size of SMBUS I2C Block Data. */
#define I2C_SMBUS_I2C_BLOCK_MAX 32      /* Not specified but we use same structure */

// Structures used in the ioctl() calls

/** I2C SMBus data union. */
union i2c_smbus_data
{
    /** Byte data. */
    uint8_t  byte ;
    /** Word data. */
    uint16_t word ;
    /** Block data.
     * Index 0 contains the length, and the rest is the data bytes.
     */
    uint8_t  block [I2C_SMBUS_BLOCK_MAX + 2] ;    // block [0] is used for length + one more for PEC
} ;

/** Structure for accessing the I2C. */
struct i2c_smbus_ioctl_data
{
    /** Read/Write flag. */
    char read_write ;
    /** Command byte. */
    uint8_t command ;
    /** Data size/type. */
    int size ;
    /** Data. */
    union i2c_smbus_data *data ;
} ;


// The rest of this was lifted from Adafruit's Adafruit_SSD1306/SSD1306.py

// Constants
#define SSD1306_I2C_ADDRESS 0x3C
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR 0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA0
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_EXTERNALVCC 0x1
#define SSD1306_SWITCHCAPVCC 0x2

// Scrolling constants
#define SSD1306_ACTIVATE_SCROLL 0x2F
#define SSD1306_DEACTIVATE_SCROLL 0x2E
#define SSD1306_SET_VERTICAL_SCROLL_AREA 0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL 0x2A

/** Hardwire the I2C Device, assuming a "modern" RPi Model 2 or 3. */
#define I2CDEVICE "/dev/i2c-1"  // Assume a "modern" RPi Model 2 or 3.

/** @brief This class is adapted from Adafruit's Adafruit_SSD1306/SSD1306.py.
 * 
 * I used Adafruit_SSD1306/SSD1306.py as a model for this class.
 */
class SSD1306 {
private:
    /** File descriptor for the I2C device. */
    int fd;
    /** Width of the display. */
    int width;
    /** Height of the display. */
    int height;
    /** Number of pages in the display. */
    int pages;
    /** The data buffer. */
    unsigned char *buffer;
    /** Vccstate. */
    int _vccstate;
    /** Send a command byte.
     * @param c the command byte.
     * @returns the I/O status.
     */
    int command(unsigned char c);
    /** Send a data byte.
     * @param c the data byte.
     * @returns the I/O status.
     */
    int data(unsigned char c);
    /** Initialize the device. */
    void initialize();
protected:
    /** The default constructor is not use. */
    SSD1306() {}
public:
    /** @brief Constructor: create an interface to the OLed bonnet.
     * @param w Width of the OLed bonnet.
     * @param h Height of the OLed bonnet.
     */
    SSD1306(int w, int h);
    /** @brief Destructor: close down the interface to the OLed bonnet. */
    ~SSD1306();
    /** Initialize the interface to the OLed bonnet.
     * @param vccstate The Vcc state.
     */
    void begin(int vccstate=SSD1306_SWITCHCAPVCC);
    /** @brief Copy a Cairo image to the image buffer.
     * @param surface A Cairo surface.
     */
    void image(cairo_surface_t *surface);
    /** @brief Transfer the image buffer to the SSD1306 chip.
     * @returns the I/O status.
     */
    int display();
    /** Copy a Cairo image to the image buffer and transfer it to the SSD1306 
     *  chip.
     * @param surface A Cairo surface.
     * @returns the I/O status.
     */
    int WriteDisplay(cairo_surface_t *surface) {
        image(surface);
        return display();
    }
    /** Clear the display memory buffer. */
    void clear();
    /** Set the contrast.
     * @param contrast The contrast value to set the display to.
     */
    void set_contrast(unsigned char contrast);
};
    


#endif // __SSD1306_H

