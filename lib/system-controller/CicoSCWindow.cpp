/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoSCWindow.h
 *
 *  @brief  
 */
//==========================================================================

#include "CicoSCWindow.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWindow::CicoSCWindow()
    // TODO initialize variables
    : nodeid(-1), displayid(-1), layerid(-1),
      zoneid(-1), subwindow(0), eventmask(0), 
      x(-1), y(-1), width(-1), height(-1),
      visible(false), raise(false), active(false)

{
    ICO_DBG("CicoSCWindow::CicoSCWindow Entry");
    ICO_DBG("CicoSCWindow::CicoSCWindow Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWindow::~CicoSCWindow()
{
    ICO_DBG("CicoSCWindow::~CicoSCWindow Entry");
    ICO_DBG("CicoSCWindow::~CicoSCWindow Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log CicoSCWindow member variables
 */
//--------------------------------------------------------------------------
void
CicoSCWindow::dump(void)
{
    ICO_DBG("CicoSCWindow: surfaceid=%d name=%s appid=%s pid=%d "
            "nodeid=%d displayid=%d layerid=%d zoneid=%d "
            "subwindow=%d eventmask=%d x=%d y=%d width=%d height=%d "
            "visible=%s raise=%s active=%s",
            surfaceid, name.c_str(), appid.c_str(), pid, nodeid, displayid,
            layerid, zoneid, subwindow, eventmask, x, y, width, height,
            visible ? "true" : "false", raise ? "true" : "false",
            active ? "true" : "false");
}
// vim:set expandtab ts=4 sw=4:
