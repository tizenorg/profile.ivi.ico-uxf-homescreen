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
 *  @file   CicoHSEFLApp.h
 *
 *  @brief  This file is definition of CicoHSEFLApp class
 */
//==========================================================================
#ifndef __CICO_HS_EFL_APP_H__
#define __CICO_HS_EFL_APP_H__

#include "CicoEFLApp.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoHomeScreen;

//==========================================================================
/**
 *  @brief  ELF Application functions for HomeScreen
 */
//==========================================================================
class CicoHSEFLApp : public CicoEFLApp {
public:
    // default constructor
    CicoHSEFLApp();
    
    // destructor
    virtual ~CicoHSEFLApp();

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
    CicoHSEFLApp& operator=(const CicoHSEFLApp &object);

    // copy constructor
    CicoHSEFLApp(const CicoHSEFLApp &object);

private:
    // CicoHomeScreen instance
    CicoHomeScreen* m_homescreen;
};
#endif  // __CICO_HS_EFL_APP_H__
// vim:set expandtab ts=4 sw=4:
