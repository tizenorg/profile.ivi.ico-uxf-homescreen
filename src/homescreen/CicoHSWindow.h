/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Window class
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HSWINDOW_H__
#define __CICO_HSWINDOW_H__

#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>
#include <string.h>
#include <unistd.h>

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "CicoHomeScreenCommon.h"

#define ICO_ORIENTATION_VERTICAL (1) 
#define ICO_ORIENTATION_HORIZONTAL (2)

#define ICO_MAX_TITLE_NAME_LEN 64

class CicoHSWindow
{
  public:
    CicoHSWindow(void);
    virtual ~CicoHSWindow(void);
    int CreateWindow(const char *title,int pos_x,int pos_y,int width,int height,int alpha);
    void FreeWindow(void);
    void WindowSetting(int pos_x,int pos_y,int width,int height,int alpha);
    void ShowWindow(void);
    void HideWindow(void);
  private:

  protected:
    CicoHSWindow operator=(const CicoHSWindow&);
    CicoHSWindow(const CicoHSWindow&);

    char title[ICO_MAX_TITLE_NAME_LEN];
    int pos_x;
    int pos_y;
    int width; /*width*/
    int height; /*height*/
    Ecore_Evas *window; /* ecore-evas object */

};

#endif
