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
 *  @brief  This file is implementation of CicoSCWindow class
 */
//==========================================================================

#include <string>
using namespace std;

#include "CicoSCWindow.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCWindow::CicoSCWindow()
    : surfaceid(-1), name(""), appid(""), pid(-1), layertype(-1),
      nodeid(-1), displayid(-1), layerid(-1), zone(""), zoneid(-1),
      subwindow(0), eventmask(0), x(-1), y(-1), width(-1), height(-1),
      visible(false), raise(false), active(false)
{
//    ICO_TRA("CicoSCWindow::CicoSCWindow Enter");
//    ICO_TRA("CicoSCWindow::CicoSCWindow Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCWindow::~CicoSCWindow()
{
//    ICO_TRA("CicoSCWindow::~CicoSCWindow Enter");
//    ICO_TRA("CicoSCWindow::~CicoSCWindow Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCWindow::dump(void)
{
    ICO_DBG("CicoSCWindow: surfaceid=0x%08X name=%s appid=%s pid=%d "
            "nodeid=%d displayid=%d layerid=%d zone=%s zoneid=%d "
            "subwindow=%d eventmask=%d x=%d y=%d width=%d height=%d "
            "visible=%s raise=%s active=%s layertype=%x",
            surfaceid, name.c_str(), appid.c_str(), pid, nodeid, displayid,
            layerid, zone.c_str(), zoneid, subwindow, eventmask,
            x, y, width, height,
            visible ? "true" : "false", raise ? "true" : "false",
            active ? "true" : "false", layertype);
}
// vim:set expandtab ts=4 sw=4:
