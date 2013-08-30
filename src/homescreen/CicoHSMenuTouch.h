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
#ifndef __CICO_HS_MENU_TOUCH_H__
#define __CICO_HS_MENU_TOUCH_H__
#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>


#include <unistd.h>
#include "CicoHSMenuWindow.h"

#define ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_DISTANCE 100
#define ICO_HS_MENU_TOUCH_FLICK_THREASHOLD_TIME_MSECONDS 1000
#define ICO_HS_MENU_TOUCH_LONG_PUSH_THREASHOLD_TIME_SECONDS 3

class CicoHSMenuTouch
{
  public:
    static void Initialize(CicoHSMenuWindow* menu_window);
    static void Finalize(void);
    static void TouchDownMenu(void *data, Evas *evas, Evas_Object *obj, void *event_info); 
    static void TouchUpMenu(void *data, Evas *evas, Evas_Object *obj, void *event_info);

    static void TouchDownTerm(void *data, Evas *evas, Evas_Object *obj, void *event_info); 
    static void TouchUpTerm(void *data, Evas *evas, Evas_Object *obj, void *event_info);

    static void TouchUpTerminateYes(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static void TouchUpTerminateNo(void *data, Evas *evas, Evas_Object *obj, void *event_info);
    static Eina_Bool LongPushed(void *data);

  private:
    static char terminate_appid[ICO_HS_MAX_PROCESS_NAME];
    static CicoHSMenuWindow* menu_window;
    
    static int touch_state_b_x;
    static int touch_state_b_y;
    static int touch_state_a_x;
    static int touch_state_a_y;

    static Ecore_Timer *timer;
    static bool long_act;

  protected:
    CicoHSMenuTouch operator=(const CicoHSMenuTouch&);
    CicoHSMenuTouch(const CicoHSMenuTouch&);
};

#endif
