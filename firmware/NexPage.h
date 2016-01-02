/**
* @file NexPage.h
*
* The definition of class NexPage. 
*
* @author Wu Pengfei (email:<pengfei.wu@itead.cc>)
* @date 2015/8/13
*
* @copyright 
* Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation; either version 2 of
* the License, or (at your option) any later version.
*
* Port for Particle platform (particle.io)
* by BSpranger & ScruffR (Dec. 2015)
*/

#ifndef __NEXPAGE_H__
#define __NEXPAGE_H__

class NexDisplay;

#include "NexTouch.h"
#include "NexDisplay.h"
#include <map>

class NexDisplay;

/**
 * @addtogroup Component 
 * @{ 
 */

/**
 * A special component , which can contain other components such as NexButton, 
 * NexText and NexWaveform, etc. 
 */
class NexPage: public NexTouch
{
protected: /* methods */
    /*
    * @copydoc NexObject::NexObject(uint8_t pid, uint8_t cid, const char *name, void *value);
    *          cid has to be 0 for pages
    */
    NexPage(uint8_t pid, uint8_t cid, const char *name, void *value = NULL);

public: /* methods */
    /**
    * @copydoc NexObject::NexObject(uint8_t pid, const char *name, void *value);
    */
    NexPage(NexDisplay& display, uint8_t pid, const char *name, void *value = NULL);

    /**
    * @copydoc NexObject::NexObject(uint8_t pid, const char *name, void *value);
    */
    NexPage(uint8_t pid, const char *name, void *value = NULL);

    /**
     * Show itself. 
     * 
     * @return true if success, false for failure.
     */
    bool show(void);

/*

In file included from NexDisplay.h:24:0,
                 from NexDisplay.cpp:16:
NexPage.h: In member function 'T& NexPage::add(T&, bool, bool)':
NexPage.h:73:23: error: invalid use of incomplete type 'class NexDisplay'
       return __display->add<T>(newComponent, withEvents, global);
                       ^
In file included from NexDisplay.h:23:0,
                 from NexDisplay.cpp:16:
NexObject.h:30:7: error: forward declaration of 'class NexDisplay'
  class NexDisplay;  // forward declare required class
        ^
In file included from NexDisplay.h:24:0,
                 from NexDisplay.cpp:16:
NexPage.h: In member function 'T& NexPage::add(uint8_t, const char*, void*, bool, bool)':
NexPage.h:77:23: error: invalid use of incomplete type 'class NexDisplay'
       return __display->add<T>(*this, compID, name, value, withEvents, global);
                       ^
In file included from NexDisplay.h:23:0,
                 from NexDisplay.cpp:16:
NexObject.h:30:7: error: forward declaration of 'class NexDisplay'
  class NexDisplay;  // forward declare required class
        ^

*/

    template<class T> T& add(T& newComponent, bool withEvents = false, bool global = false)
    {
      if (newComponent.__pid != __pid)
        return *this;  // component not part of this page return page
      return __display->add<T>(newComponent, withEvents, global);
    }
    template<class T> T& add(uint8_t compID, const char* name, void* value = NULL, bool withEvents = false, bool global = false)
    {
      return __display->add<T>(*this, compID, name, value, withEvents, global);
    }

protected: /* fields */
  // not yet implemented
  //std::map<uint16_t, NexObject> __components; /* map to hold components on page for access via the page object */
  //                                            // will be filled and used by the owning NexDisplay object
};
/**
 * @}
 */

#endif /* #ifndef __NEXPAGE_H__ */
