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

#include <libwebsockets.h>

#include <Ecore.h>
#include <Ecore_Wayland.h>
#include <Ecore_Evas.h>
#include <Elementary.h>

#include "ico_uxf.h"
#include "ico_uxf_conf.h"

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

#define ICO_SB_SIZE_COLON_W   (6)
#define ICO_SB_SIZE_COLON_H   (17)
#define ICO_SB_SIZE_NUM_W     (20)
#define ICO_SB_SIZE_NUM_H     (30)
#define ICO_SB_SIZE_AMPM_W    (47)
#define ICO_SB_SIZE_AMPM_H    (27)

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static void sb_on_destroy(Ecore_Evas *ee);
static int sb_callback_http(struct libwebsocket_context *context, 
              struct libwebsocket *wsi,
              enum libwebsocket_callback_reasons reason, void *user, void *in,
              size_t len);
static int sb_callback_statusbar(struct libwebsocket_context *context,
                   struct libwebsocket *wsi,
                   enum libwebsocket_callback_reasons reason, void *user,
                   void *in, size_t len);
static void sb_create_ws_context(void);
static void sb_time_hour(struct tm *t_st);
static void sb_time_min(struct tm *t_st);
static Eina_Bool sb_time_show(void* thread_data);
static void sb_touch_up_escathion(void *data, Evas_Object *obj, void *event_info);
static void sb_clock_display_fixation(Evas *canvas);

/*============================================================================*/
/* variabe & table                                                            */
/*============================================================================*/
static int sb_ws_port = ICO_HS_WS_PORT;
static int sb_ws_connected = 0;
static struct libwebsocket_context *sb_ws_context;
static struct libwebsocket *sb_wsi_mirror;

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

static struct libwebsocket_protocols ws_protocols[] = {
    {
        "http-only", 
        sb_callback_http, 
        0
    }, 
    {
        "statusbar-protocol", 
        sb_callback_statusbar, 
        0, 
    }, 
    {
        /* end of list */
        NULL, 
        NULL, 
        0
    }
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
 * @brief   sb_callback_http
 *          Connection status is notified from libwebsockets.
 *
 * @param[in]   context             libwebsockets context
 * @param[in]   wsi                 libwebsockets management table
 * @param[in]   reason              event type
 * @param[in]   user                intact
 * @param[in]   in                  receive message
 * @param[in]   len                 message size[BYTE]
 * @return      result
 * @retval      =0                  success
 * @retval      =1                  error
 */
/*--------------------------------------------------------------------------*/
static int
sb_callback_http(struct libwebsocket_context *context, struct libwebsocket *wsi,
              enum libwebsocket_callback_reasons reason, void *user, void *in,
              size_t len)
{
    uifw_trace("sb_callback_http %p", context);
    uifw_trace("SB-REASON %d", reason);
    return 0;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   sb_callback_statusbar
 *          this callback function is notified from libwebsockets 
 *          statusbar protocol
 *
 * @param[in]   context             libwebsockets context
 * @param[in]   wsi                 libwebsockets management table
 * @param[in]   reason              event type
 * @param[in]   user                intact
 * @param[in]   in                  receive message
 * @param[in]   len                 message size[BYTE]
 * @return      result
 * @retval      =0                  success
 * @retval      =1                  error
 */
/*--------------------------------------------------------------------------*/
static int
sb_callback_statusbar(struct libwebsocket_context *context,
                   struct libwebsocket *wsi,
                   enum libwebsocket_callback_reasons reason, void *user,
                   void *in, size_t len)
{
    int n = 0;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512
            + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

    uifw_trace("sb_callback_statusbar %p", context);

    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        uifw_trace("SB-ESTABLISHED %x", wsi);
        sb_wsi_mirror = wsi;
        n = sprintf((char *)p, "%s", "ANS HELLO");
        break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
        uifw_trace("SB-RECEIVE[%d] %s", len, in);
        sb_wsi_mirror = wsi;
        n = sprintf((char *)p, "ANS %s OK", (char *)in);
        break;
    case LWS_CALLBACK_CLOSED:
        uifw_trace("SB-CLOSE");
        sb_wsi_mirror = NULL;
        break;
    default:
        uifw_trace("SB-REASON %d", reason);
        break;
    }

    if (n != 0) {
        n = libwebsocket_write(wsi, p, n, LWS_WRITE_TEXT);
    }

    return 0;
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
    sb_ws_context
            = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                          ws_protocols,
                                          libwebsocket_internal_extensions,
                                          NULL, NULL, -1, -1, 0);
    uifw_trace("sb_create_ws_context ctx = %p", sb_ws_context);

    sb_ws_connected = 0;
    if (sb_ws_context == NULL) {
        uifw_trace("libwebsocket_create_context failed.");
    }
    else {
        sb_wsi_mirror
                = libwebsocket_client_connect(sb_ws_context, ICO_SB_WS_ADDRESS,
                                              sb_ws_port, 0, "/",
                                              ICO_SB_WS_ADDRESS, NULL,
                                              ICO_SB_WS_PROTOCOL_NAME, -1);
        uifw_trace("sb_create_ws_context wsi = %p", sb_wsi_mirror);
        if (sb_wsi_mirror != NULL) {
            sb_ws_connected = 1;
        }
    }
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
        libwebsocket_service(sb_ws_context, 0);
    }
    else {
        if (sb_ws_context != NULL) {
            libwebsocket_context_destroy(sb_ws_context);
        }
        sb_ws_context
                = libwebsocket_create_context(CONTEXT_PORT_NO_LISTEN, NULL,
                                              ws_protocols,
                                              libwebsocket_internal_extensions,
                                              NULL, NULL, -1, -1, 0);
        if (sb_ws_context == NULL) {
            uifw_trace("libwebsocket_create_context failed.");
        }
        else {
            sb_wsi_mirror
                    = libwebsocket_client_connect(sb_ws_context,
                                                  ICO_SB_WS_ADDRESS,
                                                  sb_ws_port, 0, "/",
                                                  ICO_SB_WS_ADDRESS, NULL,
                                                  ICO_SB_WS_PROTOCOL_NAME, -1);
            if (sb_wsi_mirror != NULL) {
                sb_ws_connected = 1;
            }
        }
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
 * @brief   sb_touch_up_escathion
 *          processing when change button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
sb_touch_up_escathion(void *data, Evas_Object *obj, void *event_info)
{
    int n = 0;
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512
            + LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

    if (sb_wsi_mirror != NULL) {
        n = sprintf((char *)p, "%s", "CLICK ESCUTCHEON 1");
        libwebsocket_write(sb_wsi_mirror, p, n,
                           LWS_WRITE_CLIENT_IGNORE_XOR_MASK | LWS_WRITE_TEXT);
        uifw_trace("SB: CLICK ESCUTCHEON 1");
    }

    /* operation sound */
    hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
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
    Evas_Object *title, *colon;
    int moveH;
    int escPosX;
    char file[ICO_SB_BUF_SIZE];

    moveH = sb_width - ICO_HS_SIZE_LAYOUT_WIDTH;
    escPosX = sb_width / 2 - ICO_HS_SIZE_SB_HEIGHT / 2;

    /* show escutcheon */
    Evas_Object *icon, *btn;
    title = evas_object_rectangle_add(canvas);
    evas_object_move(title, escPosX, ICO_SB_POS_Y);
    evas_object_resize(title, ICO_HS_SIZE_SB_HEIGHT, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_show(title);

    icon = elm_icon_add(title);
    elm_icon_standard_set(icon, "refresh");

    btn = elm_button_add(title);
    elm_object_part_content_set(btn, "icon", icon);
    evas_object_move(btn, escPosX, ICO_SB_POS_Y);
    evas_object_resize(btn, ICO_HS_SIZE_SB_HEIGHT, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_smart_callback_add(btn, "clicked", sb_touch_up_escathion, NULL );
    evas_object_show(btn);

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
    Evas * canvas;
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
    for (ii = 1; ii < argc; ii++) {
        if (argv[ii][0] != '-')
            continue;
        if (strncasecmp(argv[ii], "-orientaion=", strlen("-orientaion=")) == 0) {
            if (strcmp(&argv[ii][strlen("-orientaion=")], "vertical") == 0) {
                orientation = ICO_SB_VERTICAL;
            }
            else if (strcmp(&argv[ii][strlen("-orientaion=")], "horizontal")
                    == 0) {
                orientation = ICO_SB_HORIZONTAL;
            }
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
    canvas = ecore_evas_get(ee);

    /* BG color set */
    bg = evas_object_rectangle_add(canvas);
    evas_object_color_set(bg, 0, 0, 0, 255);
    evas_object_move(bg, 0, 0);
    evas_object_resize(bg, sb_width, ICO_HS_SIZE_SB_HEIGHT);
    evas_object_show(bg);

    /* Call the function */
    elm_init(argc, argv);
    sb_clock_display_fixation(canvas);

    /* ... */
    sb_ampm = evas_object_image_filled_add(canvas);
    sb_hour1 = evas_object_image_filled_add(canvas);
    sb_hour2 = evas_object_image_filled_add(canvas);
    sb_min1 = evas_object_image_filled_add(canvas);
    sb_min2 = evas_object_image_filled_add(canvas);

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
        sb_time_data[ii].time_img = evas_object_image_filled_add(canvas);
        sprintf(file, "%s%s", sb_respath, sb_time_data[ii].f_name);
        evas_object_image_file_set(sb_time_data[ii].time_img, file, NULL);
    }
    ecore_timer_add(ICO_SB_TIMEOUT, sb_time_show, NULL);

    /* Init websockets */
    sb_create_ws_context();
    ecore_timer_add(ICO_SB_WS_TIMEOUT, sb_ecore_event, NULL);

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

