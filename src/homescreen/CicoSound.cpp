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

#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreenResourceConfig.h"
#include "CicoHomeScreenConfig.h"

#include "CicoSound.h"


CicoSound *CicoSound::m_myInstance = NULL;

/*--------------------------------------------------------------------------*/
/**
 *  @brief  default constructor
 */
/*--------------------------------------------------------------------------*/
CicoSound::CicoSound()
    : m_initialized(false),
      m_command(ICO_SOUND_DEFAULT_COMMAND),
      m_operationSFile(ICO_SOUND_DEFAULT_OPERATION),
      m_successSFile(ICO_SOUND_DEFAULT_SUCCESS),
      m_failureSFile(ICO_SOUND_DEFAULT_FAILURE)
{
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
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
//--------------------------------------------------------------------------
int
CicoSound::Initialize(CicoHomeScreenConfig *hsConfig)
{
    if (true == m_initialized) {
        return ICO_HS_OK;
    }

    m_command = hsConfig->ConfigGetString(ICO_CONFIG_SOUND,
                                          ICO_CONFIG_COMMAND,
                                          ICO_SOUND_DEFAULT_COMMAND);

    m_operationSFile = hsConfig->ConfigGetString(ICO_CONFIG_SOUND,
                                                 ICO_CONFIG_OPERATION,
                                                 ICO_SOUND_DEFAULT_OPERATION);

    m_successSFile = hsConfig->ConfigGetString(ICO_CONFIG_SOUND,
                                               ICO_CONFIG_SUCCESS,
                                               ICO_SOUND_DEFAULT_SUCCESS);

    m_failureSFile = hsConfig->ConfigGetString(ICO_CONFIG_SOUND,
                                               ICO_CONFIG_FAILURE,
                                               ICO_SOUND_DEFAULT_FAILURE);

    ICO_DBG("CicoSound(command=%s operation=%s success=%s failure=%s",
            m_command.c_str(), m_operationSFile.c_str(),
            m_successSFile.c_str(), m_failureSFile.c_str())

    m_initialized = true;

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlayOperationSound
 *          play the given sound file(.wav).
 *
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
CicoSound::PlayOperationSound(void)
{
    return PlaySound(m_operationSFile);
 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlaySuccessSound
 *
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
CicoSound::PlaySuccessSound(void)
{
    return PlaySound(m_successSFile);
 
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoSound::PlaySuccessSound
 *
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
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
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
int
CicoSound::PlaySound(std::string & soundFile)
{
    int pid;
    char command[ICO_HS_TEMP_BUF_SIZE];
 
    if (false == m_initialized) {
        return ICO_HS_ERR;
    }

    if (true == m_operationSFile.empty()) {
        ICO_WRN("CicoSound::PlaySound: Leave(error file name is empty)");
        return ICO_HS_ERR;
    }

    if (m_operationSFile.at(0) != '/') {
        ICO_WRN("CicoSound::PlaySound: Leave(file name not absolute path");
        return ICO_HS_ERR;
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

    return ICO_HS_OK;
}
// vim: set expandtab ts=4 sw=4:
