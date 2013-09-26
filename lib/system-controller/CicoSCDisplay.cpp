/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/
/**
 *  @file   CicoSCDisplay.cpp
 *
 *  @brief  This file implementation of CicoSCDisplay class
 */
/*========================================================================*/

#include <string>
#include <vector>
using namespace std;

#include "CicoSCDisplay.h"
#include "CicoSCLayer.h"
#include "CicoSCDisplayZone.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCDisplay::CicoSCDisplay()
    : displayid(-1),
      type(-1),
      nodeid(-1),
      displayno(-1),
      width(-1),
      height(-1),
      inch(-1),
      pWidth(-1),
      pHeight(-1),
      orientation(ORIENTATION_HORIZONTAL),
      name("")
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCDisplay::~CicoSCDisplay()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCDisplay::dump(void)
{
    ICO_DBG("CicoSCDisplay: displayid=%d type=%d nodeid=%d displayno=%d "
            "width=%d height=%d inch=%d pWidth=%d pHeight=%d "
            "orientation =%d name=%s",
            displayid, type, nodeid, displayno, width, height, inch,
            pWidth, pHeight, orientation, name.c_str());
 
    vector<CicoSCLayer*>::iterator itr;
    itr = layerList.begin();
    for (; itr != layerList.end(); ++itr) {
        const_cast<CicoSCLayer*>(*itr)->dump();
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  find CicoSCDisplayZone instance by fullname
 */
//--------------------------------------------------------------------------
CicoSCDisplayZone*
CicoSCDisplay::findDisplayZonebyFullName(const string & fullname)
{
//    ICO_DBG("CicoSCDisplay:findDisplayZonebyFullName Enter"
//            "(fullname=%s)", fullname.c_str());
 
    std::map<unsigned int, CicoSCDisplayZone*>::iterator itr;
    itr = zoneList.begin();
    for (; itr != zoneList.end(); ++itr) {
        if (itr->second->fullname == fullname) {
//            ICO_DBG("CicoSCDisplay:findDisplayZonebyFullName Leave(found)");
            return itr->second;
        }
    }

//    ICO_DBG("CicoSCDisplay:findDisplayZonebyFullName Leave(not found)");
    return NULL;
}
// vim:set expandtab ts=4 sw=4:
