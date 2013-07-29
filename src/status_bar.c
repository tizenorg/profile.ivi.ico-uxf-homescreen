/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   statusbar application
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <bundle.h>

#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>

#include <ico_uws.h>

#include "ico_uxf.h"
#include "ico_uxf_conf.h"
#include "ico_uxf_conf_ecore.h"

#include "home_screen.h"
#include "home_screen_res.h"
#include "home_screen_conf.h"

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_SB_TIME_AM        (10) /* f_name in sb_time_data */
#define ICO_SB_TIME_PM        (11) /* f_name in sb_time_data */
#define ICO_SB_TIME_IMG_PARTS (12) /* total counts of sb_time_data */
#define ICO_SB_TIMEOUT        (0.2)

#define ICO_SB_WS_TIMEOUT     (0.05)
#define ICO_SB_WS_ADDRESS     "127.0.0.1"
#define ICO_SB_WS_PROTOCOL_NAME   ICO_HS_PROTOCOL_SB
#define ICO_SB_BUF_SIZE       (1024)
#define ICO_SB_VERTICAL       (1)
#define ICO_SB_HORIZONTAL     (2)

#define ICO_SB_POS_Y          (0)
#define ICO_SB_POS_COLON_X    (689)
#define ICO_SB_POS_COLON_Y    (30)
#define ICO_SB_POS_AMPM_X     (582)
#define ICO_SB_POS_AMPM_Y     (27)
#define ICO_SB_POS_HOUR1_X    (640)
#define ICO_SB_POS_HOUR1_Y    (18)
#define ICO_SB_POS_HOUR2_X    (662)
#define ICO_SB_POS_HOUR2_Y    (18)
#define ICO_SB_POS_MIN1_X     (703)
#define ICO_SB_POS_MIN1_Y     (18)
#define ICO_SB_POS_MIN2_X     (725)
#define ICO_SB_POS_MIN2_Y     (18)

#define ICO_SB_SIZE_SHTCT_W   (ICO_HS_SIZE_SB_HEIGHT)

#define ICO_SB_POS_LIST_X     (ICO_HS_SIZE_SB_HEIGHT * 1)
#define ICO_SB_POS_SHTCT1_X   (ICO_SB_POS_LIST_X + ICO_HS_SIZE_SB_HEIGHT + ICO_HS_SIZE_SB_HEIGHT * 2 / 2)
#define ICO_SB_POS_SHTCT2_X   (ICO_SB_POS_SHTCT1_X + ICO_HS_SIZE_SB_HEIGHT + ICO_HS_SIZE_SB_HEIGHT * 1 / 2)

#define ICO_SB_SIZE_COLON_W   (6)
#define ICO_SB_SIZE_COLON_H   (17)
#define ICO_SB_SIZE_NUM_W     (20)
#define ICO_SB_SIZE_NUM_H     (30)
#define ICO_SB_SIZE_AMPM_W    (47)
#define ICO_SB_SIZE_AMPM_H    (27)

#define ICO_SB_CHTCT_MAX      (5)

#define ICO_SB_NO_WAIT        (1)
#define ICO_SB_WAIT_REPLY     (2)

#define ICO_SB_APPLIST_OFFICON "applist_off.png"
#define ICO_SB_APPLIST_ONICON "applist_on.png"
#define ICO_SB_HOME_OFFICON "home_off.png"
#define ICO_SB_HOME_ONICON "home_on.png"

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static void sb_on_destroy(Ecore_Evas *ee);
static void sb_callback_uws(const struct ico_uws_context *context,
                            const ico_uws_evt_e event, const void *id,
                            const ico_uws_detail *detail, void *data);
static void sb_create_ws_context(void);
static void sb_time_hour(struct tm *t_st);
static void sb_time_min(struct tm *t_st);
static Eina_Bool sb_time_show(void* thread_data);
static void sb_touch_up_escathion(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void sb_touch_down_escathion(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void sb_touch_up_applist(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void sb_touch_down_applist(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void sb_clock_display_fixation(Evas *canvas);
static void sb_touch_up_shortcut(void *data, Evas *evas, Evas_Object *obj, void *event_info);
static void sb_config_event(const char *appid, int type);
static void sb_add_shortcut(Evas *canvas);

/*============================================================================*/
/* variabe & table                                                            */
/*============================================================================*/
static Evas *sb_canvas;
static Eina_List *sb_shtct_list = NULL;
static int sb_wait_reply = ICO_SB_NO_WAIT;
static int sb_ws_port = ICO_HS_WS_PORT;
static int sb_ws_connected = 0;
static struct ico_uws_context *sb_uws_context = NULL;
static void *sb_uws_id = NULL;

static int sb_width = 0;
static char sb_respath[ICO_SB_BUF_SIZE];
static struct tm sb_st_buf;
static Evas_Object *sb_ampm;
static Evas_Object *sb_hour1;
static Evas_Object *sb_hour2;
static Evas_Object *sb_min1;
static Evas_Object *sb_min2;

struct _sb_time_data {
    char f_name[64];
    Evas_Object *time_img;
};

struct _sb_time_data sb_time_data[ICO_SB_TIME_IMG_PARTS] = {
       {fname_num0, }, {fname_num1, },
       {fname_num2, }, {fname_num3, },
       {fname_num4, }, {fname_num5, },
       {fname_num6, }, {fname_num7, },
       {fname_num8, }, {fname_num9, },
       {fname_am, }, {fname_pm, }
};

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/*
 * @brief   sb_on_destroy
 *          callback function called by EFL when ecore destroyed.
 *          exit ecore main loop.
 *
 * @param[in]   ee                  ecore evas object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_on_destroy(Ecore_Evas *ee)
{
    /* Quits the main loop */
    ecore_main_loop_quit();
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   sb_callback_uws
 *          callback function from UWS
 *
 * @param[in]   context             context
 * @param[in]   event               event kinds
 * @param[in]   id                  client id
 * @param[in]   detail              event detail
 * @param[in]   data                user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_callback_uws(const struct ico_uws_context *context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data)
{
    uifw_trace("sb_callback_uws %p", context);
    unsigned char msg[ICO_HS_TEMP_BUF_SIZE];
    char *in;
    int len;

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_OPEN(id=%d)", (int)id);
        sb_uws_id = (void *)id;
        len = sprintf((char *)msg, "%s %s", ICO_HS_MSG_HEAD_SB, "ANS HELLO");
        ico_uws_send((struct ico_uws_context *)context, (void *)id, msg, len);
        break;

    case ICO_UWS_EVT_CLOSE:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_CLOSE(id=%d)", (int)id);
        sb_uws_context = NULL;
        sb_ws_connected = 0;
        sb_uws_id = NULL;
        break;

    case ICO_UWS_EVT_RECEIVE:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_RECEIVE(id=%d, msg=%s, len=%d)",
                   (int)id, (char *)detail->_ico_uws_message.recv_data,
                   detail->_ico_uws_message.recv_len);
        in = (char *)detail->_ico_uws_message.recv_data;
        if (strncmp("RECEIVE OK", in, 10) == 0) {
            sb_wait_reply = ICO_SB_NO_WAIT;
        }
        else {
            len = sprintf((char *)msg, "%s ANS %s OK", ICO_HS_MSG_HEAD_SB, (char *)in);
            ico_uws_send((struct ico_uws_context *)context, (void *)id, msg, len);
        }
        break;

    case ICO_UWS_EVT_ERROR:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_ERROR(id=%d, err=%d)",
                   (int)id, detail->_ico_uws_error.code);
        break;

    case ICO_UWS_EVT_ADD_FD:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_ADD_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;

    case ICO_UWS_EVT_DEL_FD:
        uifw_trace("sb_callback_uws: ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;

    default:
        break;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_create_ws_context
 *          connect to the homescreen using websocket.
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_create_ws_context(void)
{
    int ret;
    char uri[ICO_HS_TEMP_BUF_SIZE];

    /* set up URI "ws://HOST:PORT" */
    sprintf(uri, "ws://%s:%d", ICO_HS_WS_HOST, sb_ws_port);

    /* create context */
    sb_uws_context = ico_uws_create_context(uri, ICO_HS_PROTOCOL);
    uifw_trace("sb_create_ws_context: ctx = %p", sb_uws_context);

    sb_ws_connected = 0;
    if (sb_uws_context == NULL) {
        uifw_trace("sb_create_ws_context: libwebsocket_create_context failed.");
    }
    else {
        /* set callback */
        ret = ico_uws_set_event_cb(sb_uws_context, sb_callback_uws, NULL);
        if (ret != ICO_UWS_ERR_NONE) {
            uifw_trace("sb_create_ws_context: cannnot set callback");
        }
        sb_ws_connected = 1;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_ecore_event
 *          timer handler called by Ecore.
 *
 * @param[in]   data                user data
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
sb_ecore_event(void *data)
{
    if (sb_ws_connected) {
        ico_uws_service(sb_uws_context);
    }
    else {
        if (sb_uws_context != NULL) {
            ico_uws_close(sb_uws_context);
            sb_uws_context = NULL;
        }
        sb_create_ws_context();
    }

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_time_hour
 *          show the clock's hour image on statubar
 *
 * @param[in]   tm                  time data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_time_hour(struct tm *t_st)
{
    char file[ICO_SB_BUF_SIZE];
    if (t_st->tm_hour < 12) {
        sprintf(file, "%s%s", sb_respath, sb_time_data[ICO_SB_TIME_AM].f_name);
        evas_object_image_file_set(sb_ampm, file, NULL);
        sprintf(file, "%s%s", sb_respath,
                sb_time_data[t_st->tm_hour / 10].f_name);
        evas_object_image_file_set(sb_hour1, file, NULL);
        sprintf(file, "%s%s", sb_respath,
                sb_time_data[t_st->tm_hour % 10].f_name);
        evas_object_image_file_set(sb_hour2, file, NULL);
    }
    else {
        sprintf(file, "%s%s", sb_respath, sb_time_data[ICO_SB_TIME_PM].f_name);
        evas_object_image_file_set(sb_ampm, file, NULL);
        sprintf(file, "%s%s", sb_respath, sb_time_data[(t_st->tm_hour - 12)
                / 10].f_name);
        evas_object_image_file_set(sb_hour1, file, NULL);
        sprintf(file, "%s%s", sb_respath, sb_time_data[(t_st->tm_hour - 12)
                % 10].f_name);
        evas_object_image_file_set(sb_hour2, file, NULL);
    }

    /*Makes the given Evas object visible*/
    evas_object_show(sb_ampm);
    evas_object_show(sb_hour1);
    evas_object_show(sb_hour2);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_time_hour
 *          show the clock's minite image on statubar
 *
 * @param[in]   tm                  time data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_time_min(struct tm *t_st)
{
    char file[ICO_SB_BUF_SIZE];
    sprintf(file, "%s%s", sb_respath, sb_time_data[t_st->tm_min / 10].f_name);
    evas_object_image_file_set(sb_min1, file, NULL);
    sprintf(file, "%s%s", sb_respath, sb_time_data[t_st->tm_min % 10].f_name);
    evas_object_image_file_set(sb_min2, file, NULL);

    /*Makes the given Evas object visible*/
    evas_object_show(sb_min1);
    evas_object_show(sb_min2);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_time_show
 *          callback function called by Ecore.
 *          get time data, and show the clock's image on statubar.
 *
 * @param[in]   thread_data         user data
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
sb_time_show(void *thread_data)
{
    time_t timer;
    struct tm *t_st, *old_t_st;

    old_t_st = &sb_st_buf;

    /* get time */
    time(&timer);
    t_st = localtime(&timer);

    if (old_t_st->tm_hour != t_st->tm_hour) {
        /* hour file set */
        sb_time_hour(t_st);
    }
    if (old_t_st->tm_min != t_st->tm_min) {
        /* min file set */
        sb_time_min(t_st);
    }
    memcpy(old_t_st, t_st, sizeof(struct tm));

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_touch_up_shortcut
 *          processing when application button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   evas                evas of the button
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_up_shortcut(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    unsigned char msg[ICO_HS_TEMP_BUF_SIZE];
    int len;
    char *appid = (char *)data;

    uifw_trace("TOUCH UP: SHORTCUT %s", appid);

    if (sb_wait_reply == ICO_SB_NO_WAIT) {
        if ((sb_uws_id != NULL) && (appid != NULL)) {
            len = sprintf((char *)msg, "%s SHOW %s %s", ICO_HS_MSG_HEAD_SB, appid, getenv("PKG_NAME"));
            ico_uws_send(sb_uws_context, sb_uws_id, msg, len);
            uifw_trace("SB: SHOW %s", appid);
            sb_wait_reply = ICO_SB_WAIT_REPLY;
        }
        /* operation sound */
        hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_touch_down_applist
 *          processing when application button touch down.
 *
 * @param[in]   data                user data
 * @param[in]   evas                evas of the button
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_down_applist(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    char img[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];

    uifw_trace("TOUCH DOWN: APPLIST");

    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_APPLIST_ONICON, path);

    evas_object_image_file_set(obj, img, NULL);
    evas_object_show(obj);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_touch_up_applist
 *          processing when application button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   evas                evas of the button
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_up_applist(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    unsigned char msg[ICO_HS_TEMP_BUF_SIZE];
    int len;
    char path[ICO_HS_TEMP_BUF_SIZE];
    char img[ICO_HS_TEMP_BUF_SIZE];

    uifw_trace("TOUCH UP: APPLIST");

    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_APPLIST_OFFICON, path);

    evas_object_image_file_set(obj, img, NULL);
    evas_object_show(obj);

    if (sb_wait_reply == ICO_SB_NO_WAIT) {
        if (sb_uws_id != NULL) {
            hs_get_ons_edj_path(path, sizeof(path));
            len = sprintf((char *)msg, "%s OPEN %s%s %s", ICO_HS_MSG_HEAD_SB, path,
                    ICO_HS_ONS_APPLI_LIST_NAME, getenv("PKG_NAME"));
            ico_uws_send(sb_uws_context, sb_uws_id, msg, len);
            uifw_trace("SB: CLICK APPLIST");
            sb_wait_reply = ICO_SB_WAIT_REPLY;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_touch_down_escathion
 *          processing when escathion button touch down.
 *
 * @param[in]   data                user data
 * @param[in]   evas                evas of the button
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_down_escathion(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    char img[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];

    uifw_trace("TOUCH DOWN: CHANGE");

    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_HOME_ONICON, path);

    evas_object_image_file_set(obj, img, NULL);
    evas_object_show(obj);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_touch_up_escathion
 *          processing when change button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   evas                evas of the button
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_up_escathion(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    unsigned char msg[ICO_HS_TEMP_BUF_SIZE];
    int len;
    char path[ICO_HS_TEMP_BUF_SIZE];
    char img[ICO_HS_TEMP_BUF_SIZE];

    uifw_trace("TOUCH UP: CHANGE");

    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_HOME_OFFICON, path);

    evas_object_image_file_set(obj, img, NULL);
    evas_object_show(obj);

    if (sb_wait_reply == ICO_SB_NO_WAIT) {
        if (sb_uws_id != NULL) {
            len = sprintf((char *)msg, "%s %s", ICO_HS_MSG_HEAD_SB, "CLICK ESCUTCHEON 1");
            ico_uws_send(sb_uws_context, sb_uws_id, msg, len);
            uifw_trace("SB: CLICK ESCUTCHEON 1");
            sb_wait_reply = ICO_SB_WAIT_REPLY;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_clock_display_fixation
 *          set up fixed images on status bar
 *
 * @param[in]   canvas              evas to draw
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_clock_display_fixation(Evas *canvas)
{
    Evas_Object *tile, *colon;
    int moveH;
    int escPosX;
    char file[ICO_SB_BUF_SIZE];
    char img[ICO_HS_TEMP_BUF_SIZE];
    char path[ICO_HS_TEMP_BUF_SIZE];

    moveH = sb_width - ICO_HS_SIZE_LAYOUT_WIDTH;
    escPosX = sb_width / 2 - ICO_HS_SIZE_SB_HEIGHT / 2;

    /* show escutcheon */
    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_HOME_OFFICON, path);
    tile = evas_object_image_filled_add(canvas);
    evas_object_image_file_set(tile, img, NULL);
    evas_object_move(tile, escPosX, ICO_SB_POS_Y);
    evas_object_resize(tile, ICO_HS_SIZE_SB_HEIGHT, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_UP,
                                  sb_touch_up_escathion, NULL);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_DOWN,
                                  sb_touch_down_escathion, NULL);
    evas_object_show(tile);

    /* show app list */
    hs_get_image_path(path, sizeof(path));
    sprintf(img, "%s"ICO_SB_APPLIST_OFFICON, path);
    tile = evas_object_image_filled_add(canvas);
    evas_object_image_file_set(tile, img, NULL);
    evas_object_move(tile, ICO_SB_POS_LIST_X, ICO_SB_POS_Y);
    evas_object_resize(tile, ICO_HS_SIZE_SB_HEIGHT * 3 / 2, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_UP,
                                  sb_touch_up_applist, NULL);
    evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_DOWN,
                                  sb_touch_down_applist, NULL);
    evas_object_show(tile);

    /* shortcut bottun */
    sb_add_shortcut(canvas);

    /* show clock's colon */
    /*Creates a new image object*/
    colon = evas_object_image_filled_add(canvas);
    /*Set the image file */
    sprintf(file, "%s%s", sb_respath, fname_colon);
    evas_object_image_file_set(colon, file, NULL);
    /*This function will make layout change*/
    evas_object_move(colon, moveH + ICO_SB_POS_COLON_X, ICO_SB_POS_COLON_Y);
    /*This function will make size change of a picture*/
    evas_object_resize(colon, ICO_SB_SIZE_COLON_W, ICO_SB_SIZE_COLON_H);
    /*Makes the given Evas object visible*/
    evas_object_show(colon);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_add_shortcut
 *          add shurtcut bottun
 *
 * @param[in]   canvas              evas to draw
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_add_shortcut(Evas *canvas)
{
    Evas_Object *tile;
    int escPosX;
    int x, idx;
    char config[ICO_UXF_MAX_PROCESS_NAME];
    const char *appid;
    const Ico_Uxf_conf_application *appconf;
    Eina_List *l, *l_next;

    escPosX = sb_width / 2 - ICO_HS_SIZE_SB_HEIGHT / 2;

    /* delete shortcut */
    EINA_LIST_FOREACH_SAFE(sb_shtct_list, l, l_next, tile) {
        evas_object_del(tile);
        sb_shtct_list = eina_list_remove_list(sb_shtct_list, l);
    }

    /* add shortcut */
    for (idx = 0; idx < ICO_SB_CHTCT_MAX; idx++) {
        sprintf(config, ICO_SB_CONFIG_SHTCT_APP"%d", idx);
        x = ICO_SB_POS_SHTCT1_X + (ICO_SB_SIZE_SHTCT_W + ICO_HS_SIZE_SB_HEIGHT * 1 / 2) * idx;
        if (x > escPosX) {
            break;
        }
        appid = hs_conf_get_string(ICO_HS_CONFIG_STATUBAR, config, NULL);
        if ((appid != NULL) && (strcmp(appid, "none") != 0)) {
            appconf = ico_uxf_getAppByAppid(appid);
            if (appconf) {
                tile = evas_object_image_filled_add(canvas);
                evas_object_image_file_set(tile,
                        appconf->icon_key_name, NULL);
                evas_object_move(tile, x, ICO_SB_POS_Y);
                evas_object_resize(tile, ICO_HS_SIZE_SB_HEIGHT, ICO_HS_SIZE_SB_HEIGHT);
                evas_object_event_callback_add(tile, EVAS_CALLBACK_MOUSE_UP,
                                   sb_touch_up_shortcut, appid);
                evas_object_show(tile);
                sb_shtct_list = eina_list_append(sb_shtct_list, tile);
            }
        }
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   sb_config_event
 *          This is a callback function called when the configurations
 *          is updata.
 *
 * @param[in]   appid               application id
 * @param[in]   type                event type(install/uninstall)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_config_event(const char *appid, int type)
{
    uifw_trace("sb_config_event: Enter(appid=%s, type=%d)", appid, type);

    sb_add_shortcut(sb_canvas);

    uifw_trace("sb_config_event: Leave");

    return;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   status bar application
 *          main routine
 *
 * @param   main() finction's standard parameter (argc,argv)
 * @return  result
 * @retval  0       success
 * @retval  1       failed
 */
/*--------------------------------------------------------------------------*/
int
main(int argc, char *argv[])
{
    time_t timer;
    struct tm *t_st;
    Evas_Object *bg;
    char file[ICO_SB_BUF_SIZE];
    int width, height;
    int moveH = 0;
    int orientation = ICO_SB_HORIZONTAL;
    int ii;
    int ret;
    static Ecore_Evas *ee;
    bundle *b;
    const char *val;

    /* configure */
    hs_get_image_path(sb_respath, ICO_SB_BUF_SIZE);

    /* configure */
    if (getenv("PKG_NAME")) {
        ico_uxf_log_open(getenv("PKG_NAME"));
    }
    else {
        ico_uxf_log_open(ICO_HS_APPID_DEFAULT_SB);

    }

    /* get argment */
    b = bundle_import_from_argv(argc, argv);
    val = bundle_get_val(b, "orientation");
    if (val != NULL) {
        if (strcmp(val, "vertical") == 0) {
            orientation = ICO_SB_VERTICAL;
        }
        else if (strcmp(val, "horizontal") == 0) {
            orientation = ICO_SB_HORIZONTAL;
        }
    }

    /* load configuration */
    ret = initHomeScreenConfig(ICO_STATUSBAR_CONFIG_FILE);
    if (ret == ICO_HS_OK) {
        sb_ws_port = hs_conf_get_integer(ICO_HS_CONFIG_STATUBAR,
                                         ICO_HS_CONFIG_WS_PORT,
                                         ICO_HS_WS_PORT);
        orientation = hs_conf_get_integer(ICO_HS_CONFIG_STATUBAR,
                                          ICO_HS_CONFIG_ORIENTAION,
                                          orientation);
    }
    hs_snd_init();

    /* Initialize a new system of Ecore_Evas */
    ecore_evas_init();

    /* Generate Ecore_Evas */
    ee = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");
    if (!ee)
        goto error;

    ecore_main_loop_iterate();
    ecore_wl_screen_size_get(&width, &height);

    if (orientation == ICO_SB_VERTICAL) {
        sb_width = width > height ? height : width;
        moveH = sb_width - ICO_HS_SIZE_LAYOUT_WIDTH;
    }
    else {
        sb_width = width < height ? height : width;
        moveH = sb_width - ICO_HS_SIZE_LAYOUT_WIDTH;
    }

    /* Set the window size of the maximum and minimum */
    ecore_evas_size_min_set(ee, sb_width, ICO_HS_SIZE_SB_HEIGHT);
    ecore_evas_size_max_set(ee, sb_width, ICO_HS_SIZE_SB_HEIGHT);

    /* Set a callback for Ecore_Evas delete request events */
    ecore_evas_callback_delete_request_set(ee, sb_on_destroy);

    /* Set the title of an Ecore_Evas window */
    /* ecore_evas_title_set(ee, "Ecore_Evas buffer (image) example"); */

    ecore_evas_move(ee, 0, 0);
    ecore_evas_resize(ee, sb_width, ICO_HS_SIZE_SB_HEIGHT);
    ecore_evas_show(ee);

    /* Wrapper of Evas */
    sb_canvas = ecore_evas_get(ee);

    /* BG color set */
    bg = evas_object_rectangle_add(sb_canvas);
    evas_object_color_set(bg, 0, 0, 0, 255);
    evas_object_move(bg, 0, 0);
    evas_object_resize(bg, sb_width, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_show(bg);

    /* Call the function */
    sb_clock_display_fixation(sb_canvas);

    /* ... */
    sb_ampm = evas_object_image_filled_add(sb_canvas);
    sb_hour1 = evas_object_image_filled_add(sb_canvas);
    sb_hour2 = evas_object_image_filled_add(sb_canvas);
    sb_min1 = evas_object_image_filled_add(sb_canvas);
    sb_min2 = evas_object_image_filled_add(sb_canvas);

    /* This function will make layout change */
    evas_object_move(sb_ampm, moveH + ICO_SB_POS_AMPM_X, ICO_SB_POS_AMPM_Y);
    evas_object_move(sb_hour1, moveH + ICO_SB_POS_HOUR1_X, ICO_SB_POS_HOUR1_Y);
    evas_object_move(sb_hour2, moveH + ICO_SB_POS_HOUR2_X, ICO_SB_POS_HOUR2_Y);

    /* This function will make size change of picture */
    evas_object_resize(sb_ampm, ICO_SB_SIZE_AMPM_W, ICO_SB_SIZE_AMPM_H);
    evas_object_resize(sb_hour1, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(sb_hour2, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);

    /* Makes the given Evas object visible */
    evas_object_show(sb_ampm);
    evas_object_show(sb_hour1);
    evas_object_show(sb_hour2);

    /* Change default layout of Evas object */
    evas_object_move(sb_min1, moveH + ICO_SB_POS_MIN1_X, ICO_SB_POS_MIN1_Y);
    evas_object_move(sb_min2, moveH + ICO_SB_POS_MIN2_X, ICO_SB_POS_MIN2_Y);

    /* Change default size of Evas object */
    evas_object_resize(sb_min1, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);
    evas_object_resize(sb_min2, ICO_SB_SIZE_NUM_W, ICO_SB_SIZE_NUM_H);

    /* Makes the given Evas object visible */
    evas_object_show(sb_min1);
    evas_object_show(sb_min2);

    time(&timer);
    t_st = localtime(&timer);
    sb_time_hour(t_st);
    sb_time_min(t_st);

    /* Set the image file */
    for (ii = 0; ii < ICO_SB_TIME_IMG_PARTS; ii++) {
        sb_time_data[ii].time_img = evas_object_image_filled_add(sb_canvas);
        sprintf(file, "%s%s", sb_respath, sb_time_data[ii].f_name);
        evas_object_image_file_set(sb_time_data[ii].time_img, file, NULL);
    }
    ecore_timer_add(ICO_SB_TIMEOUT, sb_time_show, NULL);

    /* Init websockets */
    sb_create_ws_context();
    ecore_timer_add(ICO_SB_WS_TIMEOUT, sb_ecore_event, NULL);

    /* add callback to app configuration */
    ico_uxf_econf_setAppUpdateCb(sb_config_event);

    /* Runs the application main loop */
    ecore_main_loop_begin();
    /* Free an Ecore_Evas */
    ecore_evas_free(ee);
    /* Shut down the Ecore_Evas system */
    ecore_evas_shutdown();
    return 0;

error:
    uifw_warn("status_bar: error use ecore")
    ecore_evas_shutdown();
    return -1;
}

