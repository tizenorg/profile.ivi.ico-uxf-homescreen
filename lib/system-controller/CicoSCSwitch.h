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
 *  @file   CicoSCSwitch.h
 *
 *  @brief  This file is definition of CicoSCSwitch class
 */
//==========================================================================
#ifndef __CICO_SC_SWITCH_H__
#define __CICO_SC_SWITCH_H__

#include <vector>
#include <string>

//==========================================================================
/**
 *  @brief  This class hold input device switch information
 */
//==========================================================================
class CicoSCSwitch
{
public:
    // default constructor
    CicoSCSwitch();

    // destructor
    virtual ~CicoSCSwitch();

    // dump log CicoSCSwitch member variables
    void dump(void);

private:
    // assignment operator
    CicoSCSwitch& operator=(const CicoSCSwitch &object);

    // copy constructor
    CicoSCSwitch(const CicoSCSwitch &object);

public:
    static const int          SWITCH_NOFIX    = 0;
    static const int          SWITCH_FIX      = 1;
    static const unsigned int SWITCH_CODE_MAX = 8;
    std::string swname; //!< input switch name
    int         input;  //!< input switch number
    bool        fix;    //!< fixed application switch
    std::vector<int>    code;       //!< input switch code value
    std::vector<string> codename;   //!< input switch code name
};
#endif  // __CICO_SC_SWITCH_H__
// vim:set expandtab ts=4 sw=4:
