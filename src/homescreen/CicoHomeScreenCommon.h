/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   common value define
 *
 * @date    Aug-08-2013
 */
#ifndef __CICO_HOMESCREEN_COMMON_H__
#define __CICO_HOMESCREEN_COMMON_H__

#include <ico_log.h>
#include <ico-uxf-weston-plugin/ico_window_mgr-client-protocol.h>
#include "ico_syc_sysdef.h"

#define ICO_OK 0
#define ICO_ERROR -1

#define ICO_HS_OK               (0)
#define ICO_HS_ERR              (-1)

#define ICO_HS_TEMP_BUF_SIZE        (256)

/* max window num*/
#define ICO_HS_APP_MAX_WINDOW_NUM 32

/* max application num*/
#define ICO_HS_MAX_APP_NUM 512

/* Maximum length of application name   */
#define ICO_HS_MAX_PROCESS_NAME 64

/* HomeScreen default process name   */
#define ICO_HS_PROC_DEFAULT_HOMESCREEN "org.tizen.ico.homescreen"

/*app name*/
#define ICO_HS_APPID_DEFAULT_SB     "org.tizen.ico.statusbar"
#define ICO_HS_APPID_DEFAULT_ONS    "org.tizen.ico.onscreen"

/* path name max length */
#define ICO_HS_MAX_PATH_BUFF_LEN 256

/* max number of page in menu*/
#define ICO_HS_MENU_MAX_MENU_PAGE_NUM 40

/* max number of tile */
#define ICO_HS_MENU_MAX_TILE_NUM 512

/*window title*/
#define ICO_HS_MENU_WINDOW_TITLE "ico_hs_menuwindow"
#define ICO_HS_BACK_WINDOW_TITLE "ico_hs_backwindow"
#define ICO_HS_CONTROL_BAR_WINDOW_TITLE "ico_hs_controlbarwindow"
#define ICO_HS_SWIPE_INPUT_WINDOW_TITLE "ico_hs_swipeinputwindow"

/*path of layaout file*/
#define ICO_HS_MENU_LAYOUT_FILE_PATH "/home/app/layout.txt"

/* max number of category in menu*/
#define ICO_HS_MENU_MAX_MENU_CATEGORY_NUM 64

/*path of category file*/
#define ICO_HS_MENU_CATEGORY_FILE_PATH "/usr/apps/org.tizen.ico.homescreen/res/config/menu_categoryid.conf"

/*path of back window*/
#define ICO_HS_BACK_WINDOW_IMAGE_FILE_PATH "/opt/usr/media/Images/Home_default.jpg"

/*name of image file (in config image directry)*/
#define ICO_HS_IMAGE_FILE_MENU_BACK_GROUND "bg.png"
#define ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P "pagePointer_p.png"
#define ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N "pagePointer_n.png"

#define ICO_HS_IMAGE_FILE_MENU_PAGE_UP_CURSOR "pageUpCursor.png"
#define ICO_HS_IMAGE_FILE_MENU_PAGE_DOWN_CURSOR "pageDownCursor.png"

#define ICO_HS_IMAGE_FILE_MENU_TERMINATE_REALLY "button_really.png"
#define ICO_HS_IMAGE_FILE_MENU_TERMINATE_YES "button_yes.png"
#define ICO_HS_IMAGE_FILE_MENU_TERMINATE_NO "button_no.png"

#define ICO_HS_IMAGE_FILE_CONTROL_BAR_BASE "ctrl.png"
#define ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY    "applist_off.png"
#define ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_DAY2   "applist_off_2.png"
#define ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT  "applist_on.png"
#define ICO_HS_IMAGE_FILE_CONTROL_BAR_BUTTON_NIHGT2 "applist_on_2.png"

#define ICO_HS_LIFECYCLE_CONTROLLER_SETTING_PATH  "/usr/apps/org.tizen.ico.system-controller/res/config/system.xml"

/*config name*/
#define ICO_HS_CONFIG_HOMESCREEN    "homescreen"
#define ICO_HS_CONFIG_ONSCREEN      "onscreen"
#define ICO_HS_CONFIG_STATUBAR      "statusbar"
#define ICO_HS_CONFIG_SOUND         "sound"
#define ICO_HS_CONFIG_HISTORY       "app_history"

/*misc*/
#define HS_DISPLAY_HOMESCREEN   ICO_SYC_DISPLAY_CENTER  /* HomeScreen target display Id */

/*Layer dicralation*/
#define HS_LAYER_BACKGROUND     ICO_SYC_LAYER_BACKGROUND    /* layer of BackGround */
#define HS_LAYER_APPLICATION    ICO_SYC_LAYER_APPLICATION   /* layer of Application */
#define HS_LAYER_HOMESCREEN     ICO_SYC_LAYER_HOMESCREEN    /* layer of HomeScreen menu */
#define HS_LAYER_CONTROLBAR     ICO_SYC_LAYER_CONTROLBAR    /* layer of Control/Status bar */
#define HS_LAYER_INTERRUPTAPP   ICO_SYC_LAYER_INTERRUPTAPP  /* layer of Interrupt Application */
#define HS_LAYER_ONSCREEN       ICO_SYC_LAYER_ONSCREEN      /* layer of OnScreen   */
#define HS_LAYER_TOUCHPANEL     ICO_SYC_LAYER_TOUCHPANEL    /* layer of TouchPanel input */
#define HS_LAYER_CURSOR         ICO_SYC_LAYER_CURSOR        /* layer of Cursor   */

#define HS_LAYER_2NDDISP_APP    ICO_SYC_LAYER_MATERMAIN     /* second display application layer */

/* config history name */
#define ICO_HS_CONFIG_HSTRY_KEY1      "timer"
#define ICO_HS_CONFIG_HSTRY_DEF1      "1"
#define ICO_HS_CONFIG_HSTRY_KEY2      "manage_off_app"
#define ICO_HS_CONFIG_HSTRY_DEF2      "org.tizen.ico.homescreen;org.tizen.ico.statusbar"
#define ICO_HS_CONFIG_HSTRY_KEY3      "write_off_app"
#define ICO_HS_CONFIG_HSTRY_DEF3      "org.tizen.ico.login"
#define ICO_HS_CONFIG_HSTRY_SPL       ';'

#define ICO_HS_ANIMATION_SLIDE      "Slide"
#define ICO_HS_ANIMATION_SLIDE      "Slide"
#define ICO_HS_ANIMATION_FADE       "Fade"
#define ICO_HS_ANIMATION_FADE       "Fade"
#define ICO_HS_ANIMATION_DURATION   (400)

#endif
// vim:set expandtab ts=4 sw=4:
