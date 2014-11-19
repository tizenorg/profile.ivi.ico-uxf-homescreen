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
 *  @file   CicoHSEFLApp.cpp
 *
 *  @brief  This file is implimention of CicoHSEFLApp class
 */
//==========================================================================

#include <exception>
#include <iostream>
#include <string>
#include <tzplatform_config.h>

#include "CicoHSEFLApp.h"
#include "CicoHomeScreen.h"
#include "CicoHSWindow.h"
#include "ico_log.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSEFLApp::CicoHSEFLApp()
{
//    ICO_TRA("CicoHSEFLApp::CicoHSEFLApp Enter");
//    ICO_TRA("CicoHSEFLApp::CicoHSEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSEFLApp::~CicoHSEFLApp()
{
//    ICO_TRA("CicoHSEFLApp::~CicoHSEFLApp Enter");
//    ICO_TRA("CicoHSEFLApp::~CicoHSEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on create
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 *
 *  @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoHSEFLApp::onCreate(void *user_data)
{
    ICO_TRA("CicoHSEFLApp::onCreate Enter");

    try {
        // load system config
        CicoSystemConfig::getInstance()->load(
                    tzplatform_mkpath(TZ_SYS_RO_APP,
                                      ICO_HS_LIFECYCLE_CONTROLLER_SETTING_PATH));
        // start homescreen
        m_homescreen = new CicoHomeScreen();
        int ret = m_homescreen->StartHomeScreen(ICO_ORIENTATION_VERTICAL);
        if(ret != ICO_OK){
            ICO_TRA("CicoHSEFLApp::onCreate Leave(false)");
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("catch exception %s", e.what());
        ICO_TRA("CicoHSEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("catch exception %s", str.c_str());
        ICO_TRA("CicoHSEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        ICO_TRA("CicoHSEFLApp::onCreate Leave(false)");
        return false;
    }

    ICO_TRA("CicoHSEFLApp::onCreate Leave(true)");
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on terminate
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoHSEFLApp::onTerminate(void *user_data)
{
    ICO_TRA("CicoHSEFLApp::onTerminate Enter");

    m_homescreen->Finalize();
    delete m_homescreen;

    ICO_TRA("CicoHSEFLApp::onTerminate Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on pause
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoHSEFLApp::onPause(void *user_data)
{
    ICO_TRA("CicoHSEFLApp::onPause Enter");
    ICO_TRA("CicoHSEFLApp::onPause Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on resume
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoHSEFLApp::onResume(void *user_data)
{
    ICO_TRA("CicoHSEFLApp::onResume Enter");
    ICO_TRA("CicoHSEFLApp::onResume Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on service
 *
 *  @param [in] service     The handle to the service
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoHSEFLApp::onService(service_h service, void *user_data)
{
    ICO_TRA("CicoHSEFLApp::onService Enter");
    ICO_TRA("CicoHSEFLApp::onService Leave");
}
// vim: set expandtab ts=4 sw=4:
