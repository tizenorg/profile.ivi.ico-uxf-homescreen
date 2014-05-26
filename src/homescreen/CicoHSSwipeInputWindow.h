/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   swipe input window
 *
 * @date    Sep-20-2013
 */
#ifndef __CICO_HS_SWIPEINPUT_WINDOW_H__
#define __CICO_HS_SWIPEINPUT_WINDOW_H__
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

#include "ico_syc_type.h"
#include "CicoHomeScreenCommon.h"
#include "CicoGKeyFileConfig.h"
#include "CicoHSWindow.h"

class CicoHSSwipeInputWindow :public CicoHSWindow
{
  public:
    CicoHSSwipeInputWindow(void);
    ~CicoHSSwipeInputWindow(void);
    int CreateSwipeInputWindow(int pos_x, int pos_y,
                               int width, int height, const char *subname = NULL);
    void FreeSwipeInputWindow(void);
    void SetupSwipeWindow(void);
    void Show(void);
    void Hide(void);
    bool isMyWindowName(const char *winname);
    int GetPosX(void);
    int GetPosY(void);
    int GetWidth(void);
    int GetHeight(void);
    void GrabOff(void);

    void SetWindowID(const char *appid, int surface);

  private:
    /* for window control */
    char appid[ICO_HS_MAX_PROCESS_NAME];
    char winname[ICO_SYC_MAX_WINNAME_LEN];
    int surface;
    bool setuped;

    Evas *evas;                         /* evas object          */
    Evas_Object *background;            /* background object    */

  protected:
    CicoHSSwipeInputWindow operator=(const CicoHSSwipeInputWindow&);
    CicoHSSwipeInputWindow(const CicoHSSwipeInputWindow&);
};
#endif  /*__CICO_HS_SWIPEINPUT_WINDOW_H__*/
// vim: set expandtab ts=4 sw=4:
