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
 *  @file   CicoSCUser.cpp
 *
 *  @brief  
 */
/*========================================================================*/

#include "CicoSCUser.h"
#include "CicoLog.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCUser::CicoSCUser()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCUser::~CicoSCUser()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump log CicoSCUser member variables
 */
//--------------------------------------------------------------------------
void
CicoSCUser::dump(void)
{
    ICO_DBG("user name=%s, pass=%s, homescreen=%s",
            name.c_str(), passwd.c_str(), homescreen.c_str());
}
// vim:set expandtab ts=4 sw=4:
