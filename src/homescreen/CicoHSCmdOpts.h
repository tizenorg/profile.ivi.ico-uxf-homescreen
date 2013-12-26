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
 *  @file   CicoHSCmdOpts.h
 *
 *  @brief  This file is definition of CicoHSCmdOpts class
 */
//==========================================================================
#ifndef __CICO_HS_CMD_OPTS_H__
#define __CICO_HS_CMD_OPTS_H__

#include <string>

//==========================================================================
/**
 *  @brief  This class hold command options
 */
//==========================================================================
class CicoHSCmdOpts
{
public:
    // get this class instance
    static CicoHSCmdOpts* getInstance(void);

    // perse command options
    void parse(int argc, char *argv[]);

    // get login user name
    const std::string & getLoginUser(void);

    // get application history file path
    const std::string & getHistoryFilePath(void);

    /// get application default history file path
    const std::string & getDefaultHistoryFilePath(void);

    /// get application history file flag
    const std::string & getFlagPath(void);

private:
    // default constructor
    CicoHSCmdOpts();

    // destructor
    ~CicoHSCmdOpts();

    /// assignment operator
    CicoHSCmdOpts& operator=(const CicoHSCmdOpts &object);

    /// copy constructor
    CicoHSCmdOpts(const CicoHSCmdOpts &object);

public:
    /// this class instance
    static CicoHSCmdOpts* ms_myInstance;

    /// login user name
    std::string m_loginUser;
    /// application history file path
    std::string m_historyFilePath;
    /// application default history file path
    std::string m_defaultHistoryFilePath;
    /// application history file flag
    std::string m_flagPath;
};
#endif  // __CICO_HS_CMD_OPTS_H__
// vim:set expandtab ts=4 sw=4:
