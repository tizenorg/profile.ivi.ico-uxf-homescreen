/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application info
 *
 * @date    Aug-08-2013
 */
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

typedef struct _ico_hs_window_info{
    bool valid;
    bool showed;
    char appid[ICO_HS_MAX_PROCESS_NAME];
    char name[ICO_HS_MAX_WINDOW_NAME];
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

class CicoHSAppInfo
{
  public:
    CicoHSAppInfo(const char *appid);
    ~CicoHSAppInfo(void);
    char *GetAppId(void);
    ico_hs_window_info* GetWindowInfo(const char* name);
    void SetWindowInfo(ico_hs_window_info *hs_window_info,
                       ico_syc_win_info_t *window_info);
    void SetWindowAttr(ico_hs_window_info *hs_window_info,
                       ico_syc_win_attr_t *window_info);
    void FreeWindowInfo(const char *name);
    int AddWindowInfo(ico_syc_win_info_t *window_info);
    int AddWindowAttr(ico_syc_win_attr_t *window_info);
    int GetWindowNum(void);
    void SetShowed(ico_hs_window_info *info,bool showed);
    bool GetShowed(ico_hs_window_info *info);
    
    int Execute(void);
    int Terminate(void);
    bool GetStatus(void);
   
    void SetLastSurface(int last_surface);
    int GetLastSurface(void);
  private:
    ico_hs_window_info *GetFreeWindowInfoBuffer(void);

    char appid[ICO_HS_MAX_PROCESS_NAME];
    int category;
    int window_num;
    int last_surface;
    ico_hs_window_info window_info_i[ICO_HS_APP_MAX_WINDOW_NUM];
    std::vector<ico_hs_window_info *> window_info;
    CicoSCLifeCycleController *life_cycle_controller;
   
  protected:
    CicoHSAppInfo operator=(const CicoHSAppInfo&);
    CicoHSAppInfo(const CicoHSAppInfo&);
};
#endif

