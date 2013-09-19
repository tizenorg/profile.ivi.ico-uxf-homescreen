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
 *  @brief  This file is definition of CicoSCUser class
 */
//==========================================================================
#ifndef __CICO_SC_USER_H__
#define __CICO_SC_USER_H__

#include <string>
#include <vector>

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCLastInfo;

//==========================================================================
/**
 *  @brief  This class hold user information
 */
//==========================================================================
class CicoSCUser
{
public:
    // default constructor
    CicoSCUser();

    // destructor
    virtual ~CicoSCUser();

    // dump log this class member variables
    void dump(void);

private:
    // assignment operator
    CicoSCUser& operator=(const CicoSCUser &object);

    // copy constructor
    CicoSCUser(const CicoSCUser &object);

public:
    std::string name;        ///< user name
    std::string passwd;      ///< password
    std::string homescreen;  ///< application ID of user uses homescreen
    bool        autolaunch;  ///< homescreen autolaunch flag

    std::vector<CicoSCLastInfo*> lastInfoList;   ///< list of last information
};
#endif  // __CICO_SC_USER_H__
// vim:set expandtab ts=4 sw=4:
