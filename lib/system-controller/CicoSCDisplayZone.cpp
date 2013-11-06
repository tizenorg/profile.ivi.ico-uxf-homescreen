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
 *  @file   CicoSCDisplayZone.h
 *
 *  @brief  This file implementation of CicoSCDisplayZone class
 */
/*========================================================================*/

#include "CicoSCDisplayZone.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCDisplayZone::CicoSCDisplayZone()
    : zoneid(-1), x(-1), y(-1), width(-1), height(-1), fullname(""),
      displayedWindow(NULL), aspectFixed(false), aspectAlignLeft(false), 
      aspectAlignRight(false), aspectAlignTop(false), aspectAlignBottom(false)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCDisplayZone::~CicoSCDisplayZone()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log CicoSCDisplayZone member variables
 */
//--------------------------------------------------------------------------
void
CicoSCDisplayZone::dump(void)
{
    ICO_DBG("CicoSCDisplayZone: zoneid=%d "
            "x/y=%d/%d w/h=%d/%d fullname=%s window=0x%08x "
            "aspect(fixed=%s l/r=%s/%s t/b=%s/%s",
            zoneid, x, y, width, height, fullname.c_str(), displayedWindow,
            aspectFixed       ? "true" : "false",
            aspectAlignLeft   ? "true" : "false",
            aspectAlignRight  ? "true" : "false",
            aspectAlignTop    ? "true" : "false",
            aspectAlignBottom ? "true" : "false");
}
// vim:set expandtab ts=4 sw=4:
