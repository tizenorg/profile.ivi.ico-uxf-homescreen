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
 *  @file   CicoSBEFLApp.h
 *
 *  @brief  This file is definition of CicoSBEFLApp class
 */
//==========================================================================
#ifndef __CICO_SB_EFL_APP_H__
#define __CICO_SB_EFL_APP_H__

#include "CicoEFLApp.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoStatusBar;

//==========================================================================
/**
 *  @brief  ELF Application functions for HomeScreen
 */
//==========================================================================
class CicoSBEFLApp : public CicoEFLApp {
public:
    // default constructor
    CicoSBEFLApp();

    // destructor
    virtual ~CicoSBEFLApp();

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
    CicoSBEFLApp& operator=(const CicoSBEFLApp &object);

    // copy constructor
    CicoSBEFLApp(const CicoSBEFLApp &object);

private:
    // CicoHomeScreen instance
    CicoStatusBar* m_statusbar;
};
#endif  // __CICO_SB_EFL_APP_H__
// vim:set expandtab ts=4 sw=4:
