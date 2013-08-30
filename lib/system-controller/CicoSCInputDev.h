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
 *  @file   CicoSCInputDev.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_INPUT_DEV_H__
#define __CICO_SC_INPUT_DEV_H__

#include <string>
#include <vector>
using namespace std;

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCSwitch;

//--------------------------------------------------------------------------
/**
 *  @brief  input device information
 */
//--------------------------------------------------------------------------
class CicoSCInputDev
{
public:
    // default constructor
    CicoSCInputDev();

    // destructor
    virtual ~CicoSCInputDev();

    // dump log CicoSCInputDev member variables
    void dump(void);

private:
    // assignment operator
    CicoSCInputDev& operator=(const CicoSCInputDev &object);

    // copy constructor
    CicoSCInputDev(const CicoSCInputDev &object);

public:
    string device;  //!< input device name
    int    type;    //!< type

    vector<CicoSCSwitch*> switchList;   //!< switch list
};
#endif	// __CICO_SC_INPUT_DEV_H__
// vim:set expandtab ts=4 sw=4:
