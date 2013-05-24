/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   homescreen application main
 *
 * @date    Feb-15-2013
 */

#include <Eina.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_Wayland.h>
#include <Edje.h>
#include <Elementary.h>

#include <stdio.h>
#include <pthread.h>
#include <libwebsockets.h>

#include "ico_apf.h"
#include "ico_uxf.h"
#include "ico_uxf_conf.h"
#include "ico_syc_apc.h"
#include "ico_uxf_conf_ecore.h"

#include "home_screen_lib.h"
#include "home_screen_res.h"
#include "home_screen_conf.h"
#include "home_screen_parser.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define ICO_HS_WINDOW_TYPE_BG    (1)
#define ICO_HS_WINDOW_TYPE_TOUCH (2)
#define ICO_HS_TOUCH_TIME_OUT    (1.0) /* Long press for timer */
typedef struct _hs_tile_info hs_tile_info_t;

struct _hs_tile_info {
    int idx; /* index: 0 to ... */
    int valid; /* 0: invalid, 1:valid */ /* dont't touch now */
    char appid[ICO_UXF_MAX_PROCESS_NAME]; /* active app id */
    int type; /* 11:small, 21:h-wide, 12:v-wide, 22:large, 0:others */
    int l_press; /* 1: long press */
    int change;
    int size_x;
    int size_y;
    int coord_x;
    int coord_y;
};

#define HS_DISPLAY_HOMESCREEN   0           /* HomeScreen target display Id     */
#define NUM_ONSCREEN_WINDOWS    10          /* max number of windows in OnScreen*/

#define HS_LAYER_BACKGROUND     0           /* layer of BackGround              */
#define HS_LAYER_HOMESCREEN     1           /* layer of HomeScreen menu         */
#define HS_LAYER_APPLICATION    2           /* layer of Application             */
#define HS_LAYER_SOFTKEYBOARD   3           /* layer of Software Keyboard       */
#define HS_LAYER_INTERRUPTAPP   4           /* layer of Interrupted Application */
#define HS_LAYER_TOUCH          5           /* layer of TouchPanel              */
#define HS_LAYER_ONSCREEN       6           /* layer of OnScreen                */

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static int hs_is_special_app(const char *appid);
static int hs_is_noicon_app(const char *appid);
static void hs_set_invisible_all(void);
static void hs_set_appscreen(const char *appid);
static void hs_tile_start_apps(void);
static int hs_tile_init_info(void);
static void hs_convert_tile_config(char *in, int *out, int cnt, int val);
static Eina_Bool hs_ecore_uxf_eventfd(void *data, Ecore_Fd_Handler *handler);
static void hs_uxf_event(int ev, Ico_Uxf_EventDetail dd, int arg);
static int hs_get_process_window(const char *appid);
static void hs_display_control(const Ico_Uxf_conf_application *conf, const int show);
static void hs_sound_control(const Ico_Uxf_conf_application *conf, const int adjust);
static void hs_input_control(const Ico_Uxf_conf_application *conf, const int inputsw);
static void *hs_create_window(int type);
static void hs_touch_up_api_list(void *data, Evas *evas, Evas_Object *obj,
                                 void *event_info);
static void hs_touch_down_api_list(void *data, Evas *evas, Evas_Object *obj,
                                   void *event_info);
static Eina_Bool hs_ecore_timer_event(void *data);
static void hs_touch_up_tile(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void hs_touch_down_tile(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void hs_add_touch_listener(Evas *canvas_fg);
static void hs_add_bg_image(Evas *canvas_bg);
static void hs_terminate_all_app(void);
static void hs_regulation_listener(const int appcategory,
                                   const ico_apc_reguration_control_t control,
                                   void *user_data);

/*============================================================================*/
/* variables and tables                                                       */
/*============================================================================*/
char hs_kill_appid[ICO_UXF_MAX_PROCESS_NAME];
                                         /* set kill 'app id' in touch layer  */
char hs_active_onscreen[ICO_UXF_MAX_PROCESS_NAME];
                                         /* stack 'app id' at active onscreen */
hs_window_data_t hs_command_tile_req[ICO_HS_APP_NUM_MAX];
hs_window_data_t hs_app_screen_window[ICO_HS_APP_NUM_MAX];
char hs_name_homescreen[ICO_UXF_MAX_PROCESS_NAME + 1];
static char gStatusbarName[ICO_UXF_MAX_PROCESS_NAME + 1];
static char gOnscreenName[ICO_UXF_MAX_PROCESS_NAME + 1];
static hs_tile_info_t *hs_tile_info;
static int hs_tile_cnt = 0;
static int hs_notile_cnt = 0;
static int hs_regulation_category = 0;
static int hs_regulation_visible = 1;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_uxf_event
 *          callback function called by UXF.
 *
 * @param[in]   ev                  event kinds
 * @param[in]   dd                  event detail structure
 * @param[in]   arg                 user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_uxf_event(int ev, Ico_Uxf_EventDetail dd, int arg)
{
    const Ico_Uxf_conf_application *appConf;

    uifw_trace("... Event=%08x Arg=%d", ev, arg);

    Ico_Uxf_WindowAttr winAttr;
    int ret;
    int idx;
    int dispW, dispH;

    if (ev == ICO_UXF_EVENT_NEWWINDOW) {
        uifw_trace("  D(Window) ev=%08x disp=%d win=%x x/y=%d/%d w/h=%d/%d v/r/a=%d/%d/%d",
                   dd.event, dd.window.display, dd.window.window,
                   dd.window.x, dd.window.y, dd.window.w, dd.window.h,
                   dd.window.visible, dd.window.raise, dd.window.active);
        ret = ico_uxf_window_attribute_get(dd.window.window, &winAttr);
        if (ret == ICO_UXF_EOK) {
            uifw_trace("  D(Window) appid=%s sub=%d", winAttr.process,
                       winAttr.subwindow);
            if (strncmp(winAttr.process, hs_name_homescreen,
                        ICO_UXF_MAX_PROCESS_NAME) == 0) {
                ico_uxf_window_screen_size_get(&dispW, &dispH);
                /* HomeScreen APP: BG or TOUCH */
                ico_uxf_window_resize(dd.window.window, dispW, dispH
                        - ICO_HS_SIZE_SB_HEIGHT);
                ico_uxf_window_move(dd.window.window, 0, ICO_HS_SIZE_SB_HEIGHT);
                ico_uxf_window_show(dd.window.window);
                if (winAttr.subwindow > 0) {
                    /* set layer of HomeScreen TouchPanel window    */
                    ico_uxf_window_layer(dd.window.window, HS_LAYER_TOUCH);
                    hs_tile_show_screen();
                }
                else    {
                    /* set layer of HomeScreen BackGround window    */
                    ico_uxf_window_layer(dd.window.window, HS_LAYER_BACKGROUND);
                }
            }
            else if (strncmp(winAttr.process, gStatusbarName,
                             ICO_UXF_MAX_PROCESS_NAME) == 0) {
                /* set layer of StatusBar window                */
                ico_uxf_window_layer(dd.window.window, HS_LAYER_ONSCREEN);
                /* show status bar */
                ico_uxf_window_show(dd.window.window);
                ico_uxf_window_move(dd.window.window, 0, 0);
                ico_uxf_window_screen_size_get(&dispW, &dispH);
                ico_uxf_window_resize(dd.window.window, dispW, ICO_HS_SIZE_SB_HEIGHT);
            }
            else if (strncmp(winAttr.process, gOnscreenName,
                             ICO_UXF_MAX_PROCESS_NAME) == 0) {
                /* set layer of OnScreen window             */
                ico_uxf_window_layer(dd.window.window, HS_LAYER_BACKGROUND);
                ico_uxf_window_visible_raise(dd.window.window, 1, 0);
            }
            else {
                /* other normal application */
                if (hs_is_special_app(winAttr.process) == FALSE) {
                    if (hs_is_noicon_app(winAttr.process) != FALSE) {
                        hs_set_appscreen(winAttr.process);
                        if (hs_stat_touch == ICO_HS_TOUCH_IN_SHOW) {
                            hs_tile_show_screen();
                        }
                    }
                    else if (hs_stat_touch == ICO_HS_TOUCH_IN_SHOW) {
                        /* set layer of Application in HomeScreen menu  */
                        ico_uxf_window_layer(dd.window.window, HS_LAYER_HOMESCREEN);
                        hs_tile_show_screen();
                    }
                    else if (hs_stat_touch == ICO_HS_TOUCH_IN_HIDE) {
                        hs_set_appscreen(winAttr.process);
                    }
                }
            }
        }
    }
    else if (ev == ICO_UXF_EVENT_TERMPROCESS) {
        uifw_trace("hs_uxf_event: ICO_UXF_EVENT_TERMPROCESS");

        if (strlen(dd.process.process) != 0) {
            idx = hs_tile_get_index_app(dd.process.process);
            if (idx >= 0) {
                hs_tile_free_app(idx);
            }
        }
    }
    else if (ev == ICO_UXF_EVENT_ACTIVEWINDOW)  {
        /* set active window                */
        if (dd.window.active == ICO_UXF_WINDOW_SELECT)  {
            if (ico_uxf_window_attribute_get(dd.window.window, &winAttr) == ICO_UXF_EOK)  {
                if ((strncmp(winAttr.process, hs_name_homescreen, ICO_UXF_MAX_PROCESS_NAME)
                        != 0) &&
                    (strncmp(winAttr.process, gStatusbarName, ICO_UXF_MAX_PROCESS_NAME)
                        != 0) &&
                    (strncmp(winAttr.process, gOnscreenName, ICO_UXF_MAX_PROCESS_NAME)
                        != 0))  {
                    /* if selected application is soft-keyboard, activate only pointer, */
                    /* other case activate pointer and keyboard                         */
                    appConf = ico_uxf_getAppByAppid(winAttr.process);
                    if ((appConf != NULL) &&
                        (appConf->display[0].layerId == HS_LAYER_SOFTKEYBOARD)) {
                        ico_uxf_window_active(dd.window.window,
                                              ICO_UXF_WINDOW_POINTER_ACTIVE);
                        ico_uxf_window_raise(dd.window.window);
                    }
                    else    {
                        ico_uxf_window_active(dd.window.window,
                                              ICO_UXF_WINDOW_POINTER_ACTIVE |
                                                  ICO_UXF_WINDOW_KEYBOARD_ACTIVE);
                        ico_uxf_window_raise(dd.window.window);
                        ico_syc_apc_active(winAttr.process);
                    }
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_ecore_uxf_eventfd
 *          callback function of wayland fd event that is used in UXF.
 *          if the fd has event, ecore call this function.
 *
 * @param[in]   data                user data
 * @param[in]   handler             handler
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
hs_ecore_uxf_eventfd(void *data, Ecore_Fd_Handler *handler)
{
    ico_uxf_main_loop_iterate();

    return EINA_TRUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_convert_tile_config
 *          for read configuration.
 *          convert configuration string to integer array.
 *
 * @param[in]   in                  configuration
 * @param[out]  out                 integer array
 * @param[in]   cnt                 count
 * @param[in]   val                 default value
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_convert_tile_config(char *in, int *out, int cnt, int val)
{
    int idx;
    char *tok;
    char *s = ";";

    for (idx = 0; idx < cnt; idx++) {
        out[idx] = val;
    }
    if (in != NULL) {
        tok = strtok(in, s);
        for (idx = 0; idx < cnt; idx++) {
            out[idx] = atoi(tok);
            tok = strtok(NULL, s);
            if (tok == NULL) {
                break;
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_index_appscreendata
 *          get applicaton screen data index.
 *
 * @param[in]   appid               application id
 * @return      result
 * @retval      >=0                 app index
 * @retval      -1                  not exist
 */
/*--------------------------------------------------------------------------*/
int
hs_get_index_appscreendata(const char *appid)
{
    int idx = -1;
    int ii;

    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (hs_app_screen_window[ii].appid[0] == 0) {
            if (idx == -1) {
                idx = ii;
            }
            continue;
        }
        if (strncmp(hs_app_screen_window[ii].appid, appid,
                    ICO_UXF_MAX_PROCESS_NAME) == 0) {
            return ii;
        }
    }

    if (idx >= 0) {
        strncpy(hs_app_screen_window[idx].appid, appid,
                ICO_UXF_MAX_PROCESS_NAME);
    }
    return idx;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_set_appscreen
 *          set the application screen's status to the applicaton window,
 *          and visible raise the application window.
 *
 * @param[in]   appid               application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_set_appscreen(const char *appid)
{
    int idx;
    int ret;
    Ico_Uxf_ProcessWin window;
    const Ico_Uxf_conf_application *appConf;
    const Ico_Uxf_Sys_Config       *sysConf;
    Ico_Uxf_conf_display_zone      *zone;

    idx = hs_get_index_appscreendata(appid);

    if (idx < 0) {
        return;
    }

    /* visible raise */
    ret = ico_uxf_process_window_get(hs_app_screen_window[idx].appid, &window, 1);
    if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
        uifw_trace("hs_set_appscreen: app[%d]:visible=%d raise=%d %dx%d(%d,%d)",
                   idx, hs_app_screen_window[idx].visible, hs_app_screen_window[idx].raise,
                   hs_app_screen_window[idx].resize_w, hs_app_screen_window[idx].resize_h,
                   hs_app_screen_window[idx].move_x, hs_app_screen_window[idx].move_y);
        /* move application window to application layer */
        appConf = ico_uxf_getAppByAppid(hs_app_screen_window[idx].appid);
        if (! appConf)  {
            ico_uxf_window_layer(window.window, HS_LAYER_APPLICATION);
            /* show application layer                       */
            ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_APPLICATION, 1);
        }
        else    {
            ico_uxf_window_layer(window.window, appConf->display[0].layerId);
            /* show application layer                       */
            ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, appConf->display[0].layerId, 1);

            if ((hs_app_screen_window[idx].resize_w <= 0) &&
                (hs_app_screen_window[idx].resize_h <= 0) &&
                (hs_app_screen_window[idx].move_x <= 0) &&
                (hs_app_screen_window[idx].move_y <= 0))    {
                /* not set size and position, set default(zone size)    */
                sysConf = ico_uxf_getSysConfig();
                zone = (Ico_Uxf_conf_display_zone *)&sysConf->
                           display[appConf->display[0].displayId].
                               zone[appConf->display[0].zoneId];
                hs_app_screen_window[idx].resize_w = zone->width;
                hs_app_screen_window[idx].resize_h = zone->height;
                hs_app_screen_window[idx].move_x = zone->x;
                hs_app_screen_window[idx].move_y = zone->y;
                hs_app_screen_window[idx].visible = 1;
                hs_app_screen_window[idx].raise = 1;
            }
        }
        ico_uxf_window_move(window.window, hs_app_screen_window[idx].move_x,
                            hs_app_screen_window[idx].move_y);
        ico_uxf_window_resize(window.window,
                              hs_app_screen_window[idx].resize_w,
                              hs_app_screen_window[idx].resize_h);
        ico_uxf_window_visible_raise(window.window,
                                     hs_app_screen_window[idx].visible,
                                     hs_app_screen_window[idx].raise);
        /* hide HomeScreen layer                        */
        ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 0);
    }
    ico_uxf_main_loop_iterate();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_set_invisible_all
 *          set all applicatoin except special applicaion(homescreen
 *          /statusbar/onscreen) in invisible.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_set_invisible_all(void)
{
    uifw_trace("hs_set_invisible_all: Enter");

    /* hide application layer                       */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_APPLICATION, 0);
    /* hide HomeScreen layer menu                   */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 0);

    uifw_trace("hs_set_invisible_all: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_show_appscreen
 *          show applicaton screen and raise the indicated application.
 *
 * @param[in]   appid               application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_show_appscreen(const char *appid)
{
    Ico_Uxf_ProcessWin window;
    const Ico_Uxf_conf_application *appConf = NULL;
    int ii, idx;
    int ret;
    int dispW, dispH;
    int sid;

    uifw_trace("hs_show_appscreen: Enter(appid=%s)", appid ? appid : "(NULL)");
    /* change to noraml mode for AppsControler  */
    ico_uxf_window_control(NULL, -1, ICO_UXF_APPSCTL_TEMPVISIBLE, 0);

    /* touch to invisible */
    sid = hs_get_process_window(ICO_HS_APPID_DEFAULT_TOUCH);
    if (sid >= 0) {
        ico_uxf_window_hide(sid);
    }
    /* all apps invisible */
    hs_set_invisible_all();

    /* show other apps */
    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if (hs_app_screen_window[ii].appid[0] == 0)
            continue;
        appConf = ico_uxf_getAppByAppid(hs_app_screen_window[ii].appid);
        if (appConf != NULL) {
            hs_app_screen_window[ii].category = appConf->categoryId;
        }
        if (hs_is_special_app(hs_app_screen_window[ii].appid))
            continue;
        if ((appid != NULL) && strncmp(hs_app_screen_window[ii].appid, appid,
                                       ICO_UXF_MAX_PROCESS_NAME) == 0)
            continue;

        /* if invisible */
        ret = ico_uxf_process_window_get(hs_app_screen_window[ii].appid,
                                         &window, 1);
        uifw_trace("hs_show_appscreen: window_get app[%s] = %d(visible=%d)",
                   hs_app_screen_window[ii].appid, ret,
                   hs_app_screen_window[ii].visible);
        if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
            if (hs_app_screen_window[ii].visible == 0) {
            }
            else {
                uifw_trace("hs_show_appscreen: move %d=%dx%d(%d,%d)", ii,
                           hs_app_screen_window[ii].resize_w,
                           hs_app_screen_window[ii].resize_h,
                           hs_app_screen_window[ii].move_x, hs_app_screen_window[ii].move_y);
                /* move application window to application layer */
                ico_uxf_window_resize(window.window,
                                      hs_app_screen_window[ii].resize_w,
                                      hs_app_screen_window[ii].resize_h);
                ico_uxf_window_move(window.window,
                                    hs_app_screen_window[ii].move_x,
                                    hs_app_screen_window[ii].move_y);
                ico_uxf_window_show(window.window);
                if (! appConf)  {
                    ico_uxf_window_layer(window.window, HS_LAYER_APPLICATION);
                    /* show application layer                       */
                    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_APPLICATION, 1);
                }
                else    {
                    ico_uxf_window_layer(window.window, appConf->display[0].layerId);
                    /* show application layer                       */
                    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN,
                                          appConf->display[0].layerId, 1);
                }
            }
        }
    }
    /* hide HomeScreen layer menu                   */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 0);
    /* hide Touch layer                             */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_TOUCH, 0);
    /* hide OnScreen windows                        */
    hs_hide_onscreen();

    /* show own apps */
    if (appid != NULL) {
        idx = hs_get_index_appscreendata(appid);
        /* if not set pos, load default */
        appConf = ico_uxf_getAppByAppid(appid);
        if (appConf != NULL) {
            int x, y, width, height;
            (void)ico_uxf_getAppDisplay(appConf, 0, &x, &y, &width, &height);

            if ((hs_app_screen_window[idx].resize_w == -1)
                    || (hs_app_screen_window[idx].resize_h == -1)) {
                hs_app_screen_window[idx].resize_w = width;
                hs_app_screen_window[idx].resize_h = height;
            }
            if ((hs_app_screen_window[idx].move_x == -1)
                    || (hs_app_screen_window[idx].move_y == -1)) {
                hs_app_screen_window[idx].move_x = x;
                hs_app_screen_window[idx].move_y = y;
            }
            if ((hs_app_screen_window[idx].resize_w == 0)
                    && (hs_app_screen_window[idx].resize_h == 0)
                    && (hs_app_screen_window[idx].move_x == 0)
                    && (hs_app_screen_window[idx].move_y == 0)) {
                /* not have manifest, set default */
                ico_uxf_window_screen_size_get(&dispW, &dispH);
                hs_app_screen_window[idx].resize_w = dispW;
                hs_app_screen_window[idx].resize_h = (dispH - ICO_HS_SIZE_SB_HEIGHT) / 2;
                hs_app_screen_window[idx].move_x = 0;
                hs_app_screen_window[idx].move_y = ICO_HS_SIZE_SB_HEIGHT;
            }
        }

        /* show */
        ret = ico_uxf_process_window_get(appid, &window, 1);
        uifw_trace("hs_show_appscreen: window_get app[%s] = %d", appid, ret);
        if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
            uifw_trace("hs_show_appscreen: move %d=%dx%d(%d,%d)", idx,
                       hs_app_screen_window[idx].resize_w,
                       hs_app_screen_window[idx].resize_h,
                       hs_app_screen_window[idx].move_x,
                       hs_app_screen_window[idx].move_y);
            /* activate application window                  */
            if ((appConf != NULL) && (appConf->display[0].layerId == HS_LAYER_SOFTKEYBOARD)) {
                ico_uxf_window_active(window.window, ICO_UXF_WINDOW_POINTER_ACTIVE);
            }
            else    {
                ico_uxf_window_active(window.window,
                                      ICO_UXF_WINDOW_POINTER_ACTIVE |
                                          ICO_UXF_WINDOW_KEYBOARD_ACTIVE);
            }
            ico_uxf_window_raise(window.window);
            ico_syc_apc_active(appid);

            /* move application window to application layer */
            ico_uxf_window_resize(window.window,
                                  hs_app_screen_window[idx].resize_w,
                                  hs_app_screen_window[idx].resize_h);
            ico_uxf_window_move(window.window,
                                hs_app_screen_window[idx].move_x,
                                hs_app_screen_window[idx].move_y);
            ico_uxf_window_visible_raise(window.window, 1, 1);
            if (! appConf)  {
                ico_uxf_window_layer(window.window, HS_LAYER_APPLICATION);
                /* show application layer                       */
                ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_APPLICATION, 1);
            }
            else    {
                ico_uxf_window_layer(window.window, appConf->display[0].layerId);
                /* show application layer                       */
                ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN,
                                      appConf->display[0].layerId, 1);
            }
            hs_app_screen_window[idx].visible = 1;
            /* hide HomeScreen layer                        */
            ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 0);
        }
    }
    hs_stat_touch = ICO_HS_TOUCH_IN_HIDE;
    ico_uxf_main_loop_iterate();

    uifw_trace("hs_show_appscreen: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_start_apps
 *          launch the default application that indicated by configuration
 *          file.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_tile_start_apps(void)
{
    char cName[ICO_UXF_MAX_PROCESS_NAME + 1];
    int idx, ret;
    int idx1, idx2;
    const char *appName;
    const Ico_Uxf_App_Config *appConf;
    hs_tile_info_t *tinfo;

    /* excute applications in tile menu */
    for (idx = 0; idx < hs_tile_cnt; idx++) {
        tinfo = &hs_tile_info[idx];
        if (tinfo->valid == 0) continue;
        sprintf(cName, ICO_HS_CONFIG_TILE_APP"%d", idx);
        appName = hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN, cName, NULL);
        if ((appName != NULL) && (strcmp(appName, "none") != 0)) {
            ret = ico_uxf_process_execute(appName);
            if (ret >= 0) {
                uifw_trace("hs_tile_start_apps: execute tile[%d]=%s", idx, appName);
                memset(tinfo->appid, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
                strncpy(tinfo->appid, appName, ICO_UXF_MAX_PROCESS_NAME);
            }
        }
    }

    /* excute no menu applications      */
    appConf = ico_uxf_getAppConfig();
    idx = hs_tile_cnt;
    for (idx1 = 0; idx1 < appConf->applicationNum; idx1++)  {
        if (appConf->application[idx1].autostart == 0)  continue;
        for (idx2 = 0; idx2 < hs_tile_cnt; idx2++)  {
            if (! strcmp(hs_tile_info[idx2].appid, appConf->application[idx1].appid)) break;
        }
        if (idx2 >= hs_tile_cnt)    {
            ret = ico_uxf_process_execute(appConf->application[idx1].appid);
            if (ret >= 0) {
                uifw_trace("hs_tile_start_apps: execute no tile[%d]=%s",
                           idx, appConf->application[idx1].appid);
                memset(hs_tile_info[idx].appid, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
                strncpy(hs_tile_info[idx].appid,
                        appConf->application[idx1].appid, ICO_UXF_MAX_PROCESS_NAME);
                idx ++;
                if ((idx - hs_tile_cnt) >= ICO_HS_NOTILE_APP_MAX)   break;
            }
        }
    }
    hs_notile_cnt = idx - hs_tile_cnt;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_init_info
 *          initialize tile information.
 *
 * @param       none
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          err
 */
/*--------------------------------------------------------------------------*/
static int
hs_tile_init_info(void)
{
    int idx, row, column;
    char *sValid = NULL;
    char *sType = NULL;
    int *valid = NULL;
    int *type = NULL;
    int dispW, dispH;
    int width, height;
    int mSide, mTop;
    int colcnt, rowcnt, tilecnt, tilesize;
    int try = 0;
    hs_tile_info_t *tinfo;

    /* count tile */
    colcnt = hs_conf_get_integer(ICO_HS_CONFIG_HOMESCREEN,
                                 ICO_HS_CONFIG_TILE_COLCNT,
                                 ICO_HS_CONFIG_TILE_COLCNT_DEFAULT);
    rowcnt = hs_conf_get_integer(ICO_HS_CONFIG_HOMESCREEN,
                                 ICO_HS_CONFIG_TILE_ROWCNT,
                                 ICO_HS_CONFIG_TILE_ROWCNT_DEFAULT);
    tilecnt = colcnt * rowcnt;

    /* allocate tile info structure */
    hs_tile_info = malloc(sizeof(hs_tile_info_t) * (tilecnt + ICO_HS_NOTILE_APP_MAX));
    valid = (int *)malloc(sizeof(int) * (tilecnt + ICO_HS_NOTILE_APP_MAX));
    type = (int *)malloc(sizeof(int) * (tilecnt + ICO_HS_NOTILE_APP_MAX));
    if (!hs_tile_info || !valid || !type) {
        uifw_warn("hs_tile_init_info: Leave(err malloc tile str failed)");
        return ICO_HS_ERR;
    }

    /* get screen size */
    ico_uxf_window_screen_size_get(&dispW, &dispH);
    while ((dispW < 0) || (dispH < 0)) {
        ico_uxf_main_loop_iterate();
        try++;
        if (try > 10) {
            uifw_warn("hs_tile_init_info: Leave(cannot get screen size)");
            return ICO_HS_ERR;
        }
        usleep(10000);
        ico_uxf_window_screen_size_get(&dispW, &dispH);
    }
    width = dispW;
    height = dispH - ICO_HS_SIZE_SB_HEIGHT;

    /* determin tile size (interval the tile is 10 percent of the tile width) */
    if (((float)width / (float)colcnt) > ((float)height / (float)rowcnt)) {
        /* tile size based height */
        tilesize = height / (1.0 * rowcnt + 0.1 * (rowcnt + 1));
        mSide = (width - (tilesize * colcnt + tilesize * 0.1 * (colcnt + 1)))
                / 2;
        mTop = 0;
    }
    else {
        /* tile size based width */
        tilesize = width / (1.0 * colcnt + 0.1 * (colcnt + 1));
        mSide = 0;
        mTop = (height - (tilesize * rowcnt + tilesize * 0.1 * (rowcnt + 1)))
                / 2;
    }
    uifw_trace("hs_tile_init_info: mSide=%d mTop=%d disp(%dx%d)", mSide, mTop,
               dispW, dispH);

    /* get tile config */
    sValid = (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                        ICO_HS_CONFIG_TILE_VALID, NULL);
    sType = (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                       ICO_HS_CONFIG_TILE_TYPE, NULL);
    hs_convert_tile_config(sValid, valid, tilecnt, 1);
    hs_convert_tile_config(sType, type, tilecnt, ICO_HS_TILE_SMALL);

    /* set tile parameter */
    for (idx = 0; idx < tilecnt; idx++) {
        tinfo = &hs_tile_info[idx];
        row = idx / colcnt;
        column = idx % colcnt;
        tinfo->idx = idx;
        memset(tinfo->appid, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
        tinfo->valid = valid[idx];
        tinfo->type = type[idx];
        if (idx == tilecnt - 1) {
            tinfo->type = ICO_HS_TILE_OTHERS;
        }
        tinfo->l_press = 0;
        tinfo->change = 0;

        tinfo->coord_x = mSide + tilesize * 0.1 + (tilesize + tilesize * 0.1)
                * column;
        tinfo->coord_y = mTop + ICO_HS_SIZE_SB_HEIGHT + tilesize * 0.1 + (tilesize
                + tilesize * 0.1) * row;
        switch (tinfo->type) {
        case ICO_HS_TILE_SMALL:
            tinfo->size_x = tilesize;
            tinfo->size_y = tilesize;
            break;
        case ICO_HS_TILE_HWIDE:
            tinfo->size_x = tilesize;
            tinfo->size_y = tilesize * 2.1;
            break;
        case ICO_HS_TILE_VWIDE:
            tinfo->size_x = tilesize * 2.1;
            tinfo->size_y = tilesize;
            break;
        case ICO_HS_TILE_LARGE:
            tinfo->size_x = tilesize * 2.1;
            tinfo->size_y = tilesize * 2.1;
            break;
        default:
            tinfo->size_x = tilesize;
            tinfo->size_y = tilesize;
            break;
        }

        uifw_trace("TILE_INFO:tile[%d]={%d,%d,%s,%d,%d,%d,(%dx%d),(%d,%d)}",
                   idx, tinfo->idx, tinfo->valid, tinfo->appid, tinfo->type,
                   tinfo->l_press, tinfo->change, tinfo->size_x, tinfo->size_y,
                   tinfo->coord_x, tinfo->coord_y);
    }

    hs_tile_cnt = tilecnt;

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_free_app
 *          delete all information of the indicated tile.
 *
 * @param[in]   idx                 tile index
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_tile_free_app(int idx)
{
    hs_tile_info_t *tinfo;

    if ((idx >= 0) && (idx < hs_tile_cnt)) {
        tinfo = &hs_tile_info[idx];
        memset(tinfo->appid, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_show_screen
 *          show tile screen again.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_tile_show_screen(void)
{
    Ico_Uxf_ProcessWin window;
    int ret;
    int idx, sid;
    hs_tile_info_t *tinfo;

    if (hs_regulation_visible == 0) {
        uifw_trace("hs_regulation_visible: regulation, no menu display");
        return;
    }

    /* all apps invisible */
    /* hide HomeScreen layer menu                   */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 0);

    /* show application tile */
    for (idx = 0; idx < hs_tile_cnt; idx++) {
        tinfo = &hs_tile_info[idx];
        if (tinfo->appid[0] == 0)
            continue;

        ret = ico_uxf_process_window_get(tinfo->appid, &window, 1);
        uifw_trace("hs_tile_show_screen: window_get app[%s] = %d(%08x)",
                   tinfo->appid, ret, window.window);
        if (((ret > 0) || (ret == ICO_UXF_E2BIG)) && (window.window != 0))  {
            uifw_trace("hs_tile_show_screen: move %d=%dx%d(%d,%d)", idx,
                       tinfo->size_x, tinfo->size_y, tinfo->coord_x, tinfo->coord_y);
            /* move application window to HomeScreen layer  */
            ico_uxf_window_layer(window.window, HS_LAYER_HOMESCREEN);
            ico_uxf_window_resize(window.window, tinfo->size_x, tinfo->size_y);
            ico_uxf_window_move(window.window, tinfo->coord_x, tinfo->coord_y);
            ico_uxf_window_visible_raise(window.window, 1, 1);
        }
    }

    hs_stat_touch = ICO_HS_TOUCH_IN_SHOW;
    /* visible and raise touch layer */
    sid = hs_get_process_window(ICO_HS_APPID_DEFAULT_TOUCH);
    if (sid >= 0) {
        ico_uxf_window_visible_raise(sid, 1, 1);
    }
    /* show HomeScreen layer menu                   */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_HOMESCREEN, 1);
    /* show Touch layer                             */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_TOUCH, 1);
    /* hide application layer                       */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_APPLICATION, 0);

    /* chnage to menu mode for AppsControler        */
    ico_uxf_window_control(NULL, -1, ICO_UXF_APPSCTL_TEMPVISIBLE, 1);

    ico_uxf_main_loop_iterate();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_get_index_app
 *          Get tile index that has indicated application id.
 *
 * @param[in]   appid               application id
 * @return      result
 * @retval      >=0                 tile index
 * @retval      -1                  not exist
 */
/*--------------------------------------------------------------------------*/
int
hs_tile_get_index_app(const char *appid)
{
    int idx;
    hs_tile_info_t *tinfo;

    for (idx = 0; idx < hs_tile_cnt; idx++) {
        tinfo = &hs_tile_info[idx];
        if (strncmp(tinfo->appid, appid, ICO_UXF_MAX_PROCESS_NAME) == 0) {
            return idx;
        }
    }

    return -1;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_get_minchange
 *          Get a application tile index that has no application
 *          or is changed minimum
 *
 * @param       none
 * @return      result
 * @retval      >=                  success(tile index)
 * @retval      < 0                 error
 */
/*--------------------------------------------------------------------------*/
int
hs_tile_get_minchange(void)
{
    int ii;
    int idx = -1;
    int min = -1;
    hs_tile_info_t *tinfo;

    for (ii = 0; ii < hs_tile_cnt; ii++) {
        tinfo = &hs_tile_info[ii];
        if ((tinfo->type > ICO_HS_TILE_OTHERS) && ((tinfo->change <= min)
                || (min == -1))) {
            idx = ii;
            min = tinfo->change;
        }
        if ((tinfo->type > ICO_HS_TILE_OTHERS) && (strlen(tinfo->appid) == 0)) {
            return ii;
        }
    }

    return idx;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_kill_app
 *          terminate the indicated application, and free the tile
 *          that have the application.
 *
 * @param[in]   appid               application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_tile_kill_app(const char *appid)
{
    int idx, ret;

    idx = hs_tile_get_index_app(appid);
    if (idx < 0) {
        return;
    }

    ret = ico_uxf_process_terminate(appid);
    if (ret == ICO_UXF_EOK) {
        hs_tile_free_app(idx);
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_get_index_pos
 *          get the tile index of the position.
 *
 * @param[in]   appid               application id
 * @return      tile index
 * @retval      >=0                 success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
int
hs_tile_get_index_pos(int x, int y, int w, int h)
{
    int idx = -1;
    hs_tile_info_t *tinfo;

    for (idx = 0; idx < hs_tile_cnt; idx++) {
        tinfo = &hs_tile_info[idx];
        if ((tinfo->coord_x == x) && (tinfo->coord_y == y) && (tinfo->size_x
                == w) && (tinfo->size_y == h)) {
            return idx;
        }
    }

    return ICO_HS_ERR;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_tile_set_app
 *          set the applicaton to the tile of the indicated index.
 *
 * @param[in]   idx                 tile index
 * @param[in]   appid               application id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_tile_set_app(int idx, const char *appid)
{
    hs_tile_info_t *tinfo;

    uifw_trace("hs_tile_set_app: Enter(idx=%d appid=%s)", idx, appid);

    if (idx >= 0) {
        tinfo = &hs_tile_info[idx];
        if (tinfo->valid > 0) {
            int oldwin = hs_get_process_window(tinfo->appid);
            if (oldwin > 0) {
                (void) ico_uxf_window_hide(oldwin);
            }
            strncpy(tinfo->appid, appid, ICO_UXF_MAX_PROCESS_NAME);
            tinfo->change++;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_process_window
 *          get window surface from applicatoin id including special id.
 *
 * @param[in]   appid               application id
 * @return      result
 * @retval      >=0                 surface
 * @retval      -1                  not exist
 */
/*--------------------------------------------------------------------------*/
static int
hs_get_process_window(const char *appid)
{
    Ico_Uxf_ProcessWin wins[2];
    int ret;

    if (strcmp(appid, ICO_HS_APPID_DEFAULT_TOUCH) == 0) {
        ret = ico_uxf_process_window_get(hs_name_homescreen, wins, 2);
        if ((ret > 1) || (ret == ICO_UXF_E2BIG))    {
            return wins[1].window;
        }
    }
    else {
        ret = ico_uxf_process_window_get(appid, wins, 1);
        if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
            return wins[0].window;
        }
    }

    return -1;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_show_onscreen
 *          set onscreen window to visible.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_show_onscreen(void)
{
    Ico_Uxf_ProcessWin window;
    int ret;

    ret = ico_uxf_process_window_get(ICO_HS_APPID_DEFAULT_ONS, &window, 1);

    if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
        ico_uxf_window_layer(window.window, HS_LAYER_ONSCREEN);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_hide_onscreen
 *          set onscreen window to invisible.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_hide_onscreen(void)
{
    Ico_Uxf_ProcessWin window;
    int ret;

    ret = ico_uxf_process_window_get(ICO_HS_APPID_DEFAULT_ONS, &window, 1);

    if ((ret > 0) || (ret == ICO_UXF_E2BIG))    {
        ico_uxf_window_layer(window.window, HS_LAYER_BACKGROUND);
        ico_uxf_window_lower(window.window);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_click_escutcheon
 *          processing when escutcheon button is clicked.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_click_escutcheon(void)
{
    uifw_trace("hs_click_escutcheon: Enter(regulation=%d)", hs_regulation_visible);

    if (strlen(hs_active_onscreen) > 0) {
        uifw_trace("hs_click_escutcheon: Leave(active=%s)", hs_active_onscreen);
        return;
    }
    if (hs_regulation_visible == 0) {
        uifw_trace("hs_click_escutcheon: Leave(regulation)");
        return;
    }

    if (hs_stat_touch == ICO_HS_TOUCH_IN_SHOW) {
        hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
        uifw_trace("hs_click_escutcheon: hs_show_appscreen");
        hs_show_appscreen(NULL);
    }
    else if (hs_stat_touch == ICO_HS_TOUCH_IN_HIDE) {
        hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
        uifw_trace("hs_click_escutcheon: hs_tile_show_screen");
        hs_tile_show_screen();
    }
    uifw_trace("hs_click_escutcheon: Leave");
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_click_applist
 *          processing when application list button is clicked.
 *
 * @param       none
 * @return      regulation
 * @retval      =0      regulation controlled(no display)
 * @retval      =1      no regulation
 */
/*--------------------------------------------------------------------------*/
int
hs_click_applist(void)
{
    uifw_trace("hs_click_applist: Enter(regulation=%d)", hs_regulation_visible);

    if (hs_regulation_visible == 0) {
        uifw_trace("hs_click_applist: Leave(regulation)");
        return 0;
    }

    /* operation sound */
    hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
    uifw_trace("hs_click_applist: Leave");

    /* show Touch layer                             */
    ico_uxf_layer_visible(HS_DISPLAY_HOMESCREEN, HS_LAYER_TOUCH, 1);
    /* show OnScreen windows                        */
    hs_show_onscreen();

    return 1;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_is_special_app
 *          return 1 if the application is special one(homescreen)
 *          /statusbar/onscreen).
 *
 * @param[in]   appid       applicationId
 * @return      special application
 * @retval      1           special application(HomeScreen/StatusBar ... etc)
 * @retval      0           normal application
 */
/*--------------------------------------------------------------------------*/
static int
hs_is_special_app(const char *appid)
{
    if ((strncmp(appid, hs_name_homescreen, ICO_UXF_MAX_PROCESS_NAME) == 0)
            || (strncmp(appid, gStatusbarName, ICO_UXF_MAX_PROCESS_NAME) == 0)
            || (strncmp(appid, gOnscreenName, ICO_UXF_MAX_PROCESS_NAME) == 0)
            || (strncmp(appid, ICO_HS_APPID_DEFAULT_TOUCH,
                        ICO_UXF_MAX_PROCESS_NAME) == 0)) {
        uifw_trace("hs_is_special_app: app(%s) is special", appid);
        return 1;
    }
    uifw_trace("hs_is_special_app: app(%s) is normal", appid);
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_is_noicon_app
 *          return 1 if the application has no menu icon
 *
 * @param[in]   appid       applicationId
 * @return      no need menu
 * @retval      1           no need menu
 * @retval      0           need menu
 */
/*--------------------------------------------------------------------------*/
static int
hs_is_noicon_app(const char *appid)
{
    const Ico_Uxf_conf_application  *appConf;

    appConf = ico_uxf_getAppByAppid(appid);
    if (! appConf)  {
        uifw_trace("hs_is_noicon_app: appid(%s) dose not exist", appid);
        return 0;
    }

    if (appConf->noicon)    {
        uifw_trace("hs_is_noicon_app: appid(%s) has no icon", appid);
        return 1;
    }
    uifw_trace("hs_is_noicon_app: appid(%s) has icon", appid);
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_terminate_all_app
 *          terminate all application that homescreen handled
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_terminate_all_app(void)
{
    Ico_Uxf_ProcessAttr procs[ICO_HS_APP_NUM_MAX];
    int cnt;
    int ii = 0;

    cnt = ico_uxf_process_query_processes(procs, ICO_HS_APP_NUM_MAX);
    if (cnt == ICO_UXF_E2BIG)
        cnt = ICO_HS_APP_NUM_MAX;
    uifw_trace("hs_terminate_all_app: ico_uxf_process_query_processes() = %d",
               cnt);
    for (ii = 0; ii < cnt; ii++) {
        if (hs_is_special_app(procs[ii].process))
            continue;
        uifw_trace("hs_terminate_all_app: terminate(%s)", procs[ii].process);
        ico_uxf_process_terminate(procs[ii].process);
    }
    ico_uxf_process_terminate(gStatusbarName);
    ico_uxf_process_terminate(gOnscreenName);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_req_ctlr_tile
 *          processing when homescreen received json command from extra
 *          tools.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_req_ctlr_tile(void)
{
    int ii, ret;
    int idx;

    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        if ((hs_command_tile_req[ii].appid[0] == 0)
                || (hs_is_special_app(hs_command_tile_req[ii].appid) == TRUE)) {
            continue;
        }

        idx = hs_tile_get_index_app(hs_command_tile_req[ii].appid);

        /* terminate app */
        if (hs_command_tile_req[ii].terminate != IGNORE) {
            ret = ico_uxf_process_terminate(hs_command_tile_req[ii].appid);
            if (ret == ICO_UXF_EOK) {
                if (idx >= 0) {
                    hs_tile_free_app(idx);
                }
            }
        }

        /* execute app */
        if (strlen(hs_command_tile_req[ii].exe_name) > 0) {
            ico_uxf_process_execute(hs_command_tile_req[ii].exe_name);
        }
    }
    hs_tile_show_screen();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_touch_up_api_list
 *          processing when application list button touch up.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_touch_up_api_list(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    int idx = *(int *)data;
    char buf[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];
    hs_tile_info_t *tinfo;

    tinfo = &hs_tile_info[idx];

    /* set the image */
    hs_get_image_path(path, sizeof(path));
    sprintf(buf, "%s"fname_api_all_off, path);
    evas_object_image_file_set(obj, buf, NULL);
    /* move and resize in the image */
    evas_object_move(obj, tinfo->coord_x, tinfo->coord_y - ICO_HS_SIZE_SB_HEIGHT);
    evas_object_resize(obj, tinfo->size_x, tinfo->size_y);
    /* show the image */
    evas_object_show(obj);

    uifw_trace("hs_touch_up_api_list: idx=%d", idx);
    strncpy(hs_active_onscreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN,
            sizeof(hs_active_onscreen));
    hs_get_ons_edj_path(path, sizeof(path));
    sprintf(buf, "OPEN %s%s %s", path, ICO_HS_ONS_APPLI_LIST_NAME,
            ICO_UXF_PROC_DEFAULT_HOMESCREEN);
    uifw_trace("send '%s' to onscreen", buf);
    if (hs_lib_event_message(ICO_HS_PROTOCOL_TYPE_OS, buf) < 0) {
        memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
    }
    hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_touch_down_api_list
 *          processing when application list button touch down.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_touch_down_api_list(void *data, Evas *evas, Evas_Object *obj,
                       void *event_info)
{
    int idx = *(int *)data;
    char buf[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];
    hs_tile_info_t *tinfo;

    tinfo = &hs_tile_info[idx];

    /* set the image */
    hs_get_image_path(path, sizeof(path));
    sprintf(buf, "%s"fname_api_all_off, path);
    evas_object_image_file_set(obj, buf, NULL);
    /* move and resize in the image */
    evas_object_move(obj, tinfo->coord_x, tinfo->coord_y - ICO_HS_SIZE_SB_HEIGHT);
    evas_object_resize(obj, tinfo->size_x, tinfo->size_y);
    /* show the image */
    evas_object_show(obj);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_ecore_timer_event
 *          handler called when applicaton tile is pushed long term
 *          to terminate application.
 *
 * @param[in]   data                user data
 * @return      result
 * @retval      ECORE_CALLBACK_CANCEL
 *                                  delete this handler
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
hs_ecore_timer_event(void *data)
{
    int idx = *(int *)data;
    char buf[ICO_HS_TEMP_BUF_SIZE];
    char tmp[ICO_HS_TEMP_BUF_SIZE];
    hs_tile_info_t *tinfo;

    tinfo = &hs_tile_info[idx];
    if (tinfo->l_press == 1) {
        /* flag reset */
        tinfo->l_press = 0;
        /* set terminate app id for onscrAppliKill */
        if (strlen(tinfo->appid) != 0) {
            strncpy(hs_kill_appid, tinfo->appid, sizeof(hs_kill_appid));
            uifw_trace("hs_ecore_timer_event :hs_kill_appid = %s", hs_kill_appid);
            strncpy(hs_active_onscreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN,
                    sizeof(hs_active_onscreen));
            sprintf(buf, "OPEN HomeScreen/src/appli_kill.edj %s",
                    ICO_UXF_PROC_DEFAULT_HOMESCREEN);
            hs_get_ons_edj_path(tmp, sizeof(tmp));
            sprintf(buf, "OPEN %s%s %s", tmp, ICO_HS_ONS_APPLI_KILL_NAME,
                    ICO_UXF_PROC_DEFAULT_HOMESCREEN);
            uifw_trace("hs_ecore_timer_event: send %s to onscreen", buf);
            hs_lib_event_message(ICO_HS_PROTOCOL_TYPE_OS, buf);
        }
    }
    return ECORE_CALLBACK_CANCEL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_touch_up_tile
 *          processing when application tile button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_touch_up_tile(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    int idx = *(int *)data;
    int ret;
    char buf[64];
    hs_tile_info_t *tinfo;

    uifw_trace("hs_touch_up_tile: idx=%d", idx);
    tinfo = &hs_tile_info[idx];
    if (tinfo->l_press == 1) {
        /* flag reset */
        tinfo->l_press = 0;
        /* send a message to extra tools */
        if (strlen(tinfo->appid) != 0) {
            sprintf(buf, "FOCUS %s", tinfo->appid);
            uifw_trace("hs_touch_up_tile: send '%s' to extra tools", buf);
            ret = hs_lib_event_message(ICO_HS_PROTOCOL_TYPE_CM, buf);
            if (ret < 0) {
                hs_show_appscreen(tinfo->appid);

                hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
            }
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_touch_down_tile
 *          processing when application tile button touch down.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_touch_down_tile(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    int idx = *(int *)data;
    hs_tile_info_t *tinfo;

    uifw_trace("hs_touch_down_tile: idx=%d", idx);

    tinfo = &hs_tile_info[idx];
    tinfo->l_press = 1;
    if (strlen(tinfo->appid) != 0) {
    }

    if (tinfo->type > ICO_HS_TILE_OTHERS) {
        /* add timer to detect long push */
        ecore_timer_add(ICO_HS_TOUCH_TIME_OUT, hs_ecore_timer_event, &tinfo->idx);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_add_bg_image
 *          add image to background. if config does not have image file or
 *          image file is not exsit, background show default edj.
 *
 * @param[in]   canvas_bg           evas of background
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_add_bg_image(Evas *canvas_bg)
{
    char img[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];
    char *fname;
    Evas_Object *canvas;
    Evas_Load_Error err;
    int dispW, dispH;

    fname = (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                       ICO_HS_CONFIG_BG, NULL);
    if (!fname) {
        uifw_trace("hs_add_bg_image: backgound config is not exist");
        return;
    }
    if (fname[0] == '/') {
        snprintf(img, sizeof(img), "%s", fname);
    }
    else {
        hs_get_image_path(path, sizeof(path));
        snprintf(img, sizeof(img), "%s/%s", path, fname);
    }

    ico_uxf_window_screen_size_get(&dispW, &dispH);

    canvas = evas_object_image_filled_add(canvas_bg);
    evas_object_image_file_set(canvas, img, NULL);
    err = evas_object_image_load_error_get(canvas);
    if (err != EVAS_LOAD_ERROR_NONE) {
        uifw_trace("hs_add_bg_image: backgound image is not exist");
    }
    else {
        evas_object_image_fill_set(canvas, 0, 0, dispW, dispH - ICO_HS_SIZE_SB_HEIGHT);
        evas_object_resize(canvas, dispW, dispH - ICO_HS_SIZE_SB_HEIGHT);
        evas_object_show(canvas);
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_add_touch_listener
 *          make and add listener to application tile button.
 *
 * @param[in]   canvas_fg           evas of tile screen
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_add_touch_listener(Evas *canvas_fg)
{
    int idx;
    char img[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];
    Evas_Object * canvas;
    hs_tile_info_t *tinfo;

    /* set image path */
    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"fname_api_all_off, path);

    /* setup tile */
    for (idx = 0; idx < hs_tile_cnt; idx++) {
        tinfo = &hs_tile_info[idx];
        if (tinfo->valid == 0)
            continue;
        /* Creates a reclangle */
        canvas = evas_object_rectangle_add(canvas_fg);
        /* Set a color or image */
        evas_object_color_set(canvas, 0, 0, 0, 0);
        /* Move and resize the rectangle */
        evas_object_move(canvas, tinfo->coord_x, tinfo->coord_y - ICO_HS_SIZE_SB_HEIGHT);
        evas_object_resize(canvas, tinfo->size_x, tinfo->size_y);
        /* add event the rectangle */
        evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_UP,
                                       hs_touch_up_tile, &tinfo->idx);
        evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_DOWN,
                                       hs_touch_down_tile, &tinfo->idx);
        /* show the rectangle */
        evas_object_show(canvas);
    }
    /* application menu icon */
    tinfo = &hs_tile_info[hs_tile_cnt - 1];
    if ((tinfo->valid == 0) && (tinfo->type == ICO_HS_TILE_APPLIST)) {
        canvas = evas_object_image_filled_add(canvas_fg);
        evas_object_image_file_set(canvas, img, NULL);
        evas_object_move(canvas, tinfo->coord_x, tinfo->coord_y - ICO_HS_SIZE_SB_HEIGHT);
        evas_object_resize(canvas, tinfo->size_x, tinfo->size_y);
        evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_UP,
                                       hs_touch_up_api_list, &tinfo->idx);
        evas_object_event_callback_add(canvas, EVAS_CALLBACK_MOUSE_DOWN,
                                       hs_touch_down_api_list, &tinfo->idx);
        evas_object_show(canvas);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_create_window
 *          create background and tile screen window of ecore.
 *
 * @param[in]   type                background or tile screen
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void *
hs_create_window(int type)
{
    Evas_Object *edje;
    Ecore_Evas * window;
    Evas * canvas;
    int dispW, dispH;
    int width, height;
    char path[ICO_HS_TEMP_BUF_SIZE];
    char edjfile[ICO_HS_TEMP_BUF_SIZE];

    window = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");

    ecore_main_loop_iterate();
    ico_uxf_window_screen_size_get(&dispW, &dispH);
    width = dispW;
    height = dispH - ICO_HS_SIZE_SB_HEIGHT;

    if (!window) {
        uifw_warn("HomeScreen: could not create window.");
        return NULL;
    }

    canvas = ecore_evas_get(window);
    edje = edje_object_add(canvas);
    /* NULL is aborted */
    if (!edje) {
        uifw_warn("HomeScreen: could not create edje object!");
        return NULL;
    }

    hs_get_edj_path(path, sizeof(path));

    if (type == ICO_HS_WINDOW_TYPE_BG) {
        ecore_evas_move(window, 0, 0);
        ecore_evas_resize(window, width, height);
        ecore_evas_show(window);
        {
            int err = edje_object_load_error_get(edje);
            const char *errmsg = edje_load_error_str(err);
            sprintf(edjfile, "%s/home_screen_bg.edj", path);
            if (!edje_object_file_set(edje, edjfile, "home_screen_bg")) {
                uifw_warn(
                          "HomeScreen: could not load 'home_screen_bg' from %s: %s",
                          edjfile, errmsg);
                evas_object_del(edje);
                return NULL;
            }
        }
        evas_object_move(edje, 0, 0);
        evas_object_resize(edje, width, height);
        evas_object_show(edje);

        hs_add_bg_image(canvas);
    }
    else {
        ecore_evas_move(window, 0, 0);
        ecore_evas_resize(window, width, height);
        ecore_evas_alpha_set(window, EINA_TRUE);
        ecore_evas_show(window);
        {
            int err = edje_object_load_error_get(edje);
            const char *errmsg = edje_load_error_str(err);
            sprintf(edjfile, "%s/home_screen_touch.edj", path);
            if (!edje_object_file_set(edje, edjfile, "home_screen_touch")) {
                uifw_warn(
                          "HomeScreen: could not load 'home_screen_touch' from %s: %s",
                          edjfile, errmsg);
                evas_object_del(edje);
                return NULL;
            }
        }
        evas_object_move(edje, 0, 0);
        evas_object_resize(edje, width, height);
        evas_object_show(edje);

        hs_add_touch_listener(canvas);
    }

    return edje;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_display_control
 *          callback function
 *
 * @param[in]   conf                configuration
 * @param[in]   show                show
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_display_control(const Ico_Uxf_conf_application *conf, const int show)
{
    uifw_trace("hs_display_control: app=%s show=%d", conf->appid, show);
    /* no need, because all control in AppsController       */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_sound_control
 *          callback function
 *
 * @param[in]   conf                configuration
 * @param[in]   adjust              adjust
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_sound_control(const Ico_Uxf_conf_application *conf, const int adjust)
{
    uifw_trace("hs_sound_control: app=%s adjust=%d", conf->appid, adjust);
    /* no need, because all control in AppsController       */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_input_control
 *          callback function
 *
 * @param[in]   conf                configuration
 * @param[in]   inputsw             input switch
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_input_control(const Ico_Uxf_conf_application *conf, const int inputsw)
{
    uifw_trace("hs_input_control: app=%s inputsw=%d", conf->appid, inputsw);
    /* no need, because all control in AppsController       */
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_regulation_listener: change regulation callback(static function)
 *
 * @param[in]   appcategory     category Id
 * @param[in]   control         control(display/sound/input active/inactive)
 * @param[in]   user_data       user data(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_regulation_listener(const int appcategory,
                       const ico_apc_reguration_control_t control, void *user_data)
{
    if ((appcategory == hs_regulation_category) &&
        (control.display != ICO_SYC_APC_REGULATION_NOCHANGE))   {
        uifw_trace("hs_regulation_listener: Enter(category=%d disp=%d)",
                   appcategory, control.display);

        if (control.display == ICO_SYC_APC_REGULATION_REGULATION)   {
            /* Invisible by reguration      */
            if (hs_regulation_visible)  {
                hs_regulation_visible = 0;
                if (hs_stat_touch == ICO_HS_TOUCH_IN_SHOW) {
                    uifw_trace("hs_regulation_listener: Change to App screen");
                    hs_show_appscreen(NULL);
                }
                hs_hide_onscreen();
        		memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
            }
        }
        else    {
            /* Visible                      */
            uifw_trace("hs_regulation_listener: Change to ALl screen");
            hs_regulation_visible = 1;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   main
 *          homescreen main. initialize UXF, app manager, and ecore.
 *
 * @param[in]   argc                counts of argment
 * @param[in]   argv                argment
 * @return      result
 * @retval      >=0                 success
 * @retval      -1                  error
 */
/*--------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    int ii = 0;
    int ret;
    int port;

    /* get pkg name */
    char *pkg;
    printf("main: %s: %s", getenv("HOME"), getenv("PKG_NAME"));
    pkg = getenv("PKG_NAME");
    memset(hs_name_homescreen, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
    if (pkg) {
        strncpy(hs_name_homescreen, pkg, ICO_UXF_MAX_PROCESS_NAME);
    }
    else {
        strncpy(hs_name_homescreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN,
                ICO_UXF_MAX_PROCESS_NAME);
    }
    strncpy(gStatusbarName,
            (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                       ICO_HS_CONFIG_SB,
                                       ICO_HS_APPID_DEFAULT_SB),
            ICO_UXF_MAX_PROCESS_NAME);
    strncpy(gOnscreenName,
            (char *)hs_conf_get_string(ICO_HS_CONFIG_HOMESCREEN,
                                       ICO_HS_CONFIG_ONS,
                                       ICO_HS_APPID_DEFAULT_ONS),
            ICO_UXF_MAX_PROCESS_NAME);
    hs_snd_init();

    /* init for window mgr */
    ret = ico_uxf_init(hs_name_homescreen);
    if (ret != ICO_UXF_EOK) {
        uifw_error("ico_uxf_init err=%d", ret);
        exit(8);
    }
    ico_uxf_callback_set(ICO_UXF_EVENT_ALL, hs_uxf_event, 0);
    memset(hs_app_screen_window, 0, sizeof(hs_app_screen_window));
    for (ii = 0; ii < ICO_HS_APP_NUM_MAX; ii++) {
        hs_app_screen_window[ii].resize_w = -1;
        hs_app_screen_window[ii].resize_h = -1;
        hs_app_screen_window[ii].move_x = -1;
        hs_app_screen_window[ii].move_y = -1;
    }
    ico_uxf_main_loop_iterate();

    /* init tile info */
    ret = hs_tile_init_info();
    if (ret < 0) {
        uifw_warn("hs_tile_init_info err=%d", ret);
    }

    /* start onscreen & statusbar apps */
    ret = ico_uxf_process_execute(gOnscreenName);
    if (ret < 0) {
        uifw_warn("ico_uxf_process_execute(%s) err=%d", gOnscreenName, ret);
    }
    ret = ico_uxf_process_execute(gStatusbarName);
    if (ret < 0) {
        uifw_warn("ico_uxf_process_execute(%s) err=%d", gStatusbarName, ret);
    }
    ico_uxf_main_loop_iterate();

    /* init EFL */
    ecore_evas_init();
    edje_init();

    /* for communicate other process */
    port = hs_conf_get_integer(ICO_HS_CONFIG_HOMESCREEN,
                               ICO_HS_CONFIG_WS_PORT,
                               ICO_HS_WS_PORT);
    hs_lib_main(port);

    /* initialize Application Controller    */
    ret = ico_syc_apc_init(hs_display_control, hs_sound_control, hs_input_control);
    if (ret != ICO_SYC_EOK) {
        uifw_error("ico_syc_apc_init err=%d", ret);
    }

    /* start default tile apps */
    hs_tile_start_apps();
    ico_uxf_main_loop_iterate();

    /* HomeScreen Menu(background) */
    hs_create_window(ICO_HS_WINDOW_TYPE_BG);
    /* add callbacks for wayland(wl_window_mgr) */
    ecore_main_fd_handler_add(ico_uxf_wl_display_fd(), ECORE_FD_READ,
                              hs_ecore_uxf_eventfd, NULL, /* data */
                              NULL, NULL);
    ecore_main_loop_iterate();
    ico_uxf_main_loop_iterate();

    /* HomeScreen Menu(touch) */
    hs_create_window(ICO_HS_WINDOW_TYPE_TOUCH);

    /* initiailze regulation control    */
    hs_regulation_category = ico_uxf_getSysCategoryByName("CarSetting");
    ico_syc_apc_regulation_listener(hs_regulation_listener, NULL);

    if (hs_regulation_visible)  {
        uifw_trace("main: no regulation");
        hs_tile_show_screen();
    }
    else    {
        uifw_trace("main: regulation");
        hs_show_appscreen(NULL);
    }

    /* add callback to app configuration */
    ico_uxf_econf_setAppUpdateCb(NULL);

    /* wait events */
    ecore_main_loop_begin();

    /* terminate Application Controller */
    (void) ico_syc_apc_term();

    /* terminate all known application */
    hs_terminate_all_app();

    /* shutdown EFL */
    edje_shutdown();
    ecore_evas_shutdown();

    return 0;
}
