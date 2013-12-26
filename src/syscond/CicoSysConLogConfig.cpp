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
 *  @file   CicoSysConLogConfig.cpp
 *
 *  @brief  This file implementation of CicoSysConLogConfig class
 */
/*========================================================================*/    

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace boost::property_tree;
 
#include "CicoSysConLogConfig.h"

//==========================================================================    
//  private static variable
//==========================================================================    
CicoSysConLogConfig* CicoSysConLogConfig::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSysConLogConfig::CicoSysConLogConfig()
    : m_confFile(""), m_name("IcoSysConDaemon"),
      m_levelPRF(false), m_levelTRA(false), m_levelDBG(false),
      m_levelINF(false), m_levelWRN(false), m_levelCRI(false),
      m_levelERR(false), m_flush(false), m_ecore(false)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSysConLogConfig::~CicoSysConLogConfig()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get instance of CicoSysConLogConfig
 *
 *  @return  pointer of CicoSysConLogConfig object
 */
//--------------------------------------------------------------------------
CicoSysConLogConfig*
CicoSysConLogConfig::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSysConLogConfig();
    }
    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Get instance of CicoSysConLogConfig
 *
 *  @param  [in]    confFile    config file name
 *  @return 0 on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSysConLogConfig::load(const std::string & confFile)
{
    // load xml config file
    ptree root;
    read_xml(confFile, root);

    //<log>
    if (root.not_found() != root.find("system-controller.log")) {
        printf("log element not found");
        return -1;
    }

    ptree log = root.get_child("system-controller.log");

    ptree child;
    optional<std::string> sValue;
    optional<bool> bValue;

    // read <performance eneble="xxx"/>
    sValue = log.get_optional<std::string>("name");
    if (true == sValue.is_initialized()) {
        m_name = sValue.get();
    }

    // read <performance eneble="xxx"/>
    child  = log.get_child("level.performance");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelPRF = bValue.get();
    }

    // read <trace eneble="xxx"/>
    child  = log.get_child("level.trace");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelTRA = bValue.get();
    }

    // read <debug eneble="xxx"/>
    child  = log.get_child("level.debug");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelDBG = bValue.get();
    }

    // read <info eneble="xxx"/>
    child  = log.get_child("level.info");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelINF = bValue.get();
    }

    // read <warning eneble="xxx"/>
    child  = log.get_child("level.warning");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelWRN = bValue.get();
    }

    // read <critical eneble="xxx"/>
    child  = log.get_child("level.critical");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelCRI = bValue.get();
    }

    // read <error eneble="xxx"/>
    child  = log.get_child("level.error");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_levelERR = bValue.get();
    }

    // read <flush eneble="xxx"/>
    child  = log.get_child("flush");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_flush = bValue.get();
    }

    // read <ecore eneble="xxx"/>
    child  = log.get_child("ecore");
    bValue = child.get_optional<bool>("<xmlattr>.enable");
    if (true == bValue.is_initialized()) {
        m_ecore = bValue.get();
    }

    return 0;
}


//--------------------------------------------------------------------------
/**
 *  @brief  get log name
 */
//--------------------------------------------------------------------------
std::string & 
CicoSysConLogConfig::getLogName(void)
{
    return m_name;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get performance level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isPRF(void)
{
    return m_levelPRF;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get trace level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isTRA(void)
{
    return m_levelTRA;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get debug level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isDBG(void)
{
    return m_levelDBG;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get info level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isINF(void)
{
    return m_levelINF;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get warning level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isWRN(void)
{
    return m_levelWRN;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get critical level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isCRI(void)
{
    return m_levelCRI;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get error level enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isERR(void)
{
    return m_levelERR;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get flush log enable state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isFlush(void)
{
    return m_flush;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get ecore log print enalbe state
 */
//--------------------------------------------------------------------------
bool
CicoSysConLogConfig::isEcore(void)
{
    return m_ecore;
}
// vim:set expandtab ts=4 sw=4:
