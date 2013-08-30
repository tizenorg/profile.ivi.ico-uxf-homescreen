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
 *  @file   CicoSysConDaemon.h
 *
 *  @brief  
 */
/*========================================================================*/    

//#include <app.h>
#include <stdio.h>
#include "CicoEFLApp.h"

#ifndef __CICO_UI_SAMPLE_APP_H__
#define __CICO_UI_SAMPLE_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

class CicoSysConDaemon : public CicoEFLApp {
public:
    /**
     *  Default Constructor
     */
    CicoSysConDaemon();
    
    /**
     *  Destructor
     */
    virtual ~CicoSysConDaemon();

    /**
     *
     */
    virtual bool onCreate(void *user_data);
#if 0
    void onTerminate(void *user_data);
    void onPause(void *user_data);
    void onResume(void *user_data);
    void onService(service_h service, void *user_data);
#endif

protected:
    /**
     *  Assignment Operator
     */
    CicoSysConDaemon& operator=(const CicoSysConDaemon &object);

    /**
     *  Copy Constructor
     */
    CicoSysConDaemon(const CicoSysConDaemon &object);
};

#ifdef __cplusplus
}
#endif

#endif  /* __CICO_UI_SAMPLE_APP_H__ */
/* vim:set expandtab ts=4 sw=4: */
