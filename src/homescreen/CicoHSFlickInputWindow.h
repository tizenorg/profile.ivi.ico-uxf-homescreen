/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   flick input window
 *
 * @date    Sep-20-2013
 */
#ifndef __CICO_HS_FLICKINPUT_WINDOW_H__
#define __CICO_HS_FLICKINPUT_WINDOW_H__
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
#include "CicoHomeScreenConfig.h"
#include "CicoHSWindow.h"

class CicoHSFlickInputWindow :public CicoHSWindow
{
  public:
    CicoHSFlickInputWindow(void);
    ~CicoHSFlickInputWindow(void);
    int CreateFlickInputWindow(int pos_x, int pos_y,
                               int width, int height, const char *subname = NULL);
    void FreeFlickInputWindow(void);
    void SetupFlickWindow(void);
    void Show(void);
    void Hide(void);
    bool isMyWindowName(const char *winname);
    int GetPosX(void);
    int GetPosY(void);
    int GetWidth(void);
    int GetHeight(void);

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
    CicoHSFlickInputWindow operator=(const CicoHSFlickInputWindow&);
    CicoHSFlickInputWindow(const CicoHSFlickInputWindow&);
};
#endif  /*__CICO_HS_FLICKINPUT_WINDOW_H__*/
// vim: set expandtab ts=4 sw=4:
