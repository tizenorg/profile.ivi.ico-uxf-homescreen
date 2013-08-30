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
 *  @file   CicoSCUser.h
 *
 *  @brief  
 */
//==========================================================================
#ifndef __CICO_SC_USER_H__
#define __CICO_SC_USER_H__

#include <string>
using namespace std;

class CicoSCUser
{
public:
    // default constructor
    CicoSCUser();

    // destructor
    virtual ~CicoSCUser();

    // dump log CicoSCUser member variables
    void dump(void);

private:
    // assignment operator
    CicoSCUser& operator=(const CicoSCUser &object);

    // copy constructor
    CicoSCUser(const CicoSCUser &object);

public:
    string     name;        //!< user name
    string     passwd;      //!< password
    string     homescreen;  //!< application ID of user uses homescreen
};
#endif	// __CICO_SC_USER_H__
// vim:set expandtab ts=4 sw=4:
