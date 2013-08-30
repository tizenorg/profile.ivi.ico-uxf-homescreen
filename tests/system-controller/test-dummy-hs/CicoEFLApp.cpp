/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

/*========================================================================*/    
/**
 *  @file   CicoEFLApp.cpp
 *
 *  @brief  
 */
/*========================================================================*/    

#include <exception>
#include <string>
#include "Ecore.h"
#include "CicoEFLApp.h"
#include "CicoLog.h"

/*========================================================================*/    
app_event_callback_s CicoEFLApp::m_appEventCB = {
    .create                = CicoEFLApp::app_create,
    .terminate             = CicoEFLApp::app_terminate,
    .pause                 = CicoEFLApp::app_pause,
    .resume                = CicoEFLApp::app_resume,
    .service               = CicoEFLApp::app_service,
    .low_memory            = CicoEFLApp::app_low_memory,
    .low_battery           = CicoEFLApp::app_low_battery,
    .device_orientation    = CicoEFLApp::app_device_orientation,
    .language_changed      = CicoEFLApp::app_language_changed,
    .region_format_changed = CicoEFLApp::app_region_format_changed
};

/*========================================================================*/    
/**
 *  Default Constructor
 */
CicoEFLApp::CicoEFLApp()
{
    ICO_DBG("CicoEFLApp::CicoEFLApp Enter");
    ICO_DBG("CicoEFLApp::CicoEFLApp Leave");
}

/**
 *  Destructor
 */
CicoEFLApp::~CicoEFLApp()
{
    ICO_DBG("CicoEFLApp::~CicoEFLApp Enter");
    ICO_DBG("CicoEFLApp::~CicoEFLApp Leave");
}

/**
 *  Start Main Loop
 */
int
CicoEFLApp::start(int argc, char **argv)
{
    ICO_DBG("CicoEFLApp::start Enter");
#if 0
    return app_efl_main(&argc, &argv, &m_appEventCB, this);
#else
    /* init ecore*/
    if (0 == ecore_init()) {
        ICO_DBG("CIcoUIApp::start Leave");
        return 0;
    }

    if (false== onCreate(NULL)) {
        ICO_DBG("CicoEFLApp::start Leave");
        return 0;
    }

    ecore_main_loop_begin();
    ICO_DBG("CicoEFLApp::start Leave");
    return 0;
#endif
    ICO_DBG("CicoEFLApp::start Leave");
}

/**
 *  Stop Main Loop
 */
int
CicoEFLApp::stop(void)
{
    return 0;
}

const char*
CicoEFLApp::app_error_to_string(int error)
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

bool
CicoEFLApp::onCreate(void *user_data)
{
    ICO_WRN("CicoEFLApp::onCreate called.");
    return false;
}

void
CicoEFLApp::onTerminate(void *user_data)
{
    ICO_WRN("CicoEFLApp::onTerminate called.");
}

void
CicoEFLApp::onPause(void *user_data)
{
    ICO_WRN("CicoEFLApp::onPause called.");
}

void
CicoEFLApp::onResume(void *user_data)
{
    ICO_WRN("CicoEFLApp::onResume called.");
}

void
CicoEFLApp::onService(service_h service, void *user_data)
{
    ICO_WRN("CicoEFLApp::onService called.");
}

void
CicoEFLApp::onLowMemory(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLowMemory called.");
}

void
CicoEFLApp::onLowBattery(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLowBattery called.");
}

void
CicoEFLApp::onDeviceOrientation(app_device_orientation_e orientation,
                                 void *user_data)
{
    ICO_WRN("CicoEFLApp::onDeviceOrientation called.");
}

void
CicoEFLApp::onLanguageChanged(void *user_data)
{
    ICO_WRN("CicoEFLApp::onLanguageChanged called.");
}

void
CicoEFLApp::onRegionFormatChanged(void *user_data)
{
    ICO_WRN("CicoEFLApp::onRegionFormatChanged called.");
}

bool
CicoEFLApp::app_create(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_create Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_create Leave");
        return false;
    }
    
    int ret = static_cast<CicoEFLApp*>(user_data)->onCreate(user_data); 

    ICO_DBG("CicoEFLApp::app_create Leave(%d)", ret);

    return ret;
}

void
CicoEFLApp::app_terminate(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_terminate Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_terminate Leave");
        return;
    }
    
    static_cast<CicoEFLApp*>(user_data)->onTerminate(user_data); 

    ICO_DBG("CicoEFLApp::app_terminate Leave");
}

void
CicoEFLApp::app_pause(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_pause Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_pause Leave");
        return;
    }
    
    static_cast<CicoEFLApp*>(user_data)->onPause(user_data); 

    ICO_DBG("CicoEFLApp::app_pause Leave");
}

void
CicoEFLApp::app_resume(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_resume Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_resume Leave");
        return;
    }
    
    static_cast<CicoEFLApp*>(user_data)->onResume(user_data); 

    ICO_DBG("CicoEFLApp::app_resume Leave");
}

void
CicoEFLApp::app_service(service_h service, void *user_data)
{
    ICO_DBG("CicoEFLApp::app_service Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_service Leave");
        return;
    }
    
    static_cast<CicoEFLApp*>(user_data)->onService(service, user_data); 

    ICO_DBG("CicoEFLApp::app_service Leave");
}

void
CicoEFLApp::app_low_memory(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_low_memory Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_low_memory Leave");
        return;
    }

    static_cast<CicoEFLApp*>(user_data)->onLowMemory(user_data); 
 
    ICO_DBG("CicoEFLApp::app_low_memory Leave");
}

void
CicoEFLApp::app_low_battery(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_low_battery Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_low_battery Leave");
        return;
    }
 
    static_cast<CicoEFLApp*>(user_data)->onLowBattery(user_data); 

    ICO_DBG("CicoEFLApp::app_low_battery Leave");
}

void
CicoEFLApp::app_device_orientation(app_device_orientation_e orientation,
                                   void *user_data)
{
    ICO_DBG("CicoEFLApp::app_device_orientation Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_device_orientation Leave");
        return;
    }
 
    static_cast<CicoEFLApp*>(user_data)->onDeviceOrientation(orientation,
                                                             user_data);

    ICO_DBG("CicoEFLApp::app_device_orientation Leave");
}

void
CicoEFLApp::app_language_changed(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_language_changed Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_device_orientation Leave");
        return;
    }
 
    static_cast<CicoEFLApp*>(user_data)->onLanguageChanged(user_data);

    ICO_DBG("CicoEFLApp::app_language_changed Leave");
}

void
CicoEFLApp::app_region_format_changed(void *user_data)
{
    ICO_DBG("CicoEFLApp::app_region_format_changed Enter");

    if (NULL == user_data) {
        ICO_DBG("CicoEFLApp::app_device_orientation Leave");
        return;
    }
 
    static_cast<CicoEFLApp*>(user_data)->onRegionFormatChanged(user_data);

    ICO_DBG("CicoEFLApp::app_region_format_changed Leave");
}
/* vim: set expandtab ts=4 sw=4: */
