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
 *  @file   CicoHSSystemState.h
 *
 *  @brief  This file is definition of CicoHSSystemState class
 */
//==========================================================================
#ifndef __CICO_HS_SYSTEM_STATE_H__
#define __CICO_HS_SYSTEM_STATE_H__

//==========================================================================
/**
 *  @brief  This class hold system state
 */
//==========================================================================
class CicoHSSystemState
{
public:
    // get this class instance
    static CicoHSSystemState* getInstance(void);

    // get regulation state
    bool getRegulation(void);

    // set regulation state
    void setRegulation(bool state);

    // get night mode state
    bool getNightMode(void);

    // set night mode state
    void setNightMode(bool state);

private:
    // default constructor
    CicoHSSystemState();

    // destructor
    ~CicoHSSystemState();

    /// assignment operator
    CicoHSSystemState& operator=(const CicoHSSystemState &object);

    /// copy constructor
    CicoHSSystemState(const CicoHSSystemState &object);

public:
    static CicoHSSystemState* ms_myInstance;   ///< this class instance

    bool m_regulation;  ///< regulation state
    bool m_nightmode;   ///< night mode state
};
#endif  // __CICO_HS_SYSTEM_STATE_H__
// vim:set expandtab ts=4 sw=4:
