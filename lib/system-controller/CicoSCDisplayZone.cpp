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
    : zoneid(-1), x(-1), y(-1), width(-1), height(-1)
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
    ICO_DBG("CicoSCDisplayZone: zoneid=%d x=%d y=%d width=%d height=%d",
            zoneid, x, y, width, height);
}
// vim:set expandtab ts=4 sw=4:
