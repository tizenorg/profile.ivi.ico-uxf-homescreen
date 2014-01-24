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
 *  @file   CicoHSCommandArg.cpp
 *
 *  @brief  This file is implementation of CicoHSCommandArg class
 */
//==========================================================================

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <exception>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
using namespace std;
using namespace boost;
using namespace boost::property_tree;

#include "CicoHSCommandArg.h"
#include <ico_log.h>
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSCommandArg::CicoHSCommandArg()
{
    m_appid = "";
    m_zone = "";
    m_visible = -1;
    m_pid = -1;
}

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSCommandArg::CicoHSCommandArg(const CicoHSCommandArg& src)
{
    m_appid = src.m_appid;
    m_zone = src.m_zone;
    m_visible = src.m_visible;
    m_pid = src.m_pid;
}

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSCommandArg::CicoHSCommandArg(const char* appid,
                               const char* zone,
                               int visible,
                               int pid)
{
    if ((NULL != appid) &&(0 !=  strlen(appid))) {
        m_appid = appid;
    }
    if ((NULL != zone) &&(0 !=  strlen(zone))) {
        m_zone = zone;
    }
    m_visible = visible;
    m_pid = pid;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSCommandArg::~CicoHSCommandArg()
{
}
// vim:set expandtab ts=4 sw=4:
