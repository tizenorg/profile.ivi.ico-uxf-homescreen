/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   control bar window
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_CONTROLBAR_WINDOW_H__
#define __CICO_HS_CONTROLBAR_WINDOW_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreenConfig.h"
#include "CicoHSWindow.h"

#define ICO_HS_CONTROL_BAR_MENU_BTN_START_POS_Y 10
#define ICO_HS_CONTROL_BAR_MENU_BTN_WIDTH 108
#define ICO_HS_CONTROL_BAR_MENU_BTN_HEIGHT 108

class CicoHSControlBarWindow :public CicoHSWindow
{
  public:
    CicoHSControlBarWindow(void);
    ~CicoHSControlBarWindow(void);
    int Initialize(void);
    void Finalize(void);
    int CreateControlBarWindow(int pos_x,int pos_y,int width,int height);
    void FreeControlBarWindow(void);
    
    void TouchHome(void);

    void SetWindowID(const char *appid,int surface);

    void SetNightMode(void);
    void SetRegulation(void);

  private:
    /* for window control */
    char appid[ICO_HS_MAX_PROCESS_NAME];
    int surface;

    char img_dir_path[ICO_HS_MAX_PATH_BUFF_LEN];
    Evas *evas;         /* evas object */
    Evas_Object *background;
    Evas_Object *menu_btn;

  protected:
    CicoHSControlBarWindow operator=(const CicoHSControlBarWindow&);
    CicoHSControlBarWindow(const CicoHSControlBarWindow&);
};
#endif
// vim: set expandtab ts=4 sw=4:
