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
 *  @file   CicoSCSwitch.cpp
 *
 *  @brief  
 */
//==========================================================================

#include "CicoSCSwitch.h"
#include "CicoLog.h"

// default constructor
CicoSCSwitch::CicoSCSwitch()
    : swname(""), input(-1), fix(false)
{
}

// destructor
CicoSCSwitch::~CicoSCSwitch()
{
}

// dump log CicoSCSwitch member variables
void
CicoSCSwitch::dump(void)
{
    ICO_DBG("CicoSCSwitch: swname=%s input=%d fix=%d numCode=%d",
            swname.c_str(), input, fix);
    for (unsigned int i = 0; i < code.size(); ++i) {
        ICO_DBG("CicoSCSwitch: [%d] code=%d codename=%s",
                i, code[i], codename[i].c_str());
    }
}
// vim:set expandtab ts=4 sw=4:
