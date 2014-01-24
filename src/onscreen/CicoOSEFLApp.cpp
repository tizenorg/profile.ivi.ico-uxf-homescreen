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
 *  @file   CicoOSEFLApp.cpp
 *
 *  @brief  This file is implimention of CicoOSEFLApp class
 */
//==========================================================================

#include <exception>
#include <iostream>
#include <string>

#include "CicoOSEFLApp.h"
#include "CicoOnScreen.h"
#include "ico_log.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoOSEFLApp::CicoOSEFLApp()
{
//    ICO_TRA("CicoOSEFLApp::CicoOSEFLApp Enter");
//    ICO_TRA("CicoOSEFLApp::CicoOSEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoOSEFLApp::~CicoOSEFLApp()
{
//    ICO_TRA("CicoOSEFLApp::~CicoOSEFLApp Enter");
//    ICO_TRA("CicoOSEFLApp::~CicoOSEFLApp Leave");
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
CicoOSEFLApp::onCreate(void *user_data)
{
    ICO_TRA("CicoOSEFLApp::onCreate Enter");

    try {
#if 0
        // load system config
        CicoSystemConfig::getInstance()->load(
                ICO_OS_LIFECYCLE_CONTROLLER_SETTING_PATH);
#endif
        // start onscreen
        m_onscreen = new CicoOnScreen();
        bool ret = m_onscreen->StartOnScreen(ICO_ORIENTATION_VERTICAL);
        if(ret != true){
            ICO_TRA("CicoOSEFLApp::onCreate Leave(false)");
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("catch exception %s", e.what());
        ICO_TRA("CicoOSEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("catch exception %s", str.c_str());
        ICO_TRA("CicoOSEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        ICO_TRA("CicoOSEFLApp::onCreate Leave(false)");
        return false;
    }

    ICO_TRA("CicoOSEFLApp::onCreate Leave(true)");
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
CicoOSEFLApp::onTerminate(void *user_data)
{
    ICO_TRA("CicoOSEFLApp::onTerminate Enter");

    m_onscreen->Finalize();
    delete m_onscreen;

    ICO_TRA("CicoOSEFLApp::onTerminate Leave");
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
CicoOSEFLApp::onPause(void *user_data)
{
    ICO_TRA("CicoOSEFLApp::onPause Enter");
    ICO_TRA("CicoOSEFLApp::onPause Leave");
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
CicoOSEFLApp::onResume(void *user_data)
{
    ICO_TRA("CicoOSEFLApp::onResume Enter");
    ICO_TRA("CicoOSEFLApp::onResume Leave");
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
CicoOSEFLApp::onService(service_h service, void *user_data)
{
    ICO_TRA("CicoOSEFLApp::onService Enter");
    ICO_TRA("CicoOSEFLApp::onService Leave");
}
// vim: set expandtab ts=4 sw=4:
