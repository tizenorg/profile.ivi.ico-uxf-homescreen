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
#include "home_screen_conf.h"

#include <ico_uws.h>

#include <bundle.h>

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/
#define ICO_ONS_WS_TIMEOUT 0.1
#define ICO_ONS_WS_ADDRESS "127.0.0.1"
#define ICO_ONS_WS_PROTOCOL_NAME ICO_HS_PROTOCOL_OS
#define ICO_ONS_BUF_SIZE    (1024)
#define ICO_ONS_APPLI_NUM    (15)        /* only for applist */

#define ICO_ONS_VERTICAL    (1)
#define ICO_ONS_HORIZONTAL  (2)

#define ICO_ONS_CMD_WAIT    (1)
#define ICO_ONS_NO_WAIT     (2)

/*============================================================================*/
/* static(internal) functions prototype                                       */
/*============================================================================*/
static void ons_callback_uws(const struct ico_uws_context *context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data);
static int ons_loadinons_edje_file(const char *edje_file);
static void ons_event_message(char *format, ...);
static char *ons_edje_parse_str(void *in, int arg_num);
static void ons_create_context(void);
static Eina_Bool ons_ecore_event(void *data);
static void ons_on_destroy(Ecore_Evas *ee);
static void ons_touch_up_edje(void *data, Evas *evas,
                              Evas_Object *obj, void *event_info);
static void ons_touch_up_next(void *data, Evas *evas, Evas_Object *obj,
                              void *event_info);
static const char *ons_get_fname(const char *filepath);
static int ons_get_appindex(int idx);
static void ons_set_appicon(Evas *evas, Evas_Object *edje, Evas_Object* part,
                            const char *partname);
static void ons_load_config(void);
static void ons_config_event(const char *appid, int type);

/*============================================================================*/
/* variabe & table                                                            */
/*============================================================================*/
static int ons_ws_port = ICO_HS_WS_PORT;
static int ons_ws_connected = 0;
static struct ico_uws_context *ons_uws_context = NULL;
static void *ons_uws_id = NULL;
static char ons_edje_str[ICO_ONS_BUF_SIZE];

static Ecore_Evas *ons_window; /* ecore-evas object */
static Evas *ons_evas = NULL; /* evas object */
static Evas_Object *ons_edje = NULL; /* loaded edje objects */
static Eina_List *ons_img_list = NULL;
static int ons_width, ons_height;
static int ons_applist_idx = 0; /* only for applist, it's index */
static int ons_app_cnt = 0; /* only for applist. a number of app to listed */

static int ons_wait_reply = ICO_ONS_NO_WAIT;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_event_message
 *          send message
 *
 * @param[in]   wsi                 libwebsockets management table to send
 * @param[in]   fromat              message to send
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_event_message(char *format, ...)
{
    va_list list;
    unsigned char message[ICO_HS_TEMP_BUF_SIZE];

    va_start(list, format);
    vsnprintf((char *)message, sizeof(message), format, list);
    va_end(list);

    uifw_trace("OnScreen: ons_event_message %s", message);

    ico_uws_send(ons_uws_context, ons_uws_id, message, strlen((char *)message));

    return;
}

static char *
ons_edje_parse_str(void *in, int arg_num)
{
    int i;
    char *data;

    uifw_trace("ons_edje_parse_str %s, arg = %d", in, arg_num);
    data = strtok(in, " ");
    /* arg_num : 0 to n */
    for (i = 0; i < arg_num; i++) {
        data = strtok(NULL, " ");
    }
    uifw_trace("ons_edje_parse_str data: %s", data);
    return data;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   ons_callback_uws
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
ons_callback_uws(const struct ico_uws_context *context,
                const ico_uws_evt_e event, const void *id,
                const ico_uws_detail *detail, void *data)
{
    uifw_trace("ons_callback_uws %p", context);
    char *in;

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_OPEN(id=%d)", (int)id);
        ons_uws_id = (void *)id;
        ons_event_message("%s ANS HELLO", ICO_HS_MSG_HEAD_OS);
        break;

    case ICO_UWS_EVT_CLOSE:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_CLOSE(id=%d)", (int)id);
        ons_uws_context = NULL;
        ons_ws_connected = 0;
        ons_uws_id = NULL;
        break;

    case ICO_UWS_EVT_RECEIVE:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_RECEIVE(id=%d, msg=%s, len=%d)",
                   (int)id, (char *)detail->_ico_uws_message.recv_data,
                   detail->_ico_uws_message.recv_len);
        in = (char *)detail->_ico_uws_message.recv_data;

        if (strlen(in) == 0)
            break;
        ons_wait_reply = ICO_ONS_NO_WAIT;
        /* onscreen activate request */
        if (strncmp("OPEN", in, 4) == 0) {
            uifw_trace("%s", in);
            strncpy(ons_edje_str, ons_edje_parse_str(in, 1), sizeof(ons_edje_str));
            uifw_trace("ons_loadinons_edje_file: %s", &ons_edje_str[0]);
            if (ons_loadinons_edje_file(&ons_edje_str[0]) == 0) {
                ons_event_message("%s RESULT SUCCESS", ICO_HS_MSG_HEAD_OS);
            }
            else {
                ons_event_message("%s RESULT FAILED", ICO_HS_MSG_HEAD_OS);
            }
        }
        break;

    case ICO_UWS_EVT_ERROR:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_ERROR(id=%d, err=%d)",
                   (int)id, detail->_ico_uws_error.code);
        break;

    case ICO_UWS_EVT_ADD_FD:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_ADD_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;

    case ICO_UWS_EVT_DEL_FD:
        uifw_trace("ons_callback_uws: ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                   (int)id, detail->_ico_uws_fd.fd);
        break;

    default:
        break;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_create_context
 *          connect to the homescreen using websocket.
 *
 * @param[in]   none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_create_context(void)
{
    int ret;
    char uri[ICO_HS_TEMP_BUF_SIZE];

    /* set up URI "ws://HOST:PORT" */
    sprintf(uri, "ws://%s:%d", ICO_HS_WS_HOST, ons_ws_port);

    /* create context */
    ons_uws_context = ico_uws_create_context(uri, ICO_HS_PROTOCOL);
    uifw_trace("ons_create_context: ctx = %p", ons_uws_context);

    ons_ws_connected = 0;
    if (ons_uws_context == NULL) {
        uifw_trace("ons_create_context: libwebsocket_create_context failed.");
    }
    else {
        /* set callback */
        ret = ico_uws_set_event_cb(ons_uws_context, ons_callback_uws, NULL);
        if (ret != ICO_UWS_ERR_NONE) {
            uifw_trace("ons_create_context: cannnot set callback");
        }
        ons_ws_connected = 1;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_ecore_event
 *          timer handler called by Ecore.
 *
 * @param[in]   data                user data
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
ons_ecore_event(void *data)
{
    if (ons_ws_connected) {
        ico_uws_service(ons_uws_context);
    }
    else {
        if (ons_uws_context != NULL) {
            ico_uws_close(ons_uws_context);
            ons_uws_context = NULL;
        }
        ons_create_context();
    }

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   ons_on_destroy
 *          callback function called by EFL when ecore destroyed.
 *          exit ecore main loop.
 *
 * @param[in]   ee                  ecore evas object
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_on_destroy(Ecore_Evas *ee)
{
    uifw_trace("ons_on_destroy: Enter");

    ecore_main_loop_quit();
    ico_uws_close(ons_uws_context);
    edje_shutdown();
}

static void
ons_touch_up_edje(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    if (ons_wait_reply == ICO_ONS_CMD_WAIT) return;
    ons_wait_reply = ICO_ONS_CMD_WAIT;

    /* get name from userdata */
    if (data != NULL) {
        uifw_trace("OnScreen: user data is %s", (const char *)data);
    }
    else {
        uifw_trace("OnScreen: user data is NULL");
    }
    ons_event_message("%s TOUCH %s %s", ICO_HS_MSG_HEAD_OS, ons_edje_str, data);

    /* operation sound */
    hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_touch_up_next
 *          processing when next button touch up.
 *
 * @param[in]   data                user data
 * @param[in]   obj                 evas object of the button
 * @param[in]   event_info          evas event infomation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_touch_up_next(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    int listcnt;

    if (ons_wait_reply == ICO_ONS_CMD_WAIT) return;
    ons_wait_reply = ICO_ONS_CMD_WAIT;

    if (ons_app_cnt > 0) {
        listcnt = ((ons_app_cnt - 1) / ICO_ONS_APPLI_NUM) + 1;
    }
    else {
        listcnt = 1;
    }
    ons_applist_idx += 1;
    if (ons_applist_idx >= listcnt) {
        ons_applist_idx = 0;
    }

    /* get name from userdata */
    if (data != NULL) {
        uifw_trace("OnScreen: user data is %s", (const char *)data);
    }
    else {
        uifw_trace("OnScreen: user data is NULL");
    }
    ons_event_message("%s TOUCH %s %s", ICO_HS_MSG_HEAD_OS, ons_edje_str, data);

    /* operation sound */
    hs_snd_play(hs_snd_get_filename(ICO_HS_SND_TYPE_DEFAULT));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_get_fname
 *          get filename from the full path
 *
 * @param[in]   filepath            file path
 * @return      filename
 */
/*--------------------------------------------------------------------------*/
static const char *
ons_get_fname(const char *filepath)
{
    int ii;
    const char *name = filepath;

    for (ii = 0; ii < ICO_ONS_BUF_SIZE - 1; ii++) {
        if (filepath[ii] == 0)
            break;
        if (filepath[ii] == '/')
            name = &filepath[ii + 1];
    }

    return name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_get_appindex
 *          return a application index that to be set indicated index.
 *
 * @param[in]   filepath            file path
 * @return      filename
 */
/*--------------------------------------------------------------------------*/
static int
ons_get_appindex(int idx)
{
    int ii;
    int appidx = idx + ons_applist_idx * ICO_ONS_APPLI_NUM + 1;
    int cnt = 0;
    Ico_Uxf_App_Config *appconf = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();

    uifw_trace("ons_get_appindex: idx=%d appidx=%d appcnt=%d", idx, appidx, ons_app_cnt);

    for (ii = 0; ii < appconf->applicationNum; ii++) {
        if ((! appconf->application[ii].noicon) &&
            (strcmp(appconf->application[ii].type, ICO_HS_GROUP_SPECIAL) != 0)) {
            cnt++;
        }
        if (cnt == appidx) {
            return ii;
        }
    }

    return -1;
}

/* set App Icon on rect */
static void
ons_set_appicon(Evas *evas, Evas_Object *edje, Evas_Object* part,
                const char *partname)
{
    int x, y, w, h;
    int idx; /* rect index */
    int appidx; /* appli index */
    char imgfile[ICO_ONS_BUF_SIZE];
    Ico_Uxf_App_Config *appconf;
    Evas_Object *img = NULL;

    memset(imgfile, 0, sizeof(imgfile));
    /* check name, if part is rect/next_bt/cancel_bt */
    if (strncmp(partname, ICO_HS_ONS_PART_RECT_NAME,
                sizeof(ICO_HS_ONS_PART_RECT_NAME) - 1) == 0) {
        img = evas_object_image_filled_add(ons_evas);
        /* get rect index from partname(rect_01, rect_02, ...) */
        sscanf(partname, ICO_HS_ONS_PART_RECT_NAME"%d", &idx);
        idx -= 1;

        appconf = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();

        appidx = ons_get_appindex(idx);
        uifw_trace("ons_set_appicon: idx=%d appidx=%d", idx, appidx);
        if ((appidx < 0) || (appidx > appconf->applicationNum)) {
            evas_object_del(img);
            return;
        }

        /* set icon file name */
        snprintf(imgfile, ICO_ONS_BUF_SIZE, "%s",
                 appconf->application[appidx].icon_key_name);
        uifw_trace("ons_set_appicon: set image = %s(%d/%d), app=%s, group=%s",
                   imgfile, appidx, ons_app_cnt,
                   appconf->application[appidx].appid,
                   appconf->application[appidx].group);
        /* set mouse call back function */
        evas_object_event_callback_add(img, EVAS_CALLBACK_MOUSE_UP,
                                       ons_touch_up_edje,
                                       appconf->application[appidx].appid);
    }
    else if (strcmp(partname, ICO_HS_ONS_PART_NEXT_NAME) == 0) {
        evas_object_event_callback_add(part, EVAS_CALLBACK_MOUSE_UP,
                                       ons_touch_up_next, partname);
    }
    else if (strcmp(partname, ICO_HS_ONS_PART_CANCEL_NAME) == 0) {
        evas_object_event_callback_add(part, EVAS_CALLBACK_MOUSE_UP,
                                       ons_touch_up_edje, partname);
    }
    else {
        return;
    }

    if (strlen(imgfile) > 0) {
        /* calculation icon pos */
        edje_object_part_geometry_get(ons_edje, partname, &x, &y, &w, &h);
        x += (ons_width - ICO_HS_SIZE_LAYOUT_WIDTH) / 2;
        y += (ons_height - ICO_HS_SIZE_LAYOUT_HEIGHT) / 2;

        evas_object_image_file_set(img, imgfile, NULL);
        evas_object_move(img, x, y);
        evas_object_resize(img, w, h);
        evas_object_show(img);
    }

    ons_img_list = eina_list_append(ons_img_list, img);
}

static int
ons_loadinons_edje_file(const char *edje_file)
{
    Evas_Object *part; /* part handle */
    Eina_List *group; /* edje group list */
    Eina_List *list; /* part list in edje */
    int group_count = 0; /* group counter */
    int name_count = 0; /* name counter */
    int moveX, moveY; /* move list to center */
    Eina_List *l, *l_next;
    Evas_Object *data;
    Evas_Object *canvas;

    if (!ons_evas) {
        ons_evas = ecore_evas_get(ons_window);
        if (!ons_evas) {
            uifw_trace("OnScreen: could not create evas.");
            return -1;
        }
        /* set color */
        canvas = evas_object_rectangle_add(ons_evas);
        evas_object_color_set(canvas, 0, 0, 0, 191);
        evas_object_move(canvas, 0, 0);
        evas_object_resize(canvas, ons_width, ons_height);
        evas_object_show(canvas);
    }

    /* delete pre image */
    uifw_trace("OnScreen: img list is %08x", ons_img_list);
    EINA_LIST_FOREACH_SAFE(ons_img_list, l, l_next, data) {
        uifw_trace("OnScreen: del data is %08x", data);
        evas_object_del(data);
        ons_img_list = eina_list_remove_list(ons_img_list, l);
    }

    /* delete edje */
    if (ons_edje) {
        evas_object_del(ons_edje);
    }

    /* create and add object in canvas from edje */
    ons_edje = edje_object_add(ons_evas);
    if (!ons_edje) {
        uifw_trace("OnScreen: could not create edje object!");
        return -1;
    }

    uifw_trace("OnScreen: w=%d h=%d", ons_width, ons_height);

    /* calc move */
    moveX = (ons_width - ICO_HS_SIZE_LAYOUT_WIDTH) / 2;
    moveY = (ons_height - ICO_HS_SIZE_LAYOUT_HEIGHT) / 2;

    /* Put in the image */
    evas_object_move(ons_edje, moveX, moveY);
    /* Resize the image */
    evas_object_resize(ons_edje, ons_width, ons_height);
    /* Show the image */
    evas_object_show(ons_edje);

    /* get group list */
    group = edje_file_collection_list(edje_file);
    while (group != NULL) {
        /* Set the edj file */
        if (!edje_object_file_set(ons_edje, edje_file, (const char *)group->data)) {
            int err = edje_object_load_error_get(ons_edje);
            const char *errmsg = edje_load_error_str(err);
            uifw_trace("OnScreen: could not load %s: %s", edje_file, errmsg);

            edje_file_collection_list_free(group);
            evas_object_del(ons_edje);
            return -1;
        }
        uifw_trace("OnScreen: group[%d] data : %s", group_count,
                   (const char *)group->data);

        /* get list */
        list = edje_object_access_part_list_get(ons_edje);
        while (list != NULL) {
            uifw_trace("OnScreen: list[%d] data : %s", name_count,
                       (const char *)list->data);

            /* set callback for part name */
            part = (Evas_Object *)edje_object_part_object_get(ons_edje,
                                                              (const char *)list->data);
            if (part != NULL) {
                uifw_trace("OnScreen: list[%d] name : %s", name_count,
                           (const char *)list->data);

                /* if not applist */
                if (strncmp(ons_get_fname(edje_file), ICO_HS_ONS_APPLI_LIST_NAME,
                            sizeof(ICO_HS_ONS_APPLI_LIST_NAME) - 1) != 0) {
                    evas_object_event_callback_add(part,
                                                   EVAS_CALLBACK_MOUSE_UP,
                                                   ons_touch_up_edje, list->data);
                }
                /* if applist */
                else {
                    ons_set_appicon(ons_evas, ons_edje, part, (const char *)list->data);
                }
            }
            else {
                uifw_trace("OnScreen: list[%d] is NULL", name_count);
            }
            /* to next list */
            list = list->next;
            name_count++;
        }
        /* to next group */
        group = group->next;
        group_count++;
    }
    uifw_trace("OnScreen: group num is %d", group_count);
    uifw_trace("OnScreen: name num is %d", name_count);

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_load_config
 *          load/reload configuration.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_load_config(void)
{
    Ico_Uxf_App_Config *appconf;
    int appcnt;
    int appidx, idx, cnt;

    uifw_trace("ons_load_config: Enter");

    appconf = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();
    appcnt = appconf->applicationNum;
    for (appidx = 0; appidx < appconf->applicationNum; appidx++) {
        if ((appconf->application[appidx].noicon) ||
            (strcmp(appconf->application[appidx].type, ICO_HS_GROUP_SPECIAL) == 0)) {
            appcnt--;
            uifw_trace("ons_load_config: No Need appid=%s noicon=%d type=%s",
                       appconf->application[appidx].appid,
                       appconf->application[appidx].noicon,
                       appconf->application[appidx].type);
        }
    }
    cnt = 0;
    for (idx = 0; idx < appcnt; idx++) {
        appidx = ons_get_appindex(idx);
        if (appidx > 0) {
            uifw_trace("ons_load_config: appid=%s seat=%d idx=%d seatcnt=%d",
                       appconf->application[appidx].appid, cnt
                               / ICO_ONS_APPLI_NUM, idx - ICO_ONS_APPLI_NUM
                               * (cnt / ICO_ONS_APPLI_NUM), ((appcnt - 1)
                               / ICO_ONS_APPLI_NUM) + 1);
            cnt++;
        }
    }

    ons_app_cnt = appcnt;
    ons_applist_idx = 0;

    uifw_trace("ons_load_config: Leave(appcnt=%d)", appcnt);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ons_config_event
 *          This is a callback function called when the configurations
 *          is updata.
 *
 * @param[in]   appid               application id
 * @param[in]   type                event type(install/uninstall)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ons_config_event(const char *appid, int type)
{
    uifw_trace("ons_config_event: Enter(appid=%s, type=%d)", appid, type);

    ons_load_config();

    uifw_trace("ons_config_event: Leave");

    return;
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   onscreen application
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
    int width, height;
    int orientation = ICO_ONS_HORIZONTAL;
    int ret;
    bundle *b;
    const char *val;

    /* configure */
    if (getenv("PKG_NAME")) {
        ico_uxf_log_open(getenv("PKG_NAME"));
    }
    else {
        ico_uxf_log_open(ICO_HS_APPID_DEFAULT_ONS);
    }

    /* get argment */
    b = bundle_import_from_argv(argc, argv);
    val = bundle_get_val(b, "orientation");
    if (val != NULL) {
        if (strcmp(val, "vertical") == 0) {
            orientation = ICO_ONS_VERTICAL;
        }
        else if (strcmp(val, "horizontal") == 0) {
            orientation = ICO_ONS_HORIZONTAL;
        }
    }

    /* load configuration */
    ret = initHomeScreenConfig(ICO_ONSCREEN_CONFIG_FILE);
    if (ret == ICO_HS_OK) {
        ons_ws_port = hs_conf_get_integer(ICO_HS_CONFIG_ONSCREEN,
                                         ICO_HS_CONFIG_WS_PORT,
                                         ICO_HS_WS_PORT);
        orientation = hs_conf_get_integer(ICO_HS_CONFIG_ONSCREEN,
                                          ICO_HS_CONFIG_ORIENTAION,
                                          orientation);
    }
    ons_load_config();
    hs_snd_init();

    /* Reset a ecore_evas */
    ecore_evas_init();

    /* Initialize a edje */
    edje_init();

    /* Make a new ecore_evas */
    ons_window = ecore_evas_new(NULL, 0, 0, 1, 1, "frame=0");

    /* if not for a window, return NULL */
    if (!ons_window) {
        EINA_LOG_CRIT("OnScreen: could not create ons_window.");
        return -1;
    }
    ecore_evas_callback_destroy_set(ons_window, ons_on_destroy);

    /* resize window */
    ecore_main_loop_iterate();
    ecore_wl_screen_size_get(&width, &height);
    if (orientation == ICO_ONS_VERTICAL) {
        ons_width = width > height ? height : width;
        ons_height = width > height ? width : height;
    }
    else {
        ons_width = width < height ? height : width;
        ons_height = width < height ? width : height;
    }
    ecore_evas_resize(ons_window, ons_width, ons_height);

    /* Show the window */
    /* evas_output_framespace_set(ecore_evas_get(ons_window), 0, 0, 0, 0);  */
    ecore_evas_alpha_set(ons_window, EINA_TRUE);
    ecore_evas_show(ons_window);

    /* Init websockets */
    ons_create_context();
    ecore_timer_add(ICO_ONS_WS_TIMEOUT, ons_ecore_event, NULL);

    /* add callback to app configuration */
    ico_uxf_econf_setAppUpdateCb(ons_config_event);

    /* Start main loop */
    ecore_main_loop_begin();

    /* end the ecore_evas */
    ecore_evas_shutdown();

    return 0;
}
