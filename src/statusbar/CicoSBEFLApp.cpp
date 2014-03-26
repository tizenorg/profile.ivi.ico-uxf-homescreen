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
 *  @file   CicoSBEFLApp.cpp
 *
 *  @brief  This file is implimention of CicoSBEFLApp class
 */
//==========================================================================

#include <exception>
#include <iostream>
#include <string>

#include <ico_log.h>

#include "CicoSBEFLApp.h"
#include "CicoStatusBar.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSBEFLApp::CicoSBEFLApp()
{
    ICO_TRA("CicoSBEFLApp::CicoSBEFLApp Enter");
    m_statusbar = NULL;
    ICO_TRA("CicoSBEFLApp::CicoSBEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSBEFLApp::~CicoSBEFLApp()
{
    ICO_TRA("CicoSBEFLApp::~CicoSBEFLApp Enter");
    ICO_TRA("CicoSBEFLApp::~CicoSBEFLApp Leave");
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
CicoSBEFLApp::onCreate(void *user_data)
{
    ICO_TRA("CicoSBEFLApp::onCreate Enter");

    try {
        // start status bar
        m_statusbar = new CicoStatusBar();
        bool ret = m_statusbar->Initialize();
        if (false == ret) {
            ICO_TRA("CicoSBEFLApp::onCreate Leave(false)");
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        ICO_ERR("catch exception %s", e.what());
        ICO_TRA("CicoSBEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (const std::string& str) {
        std::cerr << str << std::endl;
        ICO_ERR("catch exception %s", str.c_str());
        ICO_TRA("CicoSBEFLApp::onCreate Leave(false)");
        return false;
    }
    catch (...) {
        ICO_ERR("catch exception unknown");
        ICO_TRA("CicoSBEFLApp::onCreate Leave(false)");
        return false;
    }

    ICO_TRA("CicoSBEFLApp::onCreate Leave(true)");
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
CicoSBEFLApp::onTerminate(void *user_data)
{
    ICO_TRA("CicoSBEFLApp::onTerminate Enter");
    delete m_statusbar;
    ICO_TRA("CicoSBEFLApp::onTerminate Leave");
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
CicoSBEFLApp::onPause(void *user_data)
{
    ICO_TRA("CicoSBEFLApp::onPause Enter");
    ICO_TRA("CicoSBEFLApp::onPause Leave");
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
CicoSBEFLApp::onResume(void *user_data)
{
    ICO_TRA("CicoSBEFLApp::onResume Enter");
    ICO_TRA("CicoSBEFLApp::onResume Leave");
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
CicoSBEFLApp::onService(service_h service, void *user_data)
{
    ICO_TRA("CicoSBEFLApp::onService Enter");
    ICO_TRA("CicoSBEFLApp::onService Leave");
}
// vim: set expandtab ts=4 sw=4:
