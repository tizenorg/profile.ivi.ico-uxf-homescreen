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
 *  @file   CicoEFLApp.h
 *
 *  @brief  This file is definition of CicoEFLApp class
 */
//==========================================================================
#ifndef __CICO_EFL_APP_H__
#define __CICO_EFL_APP_H__

#include <app.h>

#include <string>

//--------------------------------------------------------------------------
/**
 *  @class  CicoGeometry
 *
 *  @brief  This class provide tizen appfw funciton
 */
//--------------------------------------------------------------------------
class CicoEFLApp {
public:
    // default constructor
    CicoEFLApp();
 
    // destructor
    virtual ~CicoEFLApp();

    // start main loop
    int start(int argc, char **argv);

    // stop main loop
    void stop(void);

    // get application package name
    std::string & getPackageName(void);

    // get application id
    std::string & getAppId(void);

    // get application version
    std::string & getVersion(void);

    // convert application error to string
    const char* appfwErrorToString(int error);

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

    // callback function on low memory
    virtual void onLowMemory(void *user_data);

    // callback function on low battery
    virtual void onLowBattery(void *user_data);

    // callback function on device orientation
    virtual void onDeviceOrientation(app_device_orientation_e orientation,
                                     void *user_data);

    // callback function on language changed
    virtual void onLanguageChanged(void *user_data);

    // callback function on region format changed
    virtual void onRegionFormatChanged(void *user_data);

protected:
    // assignment operator
    CicoEFLApp& operator=(const CicoEFLApp &object);

    // copy constructor
    CicoEFLApp(const CicoEFLApp &object);

private:
    // callback function on create for tizen appfw
    static bool appfwCreateCB(void *user_data);

    // callback function on terminate for tizen appfw
    static void appfwTerminateCB(void *user_data);

    // callback function on pause for tizen appfw
    static void appfwPauseCB(void *user_data);

    // callback function on resume for tizen appfw
    static void appfwResumeCB(void *user_data);

    // callback function on service for tizen appfw
    static void appfwServiceCB(service_h service, void *user_data);

    // callback function on low memory for tizen appfw
    static void appfwLowMemoryCB(void *user_data);

    // callback function on low battery for tizen appfw
    static void appfwLowBatteryCB(void *user_data);

    // callback function on device orientation for tizen appfw
    static void appfwDeviceOrientationCB(app_device_orientation_e orientation,
                                       void *user_data);

    // callback function on language changed for tizen appfw
    static void appfwLanguageChangedCB(void *user_data);

    // callback function on region format changed for tizen appfw
    static void appfwRegionFormatChangedCB(void *user_data);

private:
    // application package name
    std::string m_pkgName;

    // application id
    std::string m_appid;

    // application version
    std::string m_version;

    // application callbacks
    app_event_callback_s m_appEventCB;
};
#endif  //__CICO_EFL_APP_H__
// vim:set expandtab ts=4 sw=4:
