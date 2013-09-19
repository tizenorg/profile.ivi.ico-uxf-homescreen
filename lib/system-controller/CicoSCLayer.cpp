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
 *  @file   CicoSCLayer.h
 *
 *  @brief  This file is implementation of CicoSCLayer class
 */
/*========================================================================*/

#include "CicoSCLayer.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCLayer::CicoSCLayer()
    : layerid(-1), type(-1), width(-1), height(-1),
      displayid(-1), menuoverlap(false)

{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCLayer::~CicoSCLayer()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCLayer::dump(void)
{
    ICO_DBG("CicoSCLayer: layerid=%d type=%d width=%d height=%d "
            "displayid =%d menuoverlap=%s",
            layerid, type, width, height, displayid,
            menuoverlap ? "true" : "false");
}
// vim:set expandtab ts=4 sw=4:
