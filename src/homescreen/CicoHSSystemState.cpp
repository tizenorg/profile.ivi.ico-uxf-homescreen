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
 *  @file   CicoHSSystemState.cpp
 *
 *  @brief  This file implementation of CicoHSSystemState class
 */
/*========================================================================*/

#include <cstdio>

#include "CicoHSSystemState.h"

CicoHSSystemState* CicoHSSystemState::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSSystemState::CicoHSSystemState()
    : m_regulation(false), m_nightmode(false)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSSystemState::~CicoHSSystemState()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  get this class instance
 */
//--------------------------------------------------------------------------
CicoHSSystemState*
CicoHSSystemState::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoHSSystemState();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get regulation state
 *
 *  @return regulation state
 *  @retval true    regulation on
 *  @retval false   regulation off
 */
//--------------------------------------------------------------------------
bool
CicoHSSystemState::getRegulation(void)
{
    return m_regulation;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set regulation state
 *
 *  @param  state   regulation state(true:on/false:off)
 */
//--------------------------------------------------------------------------
void
CicoHSSystemState::setRegulation(bool state)
{
    m_regulation = state;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get regulation state
 *
 *  @return night mode state
 *  @retval true    night mode on
 *  @retval false   night mode off
 */
//--------------------------------------------------------------------------
bool
CicoHSSystemState::getNightMode(void)
{
    return m_nightmode;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set regulation state
 *
 *  @param  state   regulation state(true:on/false:off)
 */
//--------------------------------------------------------------------------
void
CicoHSSystemState::setNightMode(bool state)
{
    m_nightmode = state;
}
// vim:set expandtab ts=4 sw=4:
