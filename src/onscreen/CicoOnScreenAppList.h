#ifndef __CICOONSCREENAPPLIST_H__
#define __CICOONSCREENAPPLIST_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   onscreen application
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

#include "ico_uxf.h"
#include "ico_uxf_conf.h"
#include "ico_uxf_conf_ecore.h"

#include "home_screen.h"
#include "home_screen_res.h"
#include "CicoOnScreenControlWindow.h"
#include "CicoOnScreenAction.h"

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_ONS_BUF_SIZE    (1024)
#define ICO_ONS_APPLI_NUM    (15)        /* only for applist */

/*============================================================================*/
/* Class Declaration (CicoOnScreenAppList)                                        */
/*============================================================================*/
class CicoOnScreenAppList                                                      
{
 public:
    static void Initialize(CicoOnScreenControlWindow *control_window_tmp);
    static void LoadConfig(void);
    static void ConfigEvent(const char *appid, int type);                                             
    static void SetConfigCallback(void);
    static int LoadIconsEdjeFile(const char *edje_file);
    static void NextList(void);                                                                               
  private:
    static int GetAppIndex(int idx);                                                                         
    static void SetAppIcons(Evas_Object *part,const char *partname); 
    static const char *GetFname(const char *filepath);
         
    static CicoOnScreenControlWindow *control_window; 
    static int applist_idx; /* only for applist, it's index */
    static int app_cnt; /* only for applist. a number of app to listed */
  protected:
    CicoOnScreenAppList operator = (const CicoOnScreenAppList&);
    CicoOnScreenAppList(const CicoOnScreenAppList&);
};

#endif

