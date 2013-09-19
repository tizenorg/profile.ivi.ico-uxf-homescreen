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

#include "CicoLog.h"

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

/*path of layaout file*/
#define ICO_HS_MENU_LAYOUT_FILE_PATH "/home/app/layout.txt"

/*path of back window*/
#define ICO_HS_BACK_WINDOW_IMAGE_FILE_PATH "/opt/usr/media/Images/Home_default.jpg"

/*name of image file (in config image directry)*/
#define ICO_HS_IMAGE_FILE_MENU_BACK_GROUND "bg.png"
#define ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_P "pagePointer_p.png"
#define ICO_HS_IMAGE_FILE_MENU_PAGEPOINTER_N "pagePointer_n.png"
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

/*misc*/
#define HS_DISPLAY_HOMESCREEN   0           /* HomeScreen target display Id     */

/*Layer dicralation*/
#define HS_LAYER_BACKGROUND     0         /* layer of BackGround */
#define HS_LAYER_APPLICATION    1         /* layer of Application */
#define HS_LAYER_HOMESCREEN     2         /* layer of HomeScreen menu */
#define HS_LAYER_SOFTKEYBOARD   3         /* layer of Software Keyboard  */
#define HS_LAYER_INTERRUPTAPP   4         /* layer of Interrupted Application */
#define HS_LAYER_TOUCH          5         /* layer of TouchPanel */
#define HS_LAYER_ONSCREEN       6         /* layer of OnScreen   */
#define HS_LAYER_CURSOR         7         /* layer of Cursor   */



#endif
