/**
* @file ITEADLIB_Nextion.cpp
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
* dummy CPP for Particle Build lib validation
*/

#if defined(SPARK)
#include "ITEADLIB_Nextion.h"
#include <stdarg.h>

#define NEX_RET_CMD_FINISHED                (0x01)
#define NEX_RET_EVENT_LAUNCHED              (0x88)
#define NEX_RET_EVENT_UPGRADED              (0x89)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)     
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_VALUE_HEAD                  (0x72)
#define NEX_RET_INVALID_CMD                 (0x00)
#define NEX_RET_INVALID_COMPONENT_ID        (0x02)
#define NEX_RET_INVALID_PAGE_ID             (0x03)
#define NEX_RET_INVALID_PICTURE_ID          (0x04)
#define NEX_RET_INVALID_FONT_ID             (0x05)
#define NEX_RET_INVALID_BAUD                (0x11)
#define NEX_RET_INVALID_VARIABLE            (0x1A)
#define NEX_RET_INVALID_OPERATION           (0x1B)

inline void swap(uint32_t* v1, uint32_t* v2)
{
  uint32_t dmy;
  dmy = *v2;
  *v2 = *v1;
  *v1 = dmy;
}

NexDisplay::NexDisplay(USARTSerial serial, uint32_t baudrate)
{ 
  __serial = serial;
  __baud = baudrate;
}

NexDisplay::NexDisplay(uint16_t width, uint16_t height, USARTSerial serial, uint32_t baudrate)
{
  __serial = serial;
  __baud = baudrate;
  __width = width;
  __height = height;
}

NexDisplay::~NexDisplay()
{
  if (!__components.empty())
    __components.clear();
}

bool NexDisplay::init(uint32_t baudrate)
{
  bool ret1 = false;
  bool ret2 = false;

  if (!baudrate)                           // no or zero baudrate -> use local __baud
    baudrate = __baud;
  else
    __baud = baudrate;
  __serial.begin(baudrate);
  sendCommand(NexCMDTERM);                 // flush buffer and reset command queue
  sendCommand(NexEXECRESPONSE, __bkCmd=1); // only return success results
  ret1 = recvRetCommandFinished();
  sendCommand(NexPAGEID, 0);               // show home page
  ret2 = recvRetCommandFinished();
  return ret1 && ret2;
}

int NexDisplay::sendCommand(const char *cmdPattern, ...)
{
  int ret = -1;
  char buf[64];
  va_list args;

  va_start(args, cmdPattern);
  ret = vsnprintf(buf, sizeof(buf), cmdPattern, args);
  va_end(args);

  while (__serial.read() >= 0);  // flush RX buffer only
  if (ret >= (int)sizeof(buf))    // cast to prevent warning [-Wsign-compare]
    ret *= -1; // if buffer was too short indicate error as neg. required length
  else if (ret > 0)
  {
    __serial.print(buf);
    if (strcmp((const char*)&buf[ret - 3], NexCMDTERM))  // if command was not properly terminated
      __serial.print(NexCMDTERM);          //   do it now
  }
  dbSerialPrintln(buf);

  return ret;
}

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
void NexDisplay::sendSkript(const char* cmd, bool noRR)
{
  char buf[12];
  while (__serial.read() >= 0);    // flush RX buffer only
  if (noRR)
  {
    snprintf(buf, sizeof(buf), NexEXECRESPONSE, 0);
    __serial.print(buf);           // deactivate result response
    __serial.print(NexCMDTERM);
  }
  __serial.print(cmd);
  if (noRR)
  {
    snprintf(buf, sizeof(buf), NexEXECRESPONSE, __bkCmd);
    __serial.print(buf);           // reactivate previous command response
    __serial.print(NexCMDTERM);
  }
}

/**
* Return current page id.
*
* @param pageId - output parameter,to save page id.
*
* @retval true - success.
* @retval false - failed.
*/
bool NexDisplay::sendCurrentPageId(uint8_t* pageId)
{

  bool ret = false;
  uint8_t temp[5] = { 0 };

  if (!pageId)
  {
    goto __return;
  }
  sendCommand("sendme");
  delay(50);
  __serial.setTimeout(100);
  if (sizeof(temp) != __serial.readBytes((char *)temp, sizeof(temp)))
  {
    goto __return;
  }

  if (temp[0] == NEX_RET_CURRENT_PAGE_ID_HEAD && strcmp((const char*)&temp[2], NexCMDTERM) == 0)
  {
    *pageId = temp[1];
    ret = true;
  }

__return:

  if (ret)
  {
    dbSerialPrint("recvPageId :");
    dbSerialPrintln(*pageId);
  }
  else
  {
    dbSerialPrintln("recvPageId err");
  }

  return ret;
}

/**
* Set backlight brightness value.
*
* @param dimValue   - current backlight brightness value.
* @param setDefault - store this value as default
*
* @retval true - success.
* @retval false - failed.
*/
bool NexDisplay::setBrightness(uint8_t dimValue, bool setDefault)
{
  bool ret = false;
  char cmd[16];

  if (setDefault)
    snprintf(cmd, sizeof(cmd), NexSETDIMS, dimValue);
  else
    snprintf(cmd, sizeof(cmd), NexSETDIM, dimValue);
  sendCommand(cmd);
  delay(10);

  if (recvRetCommandFinished())
  {
    dbSerialPrint("setCurrentBrightness[ ");
    dbSerialPrint(dimValue);
    dbSerialPrintln("]ok ");

    ret = true;
  }
  else
  {
    dbSerialPrintln("setCurrentBrightness err ");
  }

  return ret;
}

/**
* Set baudrate.
*
* @param  baudrate - baudrate, it supports 2400,4800,9600,19200,38400,57600,115200.
*
* @retval true - success.
* @retval false - failed.
*/
bool NexDisplay::setBaudrate(uint32_t baudrate, bool setDefault)
{
  bool ret = false;
  char cmd[16];

  if (!baudrate)              // no or zero baudrate -> use local __baud
    baudrate = __baud;
  else
    __baud = baudrate;

  if (setDefault)
    snprintf(cmd, sizeof(cmd), NexSETBAUDS, baudrate);
  else
    snprintf(cmd, sizeof(cmd), NexSETBAUD, baudrate);
  sendCommand(cmd);           // send in new baudrate using the current baudrate
  delay(10);

  __serial.flush();           // dump all returned data, since not usable with new baudrate
  __serial.end();             // close port (or pretend to ;-)
  __serial.begin(baudrate);   // activate new baudrate on MCU side too
  sendCommand("");            // trigger test transmission

  if (recvRetCommandFinished())
  {
    dbSerialPrintln("setBaudrate ok ");
    ret = true;
  }
  else
  {
    dbSerialPrintln("setBaudrate err ");
  }

  return ret;
}

void NexDisplay::sendRefreshAll(void)
{
  sendCommand(NexREFRESHALL);
}

bool NexDisplay::getNumeric(const char* varName, uint32_t* value)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexGET, varName);
  sendCommand(cmd);

  return recvRetNumber(value);
}

bool NexDisplay::setNumeric(const char* varName, uint32_t value)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexSETVAL, varName, value);
  sendCommand(cmd);
  return recvRetCommandFinished();
}

int NexDisplay::getInt(const char* varName)
{
  uint32_t val;
  if (NexGetNumeric(varName, &val))
    return (int)val;
  else
    return -1;
}

uint16_t NexDisplay::getString(const char* varName, char* text, uint16_t len)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexGET, varName);
  sendCommand(cmd);
  return recvRetString(text, len);
}

bool NexDisplay::setString(const char* varName, const char* text)
{
  char cmd[strlen(text) + 32];
  memset(cmd, 0, sizeof(cmd));
  snprintf(cmd, sizeof(cmd), NexSETSTRING, varName, text);
  sendCommand(cmd);
  return recvRetCommandFinished();
}

NexTouch& NexDisplay::add(NexTouch& newComponent) 
{
  __components.push_front(newComponent);
  return newComponent;
}

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
void NexDisplay::nexLoop()
{
  char buf[20];

  while (__serial.available())
  {
#if defined(SPARK)
    Particle.process();
    delay(5);
#else
    delay(10);
#endif
    switch(buf[0] = __serial.read())
    {
      case NEX_RET_EVENT_TOUCH_HEAD:
        if (__serial.available() >= 6)
        {
          __serial.readBytes(&buf[1], 6);
          buf[7] = 0x00;

          if (strcmp((const char*)&buf[4], NexCMDTERM) == 0)
            checkEvent(buf[1], buf[2], (int32_t)buf[3], NULL);
        }
        break;
      case NEX_RET_CURRENT_PAGE_ID_HEAD:
        dbSerialPrint("page cmd:");
        if (__serial.available() >= 4)
        {
          __serial.readBytes(&buf[1], 4);
          buf[5] = 0x00;

          if (strcmp((const char*)&buf[2], NexCMDTERM) == 0)
          {
            dbSerialPrintln(buf[1]);
            checkEvent(buf[1], 0, (int32_t)NEX_EVENT_PUSH, NULL);
          }
        }
        break;
      case NEX_RET_VALUE_HEAD:
        dbSerialPrint("value cmd:");
        if (__serial.available() >= 11)
        {
          __serial.readBytes(&buf[1], 11);
          buf[12] = 0x00;

          if (strcmp((const char*)&buf[9], NexCMDTERM) == 0)
          {
            dbSerialPrint(" Page:");
            dbSerialPrint(buf[1]);
            dbSerialPrint(" Component:");
            dbSerialPrint(buf[2]);
            dbSerialPrint(" Value:");
            dbSerialPrintln(buf[4] | (((unsigned long)buf[5]) << 8) | (((unsigned long)buf[6]) << 16) | (((unsigned long)buf[7]) << 24));
            checkEvent(buf[1], buf[2], (int32_t)buf[3], (void *)&(buf[4]));
          }
        }
        break;
      default:
        break;
    }
  }
}

// ----------------------------- graphics primitives ---------------------------

void NexDisplay::clearScreen(uint32_t color)
{
  char cmd[16];
  snprintf(cmd, sizeof(cmd), NexCLS, color);
  sendCommand(cmd);
}

void NexDisplay::plot(uint32_t x, uint32_t y, uint32_t color)
{
  drawLine(x, y, x, y, color);
}

void NexDisplay::drawLine(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
  char cmd[32];
  if (x1 > x2) swap(&x1, &x2);
  if (y1 > y2) swap(&y1, &y2);
  snprintf(cmd, sizeof(cmd), NexDRAWLINE, x1, y1, x2, y2, color);
  sendCommand(cmd);
}

void NexDisplay::drawRectAbs(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
  char cmd[32];
  if (x1 == x2 || y1 == y2)
  {
    drawLine(x1, y1, x2, y2, color);
    return;
  }
  if (x1 > x2) swap(&x1, &x2);
  if (y1 > y2) swap(&y1, &y2);
  snprintf(cmd, sizeof(cmd), NexDRAWRECT, x1, y1, x2 - 1, y2 - 1, color);
  sendCommand(cmd);
}
void NexDisplay::drawRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
  drawRectAbs(x, y, x + w, y + h, color);
}

void NexDisplay::fillRect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexFILLRECT, x, y, w, h, color);
  sendCommand(cmd);
}
void NexDisplay::fillRectAbs(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color)
{
  if (x1 > x2) swap(&x1, &x2);
  if (y1 > y2) swap(&y1, &y2);
  fillRect(x1, y1, x2 - x1, y2 - y1, color);
}

void NexDisplay::drawCircle(uint32_t x, uint32_t y, uint32_t r, uint32_t color)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexDRAWCIRCLE, x, y, r, color);
  sendCommand(cmd);
}

void NexDisplay::drawPic(uint32_t x, uint32_t y, uint32_t picID)
{
  char cmd[24];
  snprintf(cmd, sizeof(cmd), NexDRAWPIC, x, y, picID);
  sendCommand(cmd);
}

void NexDisplay::cropPic(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t picID)
{
  char cmd[32];
  snprintf(cmd, sizeof(cmd), NexCROPPIC, x, y, w, h, picID);
  sendCommand(cmd);
}

void NexDisplay::drawText(uint32_t x, uint32_t y, uint32_t w, uint32_t h, NexTEXTALIGN_t centerX, NexTEXTALIGN_t centerY,
  uint32_t fontID, uint32_t fontColor, uint32_t backColor, NexBACKGROUND_t backStyle,
  const char* text)
{
  char cmd[48 + strlen(text)];
  if (w == 0 || h == 0) return; // no space to draw text
  memset(cmd, 0, sizeof(cmd));
  snprintf(cmd, sizeof(cmd), NexTEXTBLOCK, x, y, w, h, fontID, fontColor, backColor, centerX, centerY, backStyle, text);
  sendCommand(cmd);
}
void NexDisplay::drawTextAbs(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, NexTEXTALIGN_t centerX, NexTEXTALIGN_t centerY,
  uint32_t fontID, uint32_t fontColor, uint32_t backColor, NexBACKGROUND_t backStyle,
  const char* text)
{
  if (x1 == x2 || y1 == y2) return; // no space to draw text
  if (x1 > x2) swap(&x1, &x2);
  if (y1 > y2) swap(&y1, &y2);
  drawText(x1, y1, x2 - x1, y2 - y1, centerX, centerY, fontID, fontColor, backColor, backStyle, text);
}


// ----------------------------- PRIVATES ---------------------------
/*
* Receive uint32_t data.
*
* @param number - save uint32_t data.
* @param timeout - set timeout time.
*
* @retval true - success.
* @retval false - failed.
*
*/
bool NexDisplay::recvRetNumber(uint32_t *number, uint32_t timeout)
{
  bool ret = false;
  uint8_t temp[8] = { 0 };

  if (!number)
  {
    goto __return;
  }

  __serial.setTimeout(timeout);
  if (sizeof(temp) != __serial.readBytes((char *)temp, sizeof(temp)))
  {
    goto __return;
  }

  if (temp[0] == NEX_RET_NUMBER_HEAD && strcmp((const char*)&temp[5], NexCMDTERM) == 0)
  {
    *number = (temp[4] << 24) | (temp[3] << 16) | (temp[2] << 8) | (temp[1]);
    ret = true;
  }

__return:

  if (ret)
  {
    dbSerialPrint("recvRetNumber :");
    dbSerialPrintln(*number);
  }
  else
  {
    dbSerialPrintln("recvRetNumber err");
  }

  return ret;
}


/*
* Receive string data.
*
* @param buffer - save string data.
* @param len - string buffer length.
* @param timeout - set timeout time.
*
* @return the length of string buffer.
*
*/
uint16_t NexDisplay::recvRetString(char *buffer, uint16_t len, uint32_t timeout)
{
  uint16_t ret = 0;
  bool str_start_flag = false;
  uint8_t cnt_0xff = 0;
  uint8_t c = 0;
  uint32_t start;

  uint16_t bufIdx = 0;

  if (!buffer || len == 0)
  {
    goto __return;
  }
  memset(buffer, 0, len);

  start = millis();
  while (cnt_0xff < 3 && millis() - start <= timeout)
  {
#if defined(SPARK)
    Particle.process();
#endif
    while (__serial.available())
    {
#if defined(SPARK)
      Particle.process();
#endif
      c = __serial.read();
      if (str_start_flag)
      {
        if (0xFF == c)
        {
          cnt_0xff++;
          if (cnt_0xff >= 3)
          {
            break;
          }
        }
        else if ((int)bufIdx < len - 1)
        {
          buffer[bufIdx++] = (char)c;
        }
      }
      else if (NEX_RET_STRING_HEAD == c)
      {
        str_start_flag = true;
      }
    }
  }

  ret = bufIdx;
  //ret = strlen(temp);
  //ret = ret > len ? len : ret;
  //strncpy(buffer, temp, ret);

__return:

  dbSerialPrint("recvRetString[");
  dbSerialPrint(strlen(buffer));
  dbSerialPrint(",");
  dbSerialPrint(buffer);
  dbSerialPrintln("]");

  return ret;
}

/*
* Command is executed successfully.
*
* @param timeout - set timeout time.
*
* @retval true - success.
* @retval false - failed.
*
*/
bool NexDisplay::recvRetCommandFinished(uint32_t timeout)
{
  bool ret = false;
  uint8_t temp[4] = "";

  __serial.setTimeout(timeout);
  if (sizeof(temp) != __serial.readBytes((char *)temp, sizeof(temp)))
    ret = false;

  if (temp[0] == NEX_RET_CMD_FINISHED && strcmp((const char*)&temp[1], NexCMDTERM) == 0)
    ret = true;

  if (ret)
  { // need curly braces due to dbSerialPrintln macro
    dbSerialPrintln("recvRetCommandFinished ok");
  }
  else
  { // need curly braces due to dbSerialPrintln macro
    dbSerialPrintln("recvRetCommandFinished err");
  }

  return ret;
}

void NexDisplay::checkEvent(uint8_t pid, uint8_t cid, int32_t event, void *value)
{
  if (__components.empty())
    return;

  for (auto comp = __components.begin(); comp != __components.end(); comp++)
  {
    if (comp->__pid == pid && comp->__cid == cid)
    {
      comp->printObjInfo();
      switch (event)
      {
        case NEX_EVENT_PUSH:
          comp->push();
          break;
        case NEX_EVENT_POP:
          comp->pop();
          break;
        case NEX_EVENT_VALUE:
          comp->value(NEX_EVENT_VALUE, value);
          break;
        case NEX_EVENT_STRING:
          comp->value(NEX_EVENT_STRING, value);
          break;
        default:
          break;
      }
      break;
    }
  }
}

#endif
