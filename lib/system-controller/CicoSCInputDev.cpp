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
 *  @file   CicoSCInputDev.cpp
 *
 *  @brief  
 */
//==========================================================================

#include "CicoSCInputDev.h"
#include "CicoLog.h"

// default constructor
CicoSCInputDev::CicoSCInputDev()
    : device("")
{
}

// destructor
CicoSCInputDev::~CicoSCInputDev()
{
}

// dump log CicoSCInputDev member variables
void
CicoSCInputDev::dump(void)
{
    ICO_DBG("CicoSCInputDev: device=%d", device.c_str());
}
// vim:set expandtab ts=4 sw=4:
