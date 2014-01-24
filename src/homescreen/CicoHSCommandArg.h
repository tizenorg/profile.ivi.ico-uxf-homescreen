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
 *  @file   CicoHSCommandArg.h
 *
 *  @brief  This file is definition of CicoHSCommandArg class
 */
//==========================================================================
#ifndef __CICO_HS_COMMAND_ARG_H__
#define __CICO_HS_COMMAND_ARG_H__

#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

#include <ico_log.h>

//==========================================================================
/**
 *  @brief  This class holds command information
 */
//==========================================================================
class CicoHSCommandArg {
public:
    // default constructor
    CicoHSCommandArg();

    CicoHSCommandArg(const CicoHSCommandArg& src);

    CicoHSCommandArg(const char* appid,
                const char* zone,
                int visible,
                int pid);
    // destructor
     ~CicoHSCommandArg();

public:
    std::string m_appid;
    std::string m_zone;
    int  m_visible;
    int  m_pid;
};
#endif  // __CICO_HS_COMMAND_ARG_H__
// vim:set expandtab ts=4 sw=4:
