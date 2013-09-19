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
 *  @brief  This file is implementation of CicoSCInputDev class
 */
//==========================================================================

#include <string>
#include <vector>
using namespace std;

#include "CicoSCInputDev.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCInputDev::CicoSCInputDev()
    : device(""), type(-1)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCInputDev::~CicoSCInputDev()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log this class member variables
 */
//--------------------------------------------------------------------------
void
CicoSCInputDev::dump(void)
{
    ICO_DBG("CicoSCInputDev: device=%d type=%d", device.c_str(), type);
}
// vim:set expandtab ts=4 sw=4:
