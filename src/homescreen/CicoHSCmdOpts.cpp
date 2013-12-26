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
 *  @file   CicoHSCmdOpts.cpp
 *
 *  @brief  This file implementation of CicoHSCmdOpts class
 */
/*========================================================================*/

#include <cstdio>
#include <bundle.h>

#include "ico_log.h"
#include "ico_syc_public.h"
#include "CicoHSCmdOpts.h"

CicoHSCmdOpts* CicoHSCmdOpts::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSCmdOpts::CicoHSCmdOpts()
    : m_loginUser(""), m_historyFilePath(""), m_defaultHistoryFilePath(""),
      m_flagPath("")
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSCmdOpts::~CicoHSCmdOpts()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  get this class instance
 */
//--------------------------------------------------------------------------
CicoHSCmdOpts*
CicoHSCmdOpts::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoHSCmdOpts();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief  parse command options
 */
//--------------------------------------------------------------------------
void
CicoHSCmdOpts::parse(int argc, char *argv[])
{
    ICO_TRA("CicoHSCmdOpts::parse Enter");
    /* get LOGIN-USER parameter */
    bundle *b = bundle_import_from_argv(argc, argv); // import from argc+argv
    const char* valusr = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY1);
    const char* valpath = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY2);
    const char* valpathD = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY3);
    const char* valFlagPath = bundle_get_val(b, ICO_SYC_APP_BUNDLE_KEY4);
    if ((NULL != valusr) && (0 != valusr)) {
        m_loginUser = valusr;
    }
    else {
        m_loginUser.clear();
    }

    if ((NULL != valpath) && (0 != valpath)) {
        m_historyFilePath = valpath;
    }
    else {
        m_historyFilePath.clear();
    }

    if ((NULL != valpathD) && (0 != valpathD)) {
        m_defaultHistoryFilePath = valpathD;
    }
    else {
        m_defaultHistoryFilePath.clear();
    }
    std::string flagPath;
    if ((NULL != valFlagPath) && (0 != valFlagPath)) {
        m_flagPath = valFlagPath;
    }
    else {
        flagPath.clear();
    }
    ICO_DBG("PARAM=\"%s\", \"%s\", \"%s\", \"%s\"",
            m_loginUser.c_str(), m_historyFilePath.c_str(),
            m_defaultHistoryFilePath.c_str(), m_flagPath.c_str());
    bundle_free(b);
    valusr = valpath = valpathD = valFlagPath = NULL;
    ICO_TRA("CicoHSCmdOpts::parse Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  get login user name
 *
 *  @return long user name
 */
//--------------------------------------------------------------------------
const std::string &
CicoHSCmdOpts::getLoginUser(void)
{
    return m_loginUser;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get history file path
 *
 *  @return history file path
 */
//--------------------------------------------------------------------------
const std::string & 
CicoHSCmdOpts::getHistoryFilePath(void)
{
    return m_historyFilePath;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application default history file path
 *
 *  @return default history file path
 */
//--------------------------------------------------------------------------
const std::string &
CicoHSCmdOpts::getDefaultHistoryFilePath(void)
{
    return m_defaultHistoryFilePath;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application history file flag
 *
 *  @return application history file flag
 */
//--------------------------------------------------------------------------
const std::string &
CicoHSCmdOpts::getFlagPath(void)
{
    return m_flagPath;
}
// vim:set expandtab ts=4 sw=4:
