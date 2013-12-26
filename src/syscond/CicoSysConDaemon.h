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
 *  @file   CicoSysConDaemon.h
 *
 *  @brief  This file is definition of CicoSysConDaemon class
 */
//==========================================================================
#ifndef __CICO_SYS_CON_DAEMON_H__
#define __CICO_SYS_CON_DAEMON_H__

#include <stdio.h>
#include "CicoEFLApp.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCWindowController;
class CicoSCInputController;
class CicoSCResourceManager;

//==========================================================================
/**
 *  @brief  ELF Application functions for SystemController
 */
//==========================================================================
class CicoSysConDaemon : public CicoEFLApp {
public:
    // default constructor
    CicoSysConDaemon();

    // destructor
    virtual ~CicoSysConDaemon();

    // callback function on create
    virtual bool onCreate(void *user_data);

    // callback function on terminate
    virtual void onTerminate(void *user_data);

    // callback function on pause
    virtual void onPause(void *user_data);

    // callback function on resume
    virtual void onResume(void *user_data);

    // callback function on service
    virtual void onService(service_h service, void *user_data);

protected:
    // assignment operator
    CicoSysConDaemon& operator=(const CicoSysConDaemon &object);

    // copy constructor
    CicoSysConDaemon(const CicoSysConDaemon &object);

private:
    // CicoSCWindowController instance
    CicoSCWindowController* m_winctrl;

    // CicoSCInputController instance
    CicoSCInputController*  m_inputctrl;

    // CicoSCResourceManager instance
    CicoSCResourceManager*  m_resourcemgr;
};
#endif  // __CICO_SYS_CON_DAEMON_H__
// vim:set expandtab ts=4 sw=4:
