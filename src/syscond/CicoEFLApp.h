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
 *  @file   CicoEFLApp.h
 *
 *  @brief  
 */
/*========================================================================*/    
#ifndef __CICO_EFL_APP_H__
#define __CICO_EFL_APP_H__

#include <app.h>

class CicoEFLApp {
public:
    //  default constructor
    CicoEFLApp();
 
    /**
     *  Destructor
     */
    virtual ~CicoEFLApp();

    /**
     *  Start Main Loop
     */ int start(int argc, char **argv);

    /**
     *  Stop Main Loop
     */
    int stop(void);

    /**
     *  Convert application error to string
     */
    const char* app_error_to_string(int error);

    virtual bool onCreate(void *user_data);
    virtual void onTerminate(void *user_data);
    virtual void onPause(void *user_data);
    virtual void onResume(void *user_data);
    virtual void onService(service_h service, void *user_data);
    virtual void onLowMemory(void *user_data);
    virtual void onLowBattery(void *user_data);
    virtual void onDeviceOrientation(app_device_orientation_e orientation,
                                     void *user_data);
    virtual void onLanguageChanged(void *user_data);
    virtual void onRegionFormatChanged(void *user_data);

protected:
    //  assignment operator
    CicoEFLApp& operator=(const CicoEFLApp &object);

    //  copy constructor
    CicoEFLApp(const CicoEFLApp &object);

private:
    static bool app_create(void *user_data);
    static void app_terminate(void *user_data);
    static void app_pause(void *user_data);
    static void app_resume(void *user_data);
    static void app_service(service_h service, void *user_data);
    static void app_low_memory(void *user_data);
    static void app_low_battery(void *user_data);
    static void app_device_orientation(app_device_orientation_e orientation,
                                       void *user_data);
    static void app_language_changed(void *user_data);
    static void app_region_format_changed(void *user_data);

private:
    //  application callbacks
    static app_event_callback_s m_appEventCB;
};
#endif  /* __CICO_EFL_APP_H__ */
/* vim:set expandtab ts=4 sw=4: */
