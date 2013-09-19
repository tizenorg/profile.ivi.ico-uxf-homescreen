/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Home Screen
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HOME_SCREEN_H__
#define __CICO_HOME_SCREEN_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>
#include <vector>
#include <string>

#include "ico_syc_common.h"
#include "ico_syc_winctl.h"

#include <unistd.h>

#include "CicoHomeScreenCommon.h"

#include "CicoHomeScreenConfig.h"
#include "CicoHSMenuWindow.h"
#include "CicoHSBackWindow.h"
#include "CicoHSControlBarWindow.h"
#include "CicoHSWindowController.h"
#include "CicoHSAppInfo.h"

#include "CicoSCSystemConfig.h"
#include "CicoSCLifeCycleController.h"

/* display position and size */
#define ICO_HS_WINDOW_POS_X 0
#define ICO_HS_WINDOW_POS_Y 0 
#define ICO_HS_STATUSBAR_WINDOW_HEIGHT 64
#define ICO_HS_MENU_WINDOW_POS_X ICO_HS_WINDOW_POS_X
#define ICO_HS_MENU_WINDOW_POS_Y ICO_HS_STATUSBAR_WINDOW_HEIGHT
#define ICO_HS_CONTROLBAR_WINDOW_POS_X ICO_HS_WINDOW_POS_X
#define ICO_HS_CONTROLBAR_WINDOW_HEIGHT 128

/*mode*/
#define ICO_HS_MODE_MENU 0
#define ICO_HS_MODE_APPLICATION 1

/*hide pattern*/
#define ICO_HS_SHOW_HIDE_PATTERN_SLIDE 0
#define ICO_HS_SHOW_HIDE_PATTERN_FADE 1

#define ICO_HS_CONFIG_HOMESCREEN    "homescreen"
#define ICO_HS_CONFIG_ONSCREEN      "onscreen"
#define ICO_HS_CONFIG_STATUBAR      "statusbar"
#define ICO_HS_CONFIG_SOUND         "sound"
#define ICO_HS_CONFIG_SB            "statusbar"
#define ICO_HS_CONFIG_ONS           "onscreen"
#define ICO_HS_APPID_DEFAULT_SB     "org.tizen.ico.statusbar"    /* default status bar appid */
#define ICO_HS_APPID_DEFAULT_ONS    "org.tizen.ico.onscreen"    /* default on screen appid */

#define ICO_HS_GROUP_SPECIAL        "menu"

#define ICO_HS_APP_STATUS_ERR -1

class CicoHomeScreen
{
  public:
    CicoHomeScreen(void);
    ~CicoHomeScreen(void);
    int Initialize(int orientation,CicoHomeScreenConfig *config);
    void Finalize(void);
    int StartRelations();
    void CreateMenuWindow(void);
    void DeleteMenuWindow(void);
    void UpDateMenuWindow(void);
    void CreateBackWindow(void);
    void DeleteBackWindow(void);
    void UpDateBackWindow(void);
    void CreateControlBarWindow(void);
    void DeleteControlBarWindow(void);
    void StartLoop(void);
    char *GetHsPackageName(void);
    char *GetSbPackageName(void);
    char *GetOsPackageName(void);
    void ShowHomeScreenLayer(void);
    void ShowHomeScreenWindow(ico_syc_win_attr_t *win_attr);
    void ShowStatusBarWindow(ico_syc_win_attr_t *win_attr);
    void ShowApplicationWindow(ico_syc_win_attr_t *win_attr);
    void RaiseApplicationWindow(const char *appid,int surface);
    static void ChangeMode(int pattern);
    static void ExecuteApp(const char *appid);
    static void TerminateApp(const char *appid);
    static bool GetAppStatus(const char *appid);
    void SetMode(int mode);
    int GetMode(void);
    void StartHomeScreen();
    void UpdateTile(const char *appid);

  private:
    int GetProcessWindow(const char *appid);
    static void EventCallBack(ico_syc_ev_e event,const void* detail,void* user_data);
    void ExecuteApp_i(const char *appid);
    void TerminateApp_i(const char *appid);
    void CreateAppInfoList(void);
    CicoHSAppInfo *GetAppInfo(const char *appid);
    ico_hs_window_info *GetWindowInfo(CicoHSAppInfo* appinfo,const char *window);

    /*application control(do not use now)*/
    int application_num;
    CicoHSAppInfo *apps_info[ICO_HS_MAX_APP_NUM];
    /*application info*/
    CicoHSAppInfo *hs_app_info;
    CicoHSAppInfo *sb_app_info;
    CicoHSAppInfo *os_app_info;
    /*sizes*/
    int full_width,full_height;
    int menu_width,menu_height;
    int bg_width,bg_height;
    int ctl_bar_width,ctl_bar_height;
    /*package names*/
    char hs_package_name[ICO_HS_MAX_PROCESS_NAME];
    char sb_package_name[ICO_HS_MAX_PROCESS_NAME];
    char os_package_name[ICO_HS_MAX_PROCESS_NAME];
    /*Window Instances*/
    CicoHSMenuWindow* menu_window;
    CicoHSBackWindow* back_window;
    CicoHSControlBarWindow* ctl_bar_window; 
    /*mode*/
    int mode;
    /*configuration*/
    CicoHomeScreenConfig *config;
    /*my instance for callback*/
    static CicoHomeScreen *hs_instance;

    CicoSCLifeCycleController* life_cycle_controller;

  protected:
    CicoHomeScreen operator=(const CicoHomeScreen&);
    CicoHomeScreen(const CicoHomeScreen&);
};

#endif