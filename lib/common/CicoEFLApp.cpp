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
 *  @file   CicoEFLApp.cpp
 *
 *  @brief  This file is implemetation of CicoEFLApp class
 */
//==========================================================================

#include <exception>
#include <string>
#include <Ecore.h>

#include <ico_log.h>

#include "CicoEFLApp.h"

//--------------------------------------------------------------------------
/**
 *  @brief  constructor
 */
//--------------------------------------------------------------------------
CicoEFLApp::CicoEFLApp()
    : m_pkgName(""), m_appid(""), m_version("")
{
//    ICO_DBG("CicoEFLApp::CicoEFLApp Enter");
    m_appEventCB.create                = appfwCreateCB;
    m_appEventCB.terminate             = appfwTerminateCB;
    m_appEventCB.pause                 = appfwPauseCB;
    m_appEventCB.resume                = appfwResumeCB;
    m_appEventCB.service               = appfwServiceCB;
    m_appEventCB.low_memory            = appfwLowMemoryCB;
    m_appEventCB.low_battery           = appfwLowBatteryCB;
    m_appEventCB.device_orientation    = appfwDeviceOrientationCB;
    m_appEventCB.language_changed      = appfwLanguageChangedCB;
    m_appEventCB.region_format_changed = appfwRegionFormatChangedCB;

    char *value;
    int ret = app_get_package(&value);
    if (APP_ERROR_NONE == ret) {
        m_pkgName = value;
        free(value);
    }

    ret = app_get_id(&value);
    if (APP_ERROR_NONE == ret) {
        m_appid = value;
        free(value);
    }

#if 0   /* no need version number   */
    ret = app_get_version(&value);
    if (APP_ERROR_NONE == ret) {
        m_version = value;
        free(value);
    }
#endif
//    ICO_DBG("CicoEFLApp::CicoEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoEFLApp::~CicoEFLApp()
{
//    ICO_DBG("CicoEFLApp::~CicoEFLApp Enter");
//    ICO_DBG("CicoEFLApp::~CicoEFLApp Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  start EFL main loop
 *
 *  @param [in] argc    count of argument
 *  @param [in] argv    variables of argument
 */
//--------------------------------------------------------------------------
int
CicoEFLApp::start(int argc, char **argv)
{
    ICO_DBG("CicoEFLApp::start Enter");
    int ret = app_efl_main(&argc, &argv, &m_appEventCB, this);
    ICO_DBG("CicoEFLApp::start Leave(ret=%d)", ret);
    return ret;
}

//--------------------------------------------------------------------------
/**
 *  @brief  stop EFL main loop
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::stop(void)
{
    app_efl_exit();
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application package name
 *
 *  @return application package name
 */
//--------------------------------------------------------------------------
std::string &
CicoEFLApp::getPackageName(void)
{
    return m_pkgName;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application id
 *
 *  @return application id
 */
//--------------------------------------------------------------------------
std::string &
CicoEFLApp::getAppId(void)
{
    return m_appid;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get application version
 *
 *  @return application version
 */
//--------------------------------------------------------------------------
std::string &
CicoEFLApp::getVersion(void)
{
    return m_version;
}

//--------------------------------------------------------------------------
/**
 *  @brief  convart appfw error code to string
 *
 *  @param [in] error   code of error
 *
 *  @return string of error
 */
//--------------------------------------------------------------------------
const char*
CicoEFLApp::appfwErrorToString(int error)
{
    switch(error) {
    case APP_ERROR_NONE:
        return (const char*)"APP_ERROR_NONE";
        break;
    case APP_ERROR_INVALID_PARAMETER:
        return (const char*)"APP_ERROR_INVALID_PARAMETER";
        break;
    case APP_ERROR_OUT_OF_MEMORY:
        return (const char*)"APP_ERROR_OUT_OF_MEMORY";
        break;
    case APP_ERROR_INVALID_CONTEXT:
        return (const char*)"APP_ERROR_INVALID_CONTEXT";
        break;
    case APP_ERROR_NO_SUCH_FILE:
        return (const char*)"APP_ERROR_NO_SUCH_FILE";
        break;
    case APP_ERROR_ALREADY_RUNNING:
        return (const char*)"APP_ERROR_ALREADY_RUNNING";
        break;
    default:
        break;
    }

    return (const char*)"TIZEN_ERROR_UNKNOWN";
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
CicoEFLApp::onCreate(void *user_data)
{
    ICO_WRN("CicoEFLApp::onCreate called.");
    return false;
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
CicoEFLApp::onTerminate(void *user_data)
{
    ICO_WRN("CicoEFLApp::onTerminate called.");
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
CicoEFLApp::onPause(void *user_data)
{
    ICO_WRN("CicoEFLApp::onPause called.");
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
CicoEFLApp::onResume(void *user_data)
{
    ICO_WRN("CicoEFLApp::onResume called.");
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
CicoEFLApp::onService(service_h service, void *user_data)
{
    ICO_WRN("CicoEFLApp::onService called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on low memory
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::onLowMemory(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLowMemory called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on low battery
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::onLowBattery(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLowBattery called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on device orientation
 *
 *  @param [in] orientation The orientation of device
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::onDeviceOrientation(app_device_orientation_e orientation,
                                void *user_data)
{
    ICO_WRN("CicoEFLApp::onDeviceOrientation called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on language changed
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::onLanguageChanged(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLanguageChanged called.");
}

//--------------------------------------------------------------------------
/**
 *  @brief  callback function on region format changed
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::onRegionFormatChanged(void *user_data)
{
    ICO_WRN("CicoEFLApp::onRegionFormatChanged called.");
}

//==========================================================================
//  pravate functions
//==========================================================================
//--------------------------------------------------------------------------
/**
 *  @brief  Called at the start of the application.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 *
 *  @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoEFLApp::appfwCreateCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwCreateCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwCreateCB Leave");
        return false;
    }

    bool ret = static_cast<CicoEFLApp*>(user_data)->onCreate(user_data);

    ICO_DBG("CicoEFLApp::appfwCreateCB Leave(%sd)", ret ? "true" : "false");

    return ret;
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called once after the main loop of application exits.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwTerminateCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwTerminateCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwTerminateCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onTerminate(user_data);

    ICO_DBG("CicoEFLApp::appfwTerminateCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when the application is completely obscured by another
 *          application and becomes invisible.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwPauseCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwPauseCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwPauseCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onPause(user_data);

    ICO_DBG("CicoEFLApp::appfwPauseCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when the application becomes visible.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwResumeCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwResumeCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwResumeCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onResume(user_data);

    ICO_DBG("CicoEFLApp::appfwResumeCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when other application send the launch request to
 *          the application.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwServiceCB(service_h service, void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwServiceCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwServiceCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onService(service, user_data);

    ICO_DBG("CicoEFLApp::appfwServiceCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when the system memory is running low.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwLowMemoryCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwLowMemoryCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwLowMemoryCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onLowMemory(user_data);

    ICO_DBG("CicoEFLApp::appfwLowMemoryCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when the battery power is running low.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwLowBatteryCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwLowBatteryCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwLowBatteryCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onLowBattery(user_data);

    ICO_DBG("CicoEFLApp::appfwLowBatteryCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when the orientation of device changes.
 *
 *  @param [in] orientation The orientation of device
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwDeviceOrientationCB(app_device_orientation_e orientation,
                                     void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwDeviceOrientationCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwDeviceOrientationCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onDeviceOrientation(orientation,
                                                             user_data);

    ICO_DBG("CicoEFLApp::appfwDeviceOrientationCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when language setting changes.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwLanguageChangedCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwLanguageChangedCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwDeviceOrientationCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onLanguageChanged(user_data);

    ICO_DBG("CicoEFLApp::appfwLanguageChangedCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  Called when region format setting changes.
 *
 *  @param [in] user_data   The user data passed from the callback
 *                          registration function
 */
//--------------------------------------------------------------------------
void
CicoEFLApp::appfwRegionFormatChangedCB(void *user_data)
{
    ICO_DBG("CicoEFLApp::appfwRegionFormatChangedCB Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::appfwDeviceOrientationCB Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onRegionFormatChanged(user_data);

    ICO_DBG("CicoEFLApp::appfwRegionFormatChangedCB Leave");
}
// vim: set expandtab ts=4 sw=4;
