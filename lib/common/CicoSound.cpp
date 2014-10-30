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
 *  @file   CicoSound.h
 *
 *  @brief  This file is implimention of CicoSound class
 */
//==========================================================================
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <tzplatform_config.h>

#include <ico_log.h>
#include "CicoResourceConfig.h"
#include "CicoGKeyFileConfig.h"

#include "CicoSound.h"


CicoSound *CicoSound::m_myInstance = NULL;

/*--------------------------------------------------------------------------*/
/**
 *  @brief  default constructor
 */
/*--------------------------------------------------------------------------*/
CicoSound::CicoSound()
    : m_initialized(false),
      m_command(ICO_SOUND_DEFAULT_COMMAND)
{
    m_operationSFile = tzplatform_mkpath(TZ_SYS_RO_APP, ICO_SOUND_DEFAULT_OPERATION);
    m_successSFile = tzplatform_mkpath(TZ_SYS_RO_APP, ICO_SOUND_DEFAULT_SUCCESS);
    m_failureSFile = tzplatform_mkpath(TZ_SYS_RO_APP, ICO_SOUND_DEFAULT_FAILURE);
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  destructor
 */
/*--------------------------------------------------------------------------*/
CicoSound::~CicoSound()
{
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  get instance
 *
 *  @return CicoSound instance
 */
/*--------------------------------------------------------------------------*/
CicoSound *
CicoSound::GetInstance(void)
{
    if (NULL == m_myInstance) {
        m_myInstance = new CicoSound();
    }

    return m_myInstance;
}

//--------------------------------------------------------------------------
/**
 * @brief   Initialize
 *          initialize to sound a operation
 *
 * @param[in]   homescreen_config   configuration instance
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
//--------------------------------------------------------------------------
bool
CicoSound::Initialize(CicoGKeyFileConfig *hsConfig)
{
    char    wkbuf[256];
    if (true == m_initialized) {
        return true;
    }

    m_command = hsConfig->ConfigGetString(ICO_CONFIG_SOUND,
                                          ICO_CONFIG_COMMAND,
                                          ICO_SOUND_DEFAULT_COMMAND);

    if (hsConfig->ConfigGetString(ICO_CONFIG_SOUND, ICO_CONFIG_OPERATION, NULL,
                                  wkbuf, sizeof(wkbuf)) == 0)   {
        m_operationSFile = wkbuf;
    }
    if (hsConfig->ConfigGetString(ICO_CONFIG_SOUND, ICO_CONFIG_SUCCESS, NULL,
                                  wkbuf, sizeof(wkbuf)) == 0)   {
        m_successSFile = wkbuf;
    }
    if (hsConfig->ConfigGetString(ICO_CONFIG_SOUND, ICO_CONFIG_FAILURE, NULL,
                                  wkbuf, sizeof(wkbuf)) == 0)   {
        m_failureSFile = wkbuf;
    }

    ICO_DBG("CicoSound(command=%s operation=%s success=%s failure=%s",
            m_command.c_str(), m_operationSFile.c_str(),
            m_successSFile.c_str(), m_failureSFile.c_str())

    m_initialized = true;

    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlayOperationSound
 *          play the given sound file(.wav).
 *
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
/*--------------------------------------------------------------------------*/
bool
CicoSound::PlayOperationSound(void)
{
    return PlaySound(m_operationSFile);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlaySuccessSound
 *
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
/*--------------------------------------------------------------------------*/
bool
CicoSound::PlaySuccessSound(void)
{
    return PlaySound(m_successSFile);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlaySuccessSound
 *
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
/*--------------------------------------------------------------------------*/
bool
CicoSound::PlayFailureSound(void)
{
    return PlaySound(m_failureSFile);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlaySound
 *          play the given sound file(.wav).
 *
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
/*--------------------------------------------------------------------------*/
bool
CicoSound::PlaySound(std::string & soundFile)
{
    int pid;
    char command[256];

    if (false == m_initialized) {
        return false;
    }

    if (true == m_operationSFile.empty()) {
        ICO_WRN("CicoSound::PlaySound: Leave(error file name is empty)");
        return false;
    }

    if (m_operationSFile.at(0) != '/') {
        ICO_WRN("CicoSound::PlaySound: Leave(file name not absolute path");
        return false;
    }

    /* given by full file path */
    snprintf(command, sizeof(command), "%s %s",
             m_command.c_str(), soundFile.c_str());

    ICO_DBG("PlaySound:system(%s)", command);
    pid = fork();
    if (!pid) {
        system(command);
        exit(0);
    }

    return true;
}
// vim: set expandtab ts=4 sw=4:
