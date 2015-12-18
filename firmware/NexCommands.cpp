/**
* @file NexCommands.cpp
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

#include "NexCommands.h"
#include <stdarg.h>

/*
#define NexCMDTERM      "\xff\xff\xff"         // command end marker (terminator)
#define NexPAGEID       "page %d"              // show page by ID
#define NexPAGENAME     "page %s"              // show page by name
#define NexREFRESHID    "ref %d"               // refresh component by ID
#define NexREFRESHNAME  "ref %s"               // refresh component by name
#define NexREFRESHALL   "ref 0"                // refresh all components
#define NexGETATTR      "get %s.%s"            // get component's attribute (compName.attrName)
#define NexSETVAL       "%s.%s=%d"             // set component's attribute (numeric value)
#define NexSETSTRING    "%s.%s=%s"             // set component's attribute (string value)
#define NexGETPAGE      "sendme"               // instructs the display to return the current page ID via serial
#define NexCONVERT      "cov %s,%s,%d"         // converts internal strings to value and vice versa
                                               //   third parameter describes string length
                                               //   =0 auto, >0 fixed length
#define NexSTOPTOUCH    "cle_c"                // cancels all touch areas on the current page till page reload
#define NexCTLTOUCH     "tsw %s,%d"            // start(1)/stop(0) touch recognition on component
                                               //   tsw <component ID/ref>,1/0
                                               //   255 as ref denotes all components of current page
#define NexCOMPAUSE     "com_stop"             // pauses execution of serial commands, but carries on receiving
                                               //   keep in mind the limited buffer size (see HW manual)
#define NexCOMRESUME    "com_star"             // resumes execution of serial commands including buffered ones
#define NexCOMCLEAR     "code_c"               // clears the commands buffer
#define NexPRINT        "print %s"             // instructs the display to send back some data (text or binary)
                                               //   e.g. print v0.val returns the four byte binary value
                                               //        print t0.txt returns the text
                                               //        print 123    returns binary value
                                               //        print "123"  returns 123
#define NexPRINTHEX     "printh %s"            // instructs the display to send back binary data queued as
                                               //   hex string "FF FF FF ..." (exactly one blank as seperator)
#define NexWAVEADDPOINT "add %d,%d,%d"         // add a point to a waveform component (compID,channel,value 0..255)
                                               // graphics commands
#define NexCLS          "cls %d"               // clear screen with given color
#define NexDRAWLINE     "line %d,%d,%d,%d,%d"  // draw line x1,y1,x2,y2,color
#define NexDRAWRECT     "draw %d,%d,%d,%d,%d"  // draw rectangle x1,y1,x2,y2,color
#define NexFILLRECT     "fill %d,%d,%d,%d,%d"  // fill rectangle x,y,width,height,color
#define NexDRAWCIRCLE   "cir %d,%d,%d,%d"      // draw circle x,y,radius,color
#define NexDRAWPIC      "pic %d,%d,%d"         // draw picture x,y,picID
#define NexCROPPIC      "picq %d,%d,%d,%d,%d"  // crop picture x,y,width,height,picID
#define NexTEXTBLOCK    "xstr %d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s"  // draw a text block (see docs)
// system variables
#define NexSETFONTSPACEX "spax=%d"             // set horizontal font spacing (default 0)
#define NexSETFONTSPACEY "spay=%d"             // set vertical font spacing (default 0)
#define NexSETTOUCHCOLOR "thc=%d"              // set touch drawing color
#define NexSETTOUCHDRAW  "thdra=%d"            // activate(1)/deactivate(0) touch drawing
#define NexSETSLEEPCOM   "ussp=%d"             // enter sleep after x seconds no serial communication (default 0=never)
#define NexSETSLEEPTOUCH "thsp=%d"             // enter sleep after x seconds no touch event (default 0=never)
#define NexSETWAKETOUCH  "thup=%d"             // wake on touch event (yes(1)/no(0))
#define NexSETSENDTOUCH  "sendxy=%d"           // activate(1)/deactivate(0) transmission of touch coordinates
#define NexDELAY         "delay=%d"            // pause code execution for x milli seconds
#define NexSLEEP         "sleep=%d"            // enter(1)/exit(0) sleep mode
#define NexEXECRESPONSE  "bkcmd=%d"            // set execution response type (0..3)
                                               //   0 .. none
                                               //   1 .. on success
                                               //   2 .. on fail
                                               //   3 .. always
#define NexCALIBRATE    "touch_j"              // triggers touch calibration (usually not needed)
*/

int NexSendCommand(const char *cmdPattern, ...)
{
  int ret = -1;
  char buf[64];
  va_list args;

  va_start(args, cmdPattern);
  ret = vsnprintf(buf, sizeof(buf), cmdPattern, args);
  va_end(args);

  while (nexSerial.read() >= 0);  // flush RX buffer only
  if (ret >= (int)sizeof(buf))    // cast to prevent warning [-Wsign-compare]
    ret *= -1; // if buffer was too short indicate error as neg. required length
  else if (ret > 0)
  {
    nexSerial.print(buf);
    if (strcmp((const char*)&buf[ret - 3], NexCMDTERM))  // if command was not properly terminated
      nexSerial.print(NexCMDTERM);          //   do it now
  }
  dbSerialPrintln(buf);

  return ret;
}
