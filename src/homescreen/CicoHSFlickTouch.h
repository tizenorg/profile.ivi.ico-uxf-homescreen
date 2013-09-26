/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action of menu
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HS_FLICK_TOUCH_H__
#define __CICO_HS_FLICK_TOUCH_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>


#include <unistd.h>
#include "CicoHSFlickInputWindow.h"
#include "CicoHSControlBarWindow.h"
#include "CicoHSAppHistoryExt.h"

#define ICO_HS_FLICK_TOUCH_FLICK_WIDTH 60
#define ICO_HS_FLICK_TOUCH_FLICK_HEIGHT 60
#define ICO_HS_FLICK_TOUCH_DISTANCE_XY 200
#define ICO_HS_FLICK_TOUCH_FLICK_THREASHOLD_DISTANCE 100
#define ICO_HS_FLICK_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS 2.5d
#define ICO_HS_FLICK_TOUCH_FLICK_ANIMA_TIME 300

#define ICO_HS_MAX_FLICKWINDOWS 4

class CicoHSFlickTouch
{
  public:
    static void Initialize(CicoHSControlBarWindow* ctl_bar, CicoHSAppHistoryExt *apphist,
                           int width, int height);
    static void Finalize(void);
    static void TouchDownFlick(void *data, Evas *evas, Evas_Object *obj, void *event_info); 
    static void TouchUpFlick(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static Eina_Bool LongPushed(void *data);

  private:
    static CicoHSControlBarWindow* ctl_bar_window;
    
    static int full_width;
    static int full_height;
    static int touch_state_b_x;
    static int touch_state_b_y;
    static int touch_state_a_x;
    static int touch_state_a_y;
    static int touch_lasttime;

    static Ecore_Timer *timer;
    static bool long_act;
    static int num_windows;
    static CicoHSFlickInputWindow* flick_windows[ICO_HS_MAX_FLICKWINDOWS];
    static CicoHSAppHistoryExt*    app_history;

  protected:
    CicoHSFlickTouch operator=(const CicoHSFlickTouch&);
    CicoHSFlickTouch(const CicoHSFlickTouch&);
};
#endif
