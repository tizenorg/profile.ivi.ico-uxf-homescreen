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
 *  @file   CicoLogConfig.h
 *
 *  @brief  This file is definition of CicoSysConLogConfig class
 */
//==========================================================================
#ifndef __CICO_SYS_CON_LOG_CONFIG_H__
#define __CICO_SYS_CON_LOG_CONFIG_H__

#include <string>
   
//==========================================================================
/**
 *  @brief  This class has function of access to my appliction log 
 *          configuration information
 */
//==========================================================================
class CicoSysConLogConfig 
{
public:
    // get my install
    static CicoSysConLogConfig* getInstance(void);

    // load configuration file
    int load(const std::string & confFile);

    // get log name
    std::string & getLogName(void);

    // get performance level enable state
    bool isPRF(void);

    // get trace level enable state
    bool isTRA(void);

    // get debug level enable state
    bool isDBG(void);

    // get info level enable state
    bool isINF(void);

    // get warning level enable state
    bool isWRN(void);

    // get critical level enable state
    bool isCRI(void);

    // get error level enable state
    bool isERR(void);

    // get flush log enable state
    bool isFlush(void);

    // get ecore log print enalbe state
    bool isEcore(void);

private:
    // default constructor
    CicoSysConLogConfig();

    // destructor
    ~CicoSysConLogConfig();

    // assignment operator
    CicoSysConLogConfig& operator=(const CicoSysConLogConfig &object);

    // copy constructor
    CicoSysConLogConfig(const CicoSysConLogConfig &object);

private:
    // this class instance
    static CicoSysConLogConfig* ms_myInstance;

    // log file path
    std::string m_confFile;

    // log name
    std::string m_name;

    // performance log enable
    bool m_levelPRF;

    // trace log enable
    bool m_levelTRA;

    // debug log enable
    bool m_levelDBG;

    // info log enable
    bool m_levelINF;

    // warning log enable
    bool m_levelWRN;

    // critical log enable
    bool m_levelCRI;

    // error log enable
    bool m_levelERR;

    // flush enable
    bool m_flush;

    // ecore log print eneble
    bool m_ecore;
};
#endif  // __CICO_SYSysConON_LOG_CONFIG_H__
// vim:set expandtab ts=4 sw=4:
