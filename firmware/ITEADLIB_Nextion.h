/**
* @file ITEADLIB_Nextion.h
*
* The implementation of class NexCrop.
*
* @author  Andreas Rothenwänder (aka ScruffR http://community.particle.io)
* @date    2015/12/01
* @copyright
* Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* dummy header for Particle Build lib validation
*/

#pragma once
#include "Nextion.h"

#if defined(SPARK)
#include <forward_list>

/**
* Main class of the Nextion display.
*
* Provides the essential attributes of a Nextion component and the methods accessing
* them. At least, Page ID(pid), Component ID(pid) and an unique name are needed for
* creating a component in Nexiton library.
*/
class NexDisplay
{
public: /* methods */
  /**
  * Constructor.
  *
  * @param width    - display width in pixels
  * @param height   - display height in pixels
  * @param serial   - reference to the serial com port to use
  * @param baudrate - baud rate for communication between MCU and display
  */
  NexDisplay(USARTSerial serial = Serial1, uint32_t baudrate = 9600);
  NexDisplay(uint16_t width, uint16_t height, USARTSerial serial = Serial1, uint32_t baudrate = 9600);
  
  ~NexDisplay();


  /**
  * Initializes the serial communication and resets display
  *
  * @param baudrate - if 0, the value of __baud is kept otherwise set
  */
  bool init(uint32_t baudrate = 0);

  /**
  * Send a printf formatted command to the display
  *
  * @param cmdPattern - printf format string for command
  * @param ...        - printf value list
  */
  int sendCommand(const char *cmdPattern, ...);

  /*
  * Send a batch of commands (skript) each terminated by NexCMDTERM
  * If noRR is set TRUE
  *   For the batch the command execution result response gets
  *   temporarily deactivated.
  *   After completion it will be reactivated and set to the
  *   previously saved state (__bkCmd).
  *
  * @param cmd  - the skript (each command must be terminated individually)
  * @param noRR - indicate if result response should be deactivated (default false)
  */
  void sendSkript(const char* cmd, bool noRR);

  bool      sendCurrentPageId(uint8_t* pageId);
  bool      setBrightness(uint8_t dimValue, bool setDefault = false);
  bool      setBaudrate(uint32_t baudrate, bool setDefault = false);
  void      sendRefreshAll(void);

  bool      getNumeric(const char* varName, uint32_t* value);
  bool      setNumeric(const char* varName, uint32_t value);
  int       getInt(const char* varName);
  uint16_t  getString(const char* varName, char* text, uint16_t len);
  bool      setString(const char* varName, const char* text);

  NexTouch& add(NexTouch& newComponent);

  /**
  * Listen touch event and calling callbacks attached before.
  *
  * Supports push and pop at present.
  *
  * @param nex_listen_list - index to Nextion Components list.
  * @return none.
  *
  * @warning This function must be called repeatedly to response touch events
  *  from Nextion touch panel. Actually, you should place it in your loop function.
  */
  void nexLoop();

protected: /* methods */

private:   /* methods */
  bool     recvRetNumber(uint32_t *number, uint32_t timeout = 100);
  uint16_t recvRetString(char *buffer, uint16_t len, uint32_t timeout = 100);
  bool     recvRetCommandFinished(uint32_t timeout = 100);
  void     checkEvent(uint8_t pid, uint8_t cid, int32_t event, void *value);

private: /* data */
  std::forward_list<NexTouch> __components; /* forward list to hold components for standard access and event polling */
  USARTSerial __serial = Serial1; /* serial port for communicatino MCU <-> display */
  uint32_t __baud = 9600;         /* baudrate     */
  uint16_t __width  = 0;          /* pixel width  */
  uint16_t __height = 0;          /* pixel height */
  uint8_t  __bkCmd = 1;           /* command execution response behaviour */
                                  // 0 .. no response
                                  // 1 .. onSuccess (default
                                  // 2 .. noFail
                                  // 3 .. always
};
/**
* @}
*/
#endif
