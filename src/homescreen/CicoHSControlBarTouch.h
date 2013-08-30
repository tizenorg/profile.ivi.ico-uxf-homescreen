/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action of control bar
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_CONTROLBAR_TOUCH_H__
#define __CICO_HS_CONTROLBAR_TOUCH_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include <unistd.h>
#include "CicoHSControlBarWindow.h"

class CicoHSControlBarTouch
{
  public:
    static void Initialize(CicoHSControlBarWindow* ctl_bar_window);
    static void Finalize(void);
    static void TouchDownControlBar(void *data, Evas *evas, Evas_Object *obj, void *event_info); 
    static void TouchUpControlBar(void *data, Evas *evas, Evas_Object *obj, void *event_info); 
  private:
    static CicoHSControlBarWindow* ctl_bar_window;

  protected:
    CicoHSControlBarTouch operator=(const CicoHSControlBarTouch&);
    CicoHSControlBarTouch(const CicoHSControlBarTouch&);
};

#endif

