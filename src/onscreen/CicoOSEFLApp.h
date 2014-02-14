/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoOSEFLApp.h
 *
 *  @brief  This file is definition of CicoOSEFLApp class
 */
//==========================================================================
#ifndef __CICO_OS_EFL_APP_H__
#define __CICO_OS_EFL_APP_H__

#include "CicoEFLApp.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoOnScreen;

//==========================================================================
/**
 *  @brief  ELF Application functions for OnScreen
 */
//==========================================================================
class CicoOSEFLApp : public CicoEFLApp {
public:
    // default constructor
    CicoOSEFLApp();

    // destructor
    virtual ~CicoOSEFLApp();

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
    CicoOSEFLApp& operator=(const CicoOSEFLApp &object);

    // copy constructor
    CicoOSEFLApp(const CicoOSEFLApp &object);

private:
    // CicoOnScreen instance
    CicoOnScreen* m_onscreen;
};
#endif  // __CICO_OS_EFL_APP_H__
// vim:set expandtab ts=4 sw=4:
