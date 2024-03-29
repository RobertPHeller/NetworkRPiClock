// -!- C++ -!- //////////////////////////////////////////////////////////////
//
//  System        : 
//  Module        : 
//  Object Name   : $RCSfile$
//  Revision      : $Revision$
//  Date          : $Date$
//  Author        : $Author$
//  Created By    : Robert Heller
//  Created       : Wed Sep 6 12:25:33 2017
//  Last Modified : <210427.2143>
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

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include <string.h>

#include <stdio.h>

#include <iostream>

#include "ItemList.h"

#include <cairo.h>
#include <pango/pangocairo.h>

// Helper function to extract the text string of a node, if any.
static const xmlChar * GetText(xmlNode *node) {
    xmlNode *n;
    
    // Loop over all children of the node.
    for (n = node->children; n ; n = n->next) {
        // If there is a TEXT node, return its contents.
        if (n->type == XML_TEXT_NODE) {
            return n->content;
        }
    }
    // Otherwise, return NULL.
    return NULL;
}

ItemList::ItemList(std::string xmltree) {
    xmlNode *cur_node = NULL;
    
    //std::cerr << "*** ItemList::ItemList(" << xmltree << ")" << std::endl;
    
    // Initialize the days instance variable.
    days = 0;
    // Parse the XML text and build a xml tree.
    tree = xmlReadMemory(xmltree.c_str(),xmltree.size(),"noname.xml", NULL, 0);
    
    // Unless there was a problem.
    if (tree == NULL) {
        std::cerr << "Error: could not parse " << xmltree << std::endl;
        return;
    }
    
    // Get the root element.
    xmlNode *root_element = xmlDocGetRootElement(tree);
    
    // Loop over the root's children
    for (cur_node = root_element->children; cur_node; cur_node = cur_node->next) {
        // For each element node...
        if (cur_node->type == XML_ELEMENT_NODE) {
            //std::cerr << "*** ItemList::ItemList(): cur_node->name is " << cur_node->name << std::endl;
            // If it is the days node, extract the number of days from its
            // text contents.
            if (strcasecmp((const char *)cur_node->name,"days") == 0) {
                days = atoi((const char *)GetText(cur_node));
            // Else if it is an item node, process it and extract a date
            // from its children.
            } else if (strcasecmp((const char *)cur_node->name,"item") == 0) {
                processAndAddItem(cur_node);
            }
        }
    }
}

void ItemList::processAndAddItem(xmlNode *node) {
    xmlNode *child;
    Date newdate;
    
    // Assume there is no starttime.
    newdate.hasstarttime = false;
    
    // Loop over all the item's children, extract the item's information.
    for (child = node->children; child; child = child->next) {
        // For all of node's element children.
        if (child->type == XML_ELEMENT_NODE) {
            //std::cerr << "*** ItemList::processAndAddItem(): child->name is " << child->name << std::endl;
            // If the element is the date element, extract the month, date, 
            // and year.
            if (strcasecmp((const char *)child->name,"date") == 0) {
                sscanf((const char *)GetText(child),"%d/%d/%d",&newdate.month,&newdate.date,&newdate.year);
                // Else if the element is the starttime element, extract starthour 
                // and startminute.
            } else if (strcasecmp((const char *)child->name,"starttime") == 0) {
                newdate.hasstarttime = true;
                sscanf((const char *)GetText(child),"%02d:%02d",&newdate.starthour,&newdate.startminute);
                // Else if the element is the text element, extract the text.
            } else if (strcasecmp((const char *)child->name,"text") == 0) {
                const char * t = (const char *)GetText(child);
                if (t) {
                    newdate.text = t;
                } else {
                    newdate.text = "";
                }
            }
        }
    }
    // Append the new date to the list of dates.
    items.push_back(newdate);
}

ItemList::~ItemList() {
    // Free up the tree.
    xmlFreeDoc(tree);
}

void Date::DisplayDate(cairo_surface_t *surface) const {
    char buffer[256];
    
    // Create a Cairo context from the Cairo surface.
    cairo_t *context = cairo_create (surface);
    // Create a Pango layout.
    PangoLayout *layout = pango_cairo_create_layout (context);
    // Setup the font.
    PangoFontDescription *Courier = 
          pango_font_description_from_string ("Courier 14px");
    pango_layout_set_font_description (layout, Courier);
    pango_font_description_free(Courier);
    // Get the surface info.
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);
    int stride = cairo_image_surface_get_stride(surface);
    unsigned char *imgbits = cairo_image_surface_get_data(surface);
    // Clear the surface memory.
    memset(imgbits,0,stride*height);
    // Set the color.
    cairo_set_source_rgba(context,1.0,1.0,1.0,1.0);
    // Move the current point to the upper left coordinate.
    cairo_move_to(context,0,0);
    // Insert the Date and if there is one the Start Time.
    if (hasstarttime) {
        snprintf(buffer,sizeof(buffer),"%d/%d/%d %2d:%02d",month,date,year,starthour,startminute);
    } else {
        snprintf(buffer,sizeof(buffer),"%d/%d/%d",month,date,year);
    }
    pango_layout_set_text(layout,buffer,-1);
    pango_cairo_update_layout (context,layout);
    pango_cairo_show_layout (context,layout);
    
    // Then insert the text, line by line.
    double y = 16;
    for (int toff = 0; toff < text.size() && y < 64; toff += 16) {
        cairo_move_to(context,0,y);
        pango_layout_set_text(layout,text.substr(toff,16).c_str(),-1);
        pango_cairo_update_layout (context,layout);
        pango_cairo_show_layout (context,layout);
        y += 16;
    }
    // Free up Pango and Cairo 
    g_object_unref (layout);
    cairo_destroy (context);
}



void FlushOldDates(struct tm *tm_now,DateVector &dates)
{
    bool done = false;
    //fprintf(stderr,"*** FlushOldDates(): tm_now->year = %d, tm_now->tm_mon = %d, tm_now->tm_mday = %d, dates.size() = %d\n",
    //        tm_now->tm_year,tm_now->tm_mon,tm_now->tm_mday,dates.size());
    while (!done) {
        done = true;
        //fprintf(stderr,"*** FlushOldDates(): top of while loop: dates has %d items\n",dates.size());
        for (DateVector::iterator i = dates.begin(); i != dates.end(); i++)
        {
            //fprintf(stderr,"*** FlushOldDates(): Checking: %s\n",((std::string)*i).c_str());
            if (i->ExpiredP(tm_now)) {
                //fprintf(stderr,"*** FlushOldDates(): deleting: %s\n",((std::string)*i).c_str());
                dates.erase(i);
                done = false;
                break;
            }
        }
    }
}

bool FindDate(DateVector::const_iterator item,const DateVector &dates)
{
    //fprintf(stderr,"*** FindDate(): item is %s\n",((std::string)*item).c_str());
    for (DateVector::const_iterator i = dates.begin(); i != dates.end(); i++)
    {
        fprintf(stderr,"*** FindDate(): Checking: %s\n",((std::string)*i).c_str());
        if (*item == *i) {
            //fprintf(stderr,"*** FindDate(): Found: %s\n",((std::string)*i).c_str());
            return true;
        }
    }
    return false;
}



