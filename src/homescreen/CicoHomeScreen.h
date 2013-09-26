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
#include "CicoHSFlickTouch.h"
#include "CicoHSWindowController.h"
#include "CicoHSAppInfo.h"

#include "CicoSCSystemConfig.h"
#include "CicoSCLifeCycleController.h"
#include "CicoHSAppHistoryExt.h"

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

#define ICO_HS_CHANGE_ZONE_MAX  10

class CicoHomeScreen
{
  public:
    CicoHomeScreen(void);
    ~CicoHomeScreen(void);
    int Initialize(int orientation,CicoHomeScreenConfig *config);
    void InitializeAppHistory(const std::string& user, const std::string& path,
                              const std::string& flagpath);
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
    void CreateFlickInputWindow(void);
    void DeleteFlickInputWindow(void);
    void StartLoop(void);
    char *GetHsPackageName(void);
    char *GetSbPackageName(void);
    char *GetOsPackageName(void);
    void ShowHomeScreenLayer(void);
    void ShowHomeScreenWindow(ico_syc_win_info_t *win_info);
    void ShowStatusBarWindow(ico_syc_win_info_t *win_info);
    void ShowApplicationWindow(ico_syc_win_info_t *win_info);
    void RaiseApplicationWindow(const char *appid,int surface);
    static void ChangeMode(int pattern);
    static void ExecuteApp(const char *appid);
    static void TerminateApp(const char *appid);
    static bool GetAppStatus(const char *appid);
    void ChangeActive(const char *appid, int surface);
    static void ChangeZone(void);
    static CicoHSAppInfo *GetAppInfo(const char *appid);
    void SetMode(int mode);
    int GetMode(void);
    void StartHomeScreen();
    void UpdateTile(const char *appid);
    static void RenewAppInfoList(void);

    void setActiveApp(const char* appid);
    // update current active application information
    void SetActiveAppInfo(const char *appid);
    CicoHSAppInfo* GetActiveAppInfo(void);

    // update current sub displaye applicatin information
    void SetSubDisplayAppInfo(const char *appid);
    CicoHSAppInfo* GetSubDisplayAppInfo(void);
    // order of the start-up window
    void startupCheckAdd(int pid, const std::string& appid);
    void startupCheck(const char* appid);

  private:
    int GetProcessWindow(const char *appid);
    static void EventCallBack(ico_syc_ev_e event,const void* detail,void* user_data);
    void ExecuteApp_i(const char *appid);
    void TerminateApp_i(const char *appid);
    void CreateAppInfoList(void);
    ico_hs_window_info *GetWindowInfo(CicoHSAppInfo* appinfo,const char *window);
    ico_hs_window_info *GetWindowInfo(CicoHSAppInfo* appinfo,int surface);

    static void SetNightMode(void* data);
    static void SetRegulation(void* data);

    void RenewAppInfoList_i(void);

    /*application control(do not use now)*/
    int application_num;
    CicoHSAppInfo *apps_info[ICO_HS_MAX_APP_NUM];
    // current active application information
    CicoHSAppInfo *active_appinfo;
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
    int num_flick_input_windows;
    CicoHSFlickInputWindow* flick_input_windows[ICO_HS_MAX_FLICKWINDOWS];
    /*mode*/
    int mode;
    /*configuration*/
    CicoHomeScreenConfig *config;
    /*my instance for callback*/
    static CicoHomeScreen *hs_instance;

    CicoSCLifeCycleController* life_cycle_controller;
    CicoHSAppHistoryExt* m_appHis;

    // current sub display appinfo
    CicoHSAppInfo *sub_display_appinfo;

  public:
    // swith trigger zone rotation list
    const char* moveZoneName;

    // move zone animation information
    ico_syc_animation_t moveZoneAnimation;


  protected:
    CicoHomeScreen operator=(const CicoHomeScreen&);
    CicoHomeScreen(const CicoHomeScreen&);
};

#endif
