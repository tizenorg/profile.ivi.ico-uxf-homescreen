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
 *  @file   CicoHSAppInfo.h
 *
 *  @brief  This file is definition of CicoHSAppInfo class
 */
//==========================================================================
#ifndef __CICO_HS_APP_INFO_H__
#define __CICO_HS_APP_INFO_H__

#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>
#include <string.h>
#include <vector>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include "CicoHomeScreenCommon.h"
#include "CicoSCLifeCycleController.h"

#define ICO_HS_APPLICATION_FULL_SCREEN_POS_X 0
#define ICO_HS_APPLICATION_FULL_SCREEN_POS_Y 64
#define ICO_HS_APPLICATION_FULL_SCREEN_WIDTH 1080
#define ICO_HS_APPLICATION_FULL_SCREEN_HEIGHT 1728

#define ICO_HS_MAX_WINDOW_NAME 64
#define ICO_HS_MAX_ZONE_NAME 64

typedef struct _ico_hs_window_info{
    bool valid;
    char appid[ICO_HS_MAX_PROCESS_NAME];
    char name[ICO_HS_MAX_WINDOW_NAME];
    char zone[ICO_HS_MAX_ZONE_NAME];
    int  surface;
    int  nodeid;
    int  layer;
    int  pos_x;
    int  pos_y;
    int  width;
    int  height;
    int  raise;
    int  visible;
    int  active;
}ico_hs_window_info;

//--------------------------------------------------------------------------
/**
 *  @brief  
 */
//--------------------------------------------------------------------------
class CicoHSAppInfo
{
public:
    // constructor
    CicoHSAppInfo(const char *appid);

    // destructor
    ~CicoHSAppInfo(void);

    // add window information
    int AddWindowInfo(ico_syc_win_info_t *wininfo);

    // add window attribute
    int AddWindowAttr(ico_syc_win_attr_t *winattr);

    // free window information
    void FreeWindowInfo(const char *name);

    // get application id
    const char * GetAppId(void);

    // get tatal count window information
    int GetWindowNum(void);

    // get window information by window name
    ico_hs_window_info* GetWindowInfo(const char* name);

    // get window iformation by index
    ico_hs_window_info* GetWindowInfo(int idx);

    // get window information by surface id
    ico_hs_window_info* GetWindowInfobySurface(int surface);

    // launch application
    int Execute(void);

    // terminate application
    int Terminate(void);

    // get application running status
    bool GetStatus(void);
   
    // set last surface id
    void SetLastSurface(int last_surface);

    // get last surface id
    int GetLastSurface(void);

private:
    // default constructor
    CicoHSAppInfo();

    // assignment operator
    CicoHSAppInfo operator=(const CicoHSAppInfo&);

    // copy constructor
    CicoHSAppInfo(const CicoHSAppInfo&);

    // set window information
    void SetWindowInfo(ico_hs_window_info *hs_wininfo,
                       ico_syc_win_info_t *wininfo);

    // set window attribute
    void SetWindowAttr(ico_hs_window_info *hs_wininfo,
                       ico_syc_win_attr_t *winattr);

    // get ico_hs_window_info from freebuffer
    ico_hs_window_info* GetFreeWindowInfoBuffer(void);

private:
    /// application id
    std::string m_appid;
    /// application category
    int m_category;
    /// total window count
    int m_window_num;
    /// last surface id
    int m_last_surface;
    /// window information free list
    ico_hs_window_info m_window_info_i[ICO_HS_APP_MAX_WINDOW_NUM];
    /// window information list
    std::vector<ico_hs_window_info *> m_window_info;
    /// life cycle controller instance
    CicoSCLifeCycleController *m_life_cycle_controller;
};
#endif  // __CICO_HS_APP_INFO_H__
// vim: set expandtab ts=4 sw=4:
