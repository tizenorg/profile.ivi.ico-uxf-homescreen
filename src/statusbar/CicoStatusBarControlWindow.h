#ifndef __CICOSTATUSBARCONTROLWINDOW_H__
#define __CICOSTATUSBARCONTROLWINDOW_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   window control for statusbar application
 *
 * @date    Feb-15-2013
 */
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>


//#include "CicoHomeScreenCommon.h"
#include "CicoGKeyFileConfig.h"
#include "CicoStatusBarTime.h"

#define ICO_HS_CONFIG_ORIENTAION    "orientation"
#define ICO_HS_SIZE_SB_HEIGHT        (64)
#define ICO_HS_SIZE_LAYOUT_WIDTH     (768)
#define ICO_HS_SIZE_LAYOUT_HEIGHT    (1024)

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_SB_TIME_AMPM_THRESHOLD (12)
#define ICO_SB_TIME_AM        (10) /* f_name in sb_time_data */
#define ICO_SB_TIME_PM        (11) /* f_name in sb_time_data */
#define ICO_SB_TIME_IMG_PARTS (12) /* total counts of sb_time_data */
#define ICO_SB_TIMEOUT        (0.2)

#define ICO_SB_BUF_SIZE       (1024)
#define ICO_SB_VERTICAL       (1)
#define ICO_SB_HORIZONTAL     (2)

#define ICO_SB_POS_Y          (0)
#define ICO_SB_POS_COLON_X    (689)
#define ICO_SB_POS_COLON_Y    (30 + ICO_SB_POS_Y)
#define ICO_SB_POS_AMPM_X     (582)
#define ICO_SB_POS_AMPM_Y     (27 + ICO_SB_POS_Y)
#define ICO_SB_POS_HOUR1_X    (640)
#define ICO_SB_POS_HOUR1_Y    (18 + ICO_SB_POS_Y)
#define ICO_SB_POS_HOUR2_X    (662)
#define ICO_SB_POS_HOUR2_Y    (18 + ICO_SB_POS_Y)
#define ICO_SB_POS_MIN1_X     (703)
#define ICO_SB_POS_MIN1_Y     (18 + ICO_SB_POS_Y)
#define ICO_SB_POS_MIN2_X     (725)
#define ICO_SB_POS_MIN2_Y     (18 + ICO_SB_POS_Y)

#define ICO_SB_SIZE_SHTCT_W   (ICO_HS_SIZE_SB_HEIGHT)

#define ICO_SB_POS_LIST_X     (ICO_HS_SIZE_SB_HEIGHT * 5)
#define ICO_SB_POS_SHTCT1_X   (ICO_SB_POS_LIST_X + ICO_HS_SIZE_SB_HEIGHT + ICO_HS_SIZE_SB_HEIGHT * 2 / 2)
#define ICO_SB_POS_SHTCT2_X   (ICO_SB_POS_SHTCT1_X + ICO_HS_SIZE_SB_HEIGHT + ICO_HS_SIZE_SB_HEIGHT * 1 / 2)

#define ICO_SB_SIZE_COLON_W   (6)
#define ICO_SB_SIZE_COLON_H   (17)
#define ICO_SB_SIZE_NUM_W     (20)
#define ICO_SB_SIZE_NUM_H     (30)
#define ICO_SB_SIZE_AMPM_W    (47)
#define ICO_SB_SIZE_AMPM_H    (27)

#define ICO_SB_CHTCT_MAX      (5)

#define ICO_SB_APPLIST_OFFICON "applist_off.png"
#define ICO_SB_APPLIST_ONICON "applist_on.png"
#define ICO_SB_HOME_OFFICON "home_off.png"
#define ICO_SB_HOME_ONICON "home_on.png"

/*============================================================================*/
/* Class Declaration (CicoStatusBarControlWindow)                             */
/*============================================================================*/
class CicoStatusBarControlWindow
{
  public:
    CicoStatusBarControlWindow();
    ~CicoStatusBarControlWindow();

    void Initialize(CicoGKeyFileConfig *homescreen_config_tmp);
    void Finalize(void);
    int CreateEcoreEvas(void);
    static void StartEcoreLoop(void);
    static void QuitEcoreLoop(Ecore_Evas *ee);
    void ResizeWindow(int orientation);
    void ShowWindow(void);
    int InitEvas(void);
    void ShowClock(void);
    void ShowShortCut(void);
    void ShowAppListIcon(void);
    void ShowEscathion(void);
    void EvasObjectImageFileSetAndShow(Evas_Object *obj,char *icon);
    static void SetInstanceForClock(CicoStatusBarControlWindow *instance_tmp); 

  private:
    void ShowClockHour(struct tm *t_st);
    void ShowClockMinutes(struct tm *t_st);
    static Eina_Bool UpdateTime(void *thread_data);

    CicoStatusBarTime *time_data[ICO_SB_TIME_IMG_PARTS];
    static CicoStatusBarControlWindow *instance;
    int width;
    int moveH;
    Ecore_Evas *window;
    Evas *evas;
    Eina_List *shtct_list;
    Evas_Object *ampm;
    Evas_Object *hour1;
    Evas_Object *hour2;
    Evas_Object *min1;
    Evas_Object *min2;
    char respath[ICO_SB_BUF_SIZE];
    struct tm latest_time;
    CicoGKeyFileConfig *homescreen_config;
  protected:
    CicoStatusBarControlWindow operator = (const CicoStatusBarControlWindow&);
    CicoStatusBarControlWindow(const CicoStatusBarControlWindow&);
};
#endif
// vim: set expandtab ts=4 sw=4:
