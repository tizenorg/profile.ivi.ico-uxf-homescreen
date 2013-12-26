#ifndef __CICOONSCREENACTION_H__
#define __CICOONSCREENACTION_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   touch action for onscreen application
 *
 * @date    Feb-15-2013
 */
#include <unistd.h>
#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include "home_screen.h"
#include "home_screen_res.h"
#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenSound.h"
#include "CicoOnScreenAppList.h"
#include "CicoOnScreenContext.h"

#include <ico_uws.h>

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_ONS_WS_TIMEOUT 0.1
#define ICO_ONS_BUF_SIZE    (1024)

#define ICO_ONS_CMD_WAIT    (1)
#define ICO_ONS_NO_WAIT     (2)

/*============================================================================*/
/* Class Declaration (CicoOnscreenControlContext)                                    */
/*============================================================================*/
class CicoOnScreenAction                                                     
{
  public:
    static void Initialize(CicoHomeScreenConfig *homescreen_config,
                           CicoHomeScreenSound *sound_tmp);
    static void Finalize(void);
    static Eina_Bool EcoreEvent(void *data);
    static void UwsCallBack(const struct ico_uws_context *context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data);
    static void TouchUpEdje(void *data,
                            Evas *evas, 
                            Evas_Object *obj,
                            void *event_info);
    static void TouchUpNext(void *data,
                            Evas *evas,
                            Evas_Object *obj,
                            void *event_info);
  private:
    static void CreateContext(void);
    static void DeleteContext(void);
    static char *EdjeParseStr(void *in, int arg_num);
   
    static char edje_str[ICO_ONS_BUF_SIZE];
    static int wait_reply;
    static int ws_port;
    static CicoOnScreenContext* context;
    static CicoHomeScreenSound* sound;
  protected:
    CicoOnScreenAction operator = (const CicoOnScreenAction&);
    CicoOnScreenAction(const CicoOnScreenAction&);
};

#endif

