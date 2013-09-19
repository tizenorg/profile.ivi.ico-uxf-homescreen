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
 *  @file   CicoSCLastInfo.h
 *
 *  @brief  This file is definition of CicoSCLastInfo class
 */
//==========================================================================
#ifndef __CICO_SC_LAST_INFO_H__
#define __CICO_SC_LAST_INFO_H__

#include <string>

#include "CicoLog.h"
   
//==========================================================================
/**
 *  @brief  This class hold application's last information
 */
//==========================================================================
class CicoSCLastInfo
{
public:
    // default constructor
    CicoSCLastInfo()
        : appid(""), lastinfo("") {}

    // destructor
    ~CicoSCLastInfo() {}

    // dump log this class member variables
    void dumpLastInfo(void)
    {
        ICO_DBG("CicoSCLastInfo: appid=%s lastinfo=%s",
                appid.c_str(), lastinfo.c_str());
    }

public:
    std::string appid;       ///< application id
    std::string lastinfo;    ///< application's last information
};
#endif  // __CICO_SC_LAST_INFO_H__
// vim:set expandtab ts=4 sw=4:
