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
 *  @brief  This file is definition of CicoSound class
 */
//==========================================================================
#ifndef __CICO_SOUND_H__
#define __CICO_SOUND_H__

#include <string>

#define ICO_CONFIG_SOUND            "sound"
#define ICO_CONFIG_COMMAND          "command"
#define ICO_CONFIG_OPERATION        "operation"
#define ICO_CONFIG_SUCCESS          "success"
#define ICO_CONFIG_FAILURE          "failure"

#define ICO_SOUND_DEFAULT_COMMAND   "paplay --stream-name=HOMESCREEN_BEEP"
#define ICO_SOUND_DEFAULT_OPERATION "/usr/apps/org.tizen.ico.homescreen/res/sound/operation.wav"
#define ICO_SOUND_DEFAULT_SUCCESS   "/usr/apps/org.tizen.ico.homescreen/res/sound/success.wav"
#define ICO_SOUND_DEFAULT_FAILURE   "/usr/apps/org.tizen.ico.homescreen/res/sound/failure.wav"

//--------------------------------------------------------------------------
/**
 *  @brief  sound of operation function
 */
//--------------------------------------------------------------------------
class CicoSound
{
public:
    // get singleton instance
    static CicoSound* GetInstance(void);

    // initialized
    int Initialize(CicoHomeScreenConfig *hs_config);

    // play sound
    int PlaySound(std::string & soundFile);

    // play opreration sound
    int PlayOperationSound(void);

    // play success sound
    int PlaySuccessSound(void);

    // play failure sound
    int PlayFailureSound(void);

protected:
    // default constructor
    CicoSound();

    // destructor
    ~CicoSound();

    // assignment operator
    CicoSound operator=(const CicoSound&);

    // copy constructor
    CicoSound(const CicoSound&);

private:
    // my instance
    static CicoSound *m_myInstance;

    // initialiezed flag
    bool m_initialized;

    // play sound command
    std::string m_command;

    // operation sound file
    std::string m_operationSFile;

    // success sound file
    std::string m_successSFile;

    // failure sound file
    std::string m_failureSFile;
};
#endif  //__CICO_SOUND_H__
// vim: set expandtab ts=4 sw=4:
