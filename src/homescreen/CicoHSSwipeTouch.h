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
#ifndef __CICO_HS_SWIPE_TOUCH_H__
#define __CICO_HS_SWIPE_TOUCH_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>


#include <unistd.h>
#include "CicoHSSwipeInputWindow.h"
#include "CicoHSControlBarWindow.h"
#include "CicoHSAppHistoryExt.h"

#define ICO_HS_SWIPE_TOUCH_SWIPE_WIDTH  100
#define ICO_HS_SWIPE_TOUCH_SWIPE_HEIGHT 100
#define ICO_HS_SWIPE_TOUCH_DISTANCE_TOP     200
#define ICO_HS_SWIPE_TOUCH_DISTANCE_BOTTOM  128
#define ICO_HS_SWIPE_TOUCH_SWIPE_THREASHOLD_DISTANCE 60
#define ICO_HS_SWIPE_TOUCH_SWIPE_ANIMA_TIME 300

#define ICO_HS_MAX_SWIPEWINDOWS 4

class CicoHSSwipeTouch
{
  public:
    static void Initialize(CicoHSControlBarWindow* ctl_bar, CicoHSAppHistoryExt *apphist,
                           int width, int height);
    static void Finalize(void);
    static void TouchDownSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static void TouchUpSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static void TouchMoveSwipe(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static Eina_Bool LongPushed(void *data);

  private:
    static CicoHSControlBarWindow* ctl_bar_window;

    static int full_width;
    static int full_height;
    static int touch_state_b_x;
    static int touch_state_b_y;
    static int touch_state_a_x;
    static int touch_state_a_y;

    static int touch_down;
    static bool set_xy_pos;
    static int num_windows;
    static CicoHSSwipeInputWindow* swipe_windows[ICO_HS_MAX_SWIPEWINDOWS];
    static CicoHSAppHistoryExt*    app_history;

  protected:
    CicoHSSwipeTouch operator=(const CicoHSSwipeTouch&);
    CicoHSSwipeTouch(const CicoHSSwipeTouch&);
};
#endif
// vim: set expandtab ts=4 sw=4:
