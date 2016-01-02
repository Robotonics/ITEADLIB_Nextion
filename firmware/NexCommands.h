/**
* @file NexCommands.h
*
* The definition of miscellaneous base API commands of Nextion.
* based on http://wiki.iteadstudio.com/Nextion_Instruction_Set
*
* @author  Andreas Rothenwänder (aka ScruffR <http://community.particle.io>)
* @date    2015/12
* @copyright
* Copyright (C) 2015-2016 ScruffR 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*/

#ifndef __NEXCOMMANDS_H__
#define __NEXCOMMANDS_H__
#if defined(SPARK)
#include "application.h"
#else
#include "Arduino.h"
#endif
#include "NexConfig.h"

#define NexCMDTERM       "\xff\xff\xff"             // command end marker (terminator)
#define NexPAGEID        "page %lu"                 // show page by ID
#define NexPAGENAME      "page %s"                  // show page by name
#define NexREFRESHID     "ref %lu"                  // refresh component by ID
#define NexREFRESHNAME   "ref %s"                   // refresh component by name
#define NexREFRESHALL    "ref 0"                    // refresh all components
#define NexGET           "get %s"                   // get any value
#define NexSETVAL        "%s=%lu"                   // set component's attribute (numeric value)
#define NexSETSTRING     "%s=\"%s\""                // set component's attribute (string value)
#define NexGETATTR       "get %s.%s"                // get component's attribute (compName.attrName)
#define NexSETATTRVAL    "%s.%s=%lu"                // set component's attribute (numeric value)
#define NexSETATTRSTRING "%s.%s=\"%s\""             // set component's attribute (string value)
#define NexGETPAGE       "sendme"                   // instructs the display to return the current page ID via serial
#define NexCONVERT       "cov %s,%s,%lu"            // converts internal strings to value and vice versa
                                                    //   third parameter describes string length 
                                                    //   =0 auto, >0 fixed length
#define NexSTOPTOUCH     "cle_c"                    // cancels all touch areas on the current page till page reload
#define NexCTLTOUCH      "tsw %s,%lu"               // start(1)/stop(0) touch recognition on component
                                                    //   tsw <component ID/ref>,1/0 
                                                    //   255 as ref denotes all components of current page
#define NexCOMPAUSE      "com_stop"                 // pauses execution of serial commands, but carries on receiving
                                                    //   keep in mind the limited buffer size (see HW manual)
#define NexCOMRESUME     "com_star"                 // resumes execution of serial commands including buffered ones
#define NexCOMCLEAR      "code_c"                   // clears the commands buffer
#define NexPRINT         "print %s"                 // instructs the display to send back some data (text or binary)
                                                    //   e.g. print v0.val returns the four byte binary value
                                                    //        print t0.txt returns the text
                                                    //        print 123    returns binary value
#define NexPRINTSTRING   "print \"%s\""             // instructs the display to send back a string
#define NexPRINTHEX      "printh %s"                // instructs the display to send back binary data queued as 
                                                    //   hex string "FF FF FF ..." (exactly one blank as seperator, no quotes)
#define NexWAVEADDPOINT  "add %u,%u,%u"             // add a point to a waveform component (compID,channel,value 0..255)
// graphics commands                            
#define NexCLS           "cls %lu"                  // clear screen with given color
#define NexDRAWLINE      "line %lu,%lu,%lu,%lu,%lu" // draw line x1,y1,x2,y2,color
#define NexDRAWRECT      "draw %lu,%lu,%lu,%lu,%lu" // draw rectangle x1,y1,x2,y2,color
#define NexFILLRECT      "fill %lu,%lu,%lu,%lu,%lu" // fill rectangle x,y,width,height,color
#define NexDRAWCIRCLE    "cir %lu,%lu,%lu,%lu"      // draw circle x,y,radius,color
#define NexDRAWPIC       "pic %lu,%lu,%lu"          // draw picture x,y,picID
#define NexCROPPIC       "picq %lu,%lu,%lu,%lu,%lu" // crop picture x,y,width,height,picID
#define NexTEXTBLOCK     "xstr %lu,%lu,%lu,%lu,%lu,%lu,%lu,%u,%u,%u,\"%s\"" // draw a text block (see docs)
// system variables      
#define NexSETDIM        "dim=%lu"                  // set backlight 0..100[%] brightness
#define NexSETDIMS       "dim=%lu"                  // set backlight 0..100[%] brightness and store as default
#define NexSETBAUD       "baud=%lu"                 // set the current baud rate for RX/TX
#define NexSETBAUDS      "bauds=%lu"                // set the current baud rate for RX/TX and store as default
#define NexSETFONTSPACEX "spax=%lu"                 // set horizontal font spacing (default 0)
#define NexSETFONTSPACEY "spay=%lu"                 // set vertical font spacing (default 0)
#define NexSETTOUCHCOLOR "thc=%lu"                  // set touch drawing color
#define NexSETTOUCHDRAW  "thdra=%lu"                // activate(1)/deactivate(0) touch drawing
#define NexSETSLEEPCOM   "ussp=%lu"                 // enter sleep after x seconds no serial communication (default 0=never)
#define NexSETSLEEPTOUCH "thsp=%lu"                 // enter sleep after x seconds no touch event (default 0=never)
#define NexSETWAKETOUCH  "thup=%lu"                 // wake on touch event (yes(1)/no(0))
#define NexSETSENDTOUCH  "sendxy=%lu"               // activate(1)/deactivate(0) transmission of touch coordinates
#define NexDELAY         "delay=%lu"                // pause code execution for x milli seconds
#define NexSLEEP         "sleep=%lu"                // enter(1)/exit(0) sleep mode
#define NexEXECRESPONSE  "bkcmd=%lu"                // set execution response type (0..3)
                                                    //   0 .. none
                                                    //   1 .. on success
                                                    //   2 .. on fail
                                                    //   3 .. always
#define NexCALIBRATE     "touch_j"                  // triggers touch calibration (usually not needed)
#endif
