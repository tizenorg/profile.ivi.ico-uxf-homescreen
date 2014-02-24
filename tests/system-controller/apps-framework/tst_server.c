/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   test server
 *
 * @date    Aug-19-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <glib.h>
#include <ico_uws.h>

#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"

#include "tst_common.h"

/* ----------------------------------------------- */
/* Define of static variable                       */
/* ----------------------------------------------- */
#define ZONE_EXIST      0
#define ZONE_NO_EXIST   1

static int num_close    = 0;

/* ----------------------------------------------- */
/* Define of static function                       */
/* ----------------------------------------------- */
static int _get_command(const void *data, size_t len);
static int _check_zone(const void *data, size_t len);
static msg_t _create_winctl_msg(int cmd);
static msg_t _create_winctl_attr_msg(int cmd, const char *type);
static msg_t _create_winctl_move_msg(const void *data, size_t len);
static msg_t _create_winctl_layer_msg(int cmd);
static msg_t _create_userctl_msg(int cmd);
static msg_t _create_resctl_msg(int cmd);
static msg_t _create_resctl_region_msg(int cmd);
static msg_t _create_msg(const void *data, size_t len);
static void tst_uws_callback(const struct ico_uws_context *context,
                             const ico_uws_evt_e event,
                             const void *id,
                             const ico_uws_detail *detail,
                             void *user_data);
static int test_server(void);

/* ----------------------------------------------- */
/* Static function                                 */
/* ----------------------------------------------- */
static int
_get_command(const void *data, size_t len)
{
    /* get command from receive data */
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *r_root    = NULL;
    JsonObject *r_obj   = NULL;
    int cmd             = -1;

    /* get command */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        return -1;
    }
    r_root = json_parser_get_root(parser);
    if (r_root == NULL) {
        g_object_unref(parser);
        return -1;
    }
    r_obj = json_node_get_object(r_root);
    cmd = json_object_get_int_member(r_obj, MSG_PRMKEY_CMD);
    g_object_unref(parser); 

    return cmd;
}

static int
_check_zone(const void *data, size_t len)
{
    /* get command from receive data */
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *r_root    = NULL;
    JsonObject *r_obj   = NULL;
    JsonObject *arg_obj = NULL;
    int ret             = -1;

    /* get command */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        return -1;
    }
    r_root = json_parser_get_root(parser);
    if (r_root == NULL) {
        g_object_unref(parser);
        return -1;
    }

    r_obj = json_node_get_object(r_root);
    arg_obj = json_object_get_object_member(r_obj, MSG_PRMKEY_ARG);
    if (json_object_has_member(arg_obj, MSG_PRMKEY_ZONE) == TRUE) {
        ret = ZONE_EXIST;
    }
    else {
        ret = ZONE_NO_EXIST;
    }
    g_object_unref(parser); 

    return ret;
}

static msg_t
_create_winctl_msg(int cmd)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);
    json_object_set_string_member(argobj, MSG_PRMKEY_WINNAME, TST_WIN_NAME);
    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, TST_SURFACE);
    json_object_set_int_member(argobj, MSG_PRMKEY_NODE, TST_NODE);
    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, TST_LAYER);
    json_object_set_int_member(argobj, MSG_PRMKEY_POS_X, TST_POS_X);
    json_object_set_int_member(argobj, MSG_PRMKEY_POS_Y, TST_POS_Y);
    json_object_set_int_member(argobj, MSG_PRMKEY_WIDTH, TST_WIDTH);
    json_object_set_int_member(argobj, MSG_PRMKEY_HEIGHT, TST_HEIGHT);
    json_object_set_int_member(argobj, MSG_PRMKEY_RAISE, TST_RAISE);
    json_object_set_int_member(argobj, MSG_PRMKEY_VISIBLE, TST_VISIBLE);
    json_object_set_int_member(argobj, MSG_PRMKEY_ACTIVE, TST_ACTIVE);
    json_object_set_int_member(argobj, MSG_PRMKEY_STRIDE, TST_STRIDE);
    json_object_set_int_member(argobj, MSG_PRMKEY_FORMAT, TST_FORMAT);

    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_winctl_attr_msg(int cmd, const char *type)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);
    json_object_set_string_member(argobj, MSG_PRMKEY_WINNAME, type);
    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, TST_SURFACE);
    json_object_set_int_member(argobj, MSG_PRMKEY_NODE, TST_NODE);
    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, TST_LAYER);
    json_object_set_int_member(argobj, MSG_PRMKEY_POS_X, TST_POS_X);
    json_object_set_int_member(argobj, MSG_PRMKEY_POS_Y, TST_POS_Y);
    json_object_set_int_member(argobj, MSG_PRMKEY_WIDTH, TST_WIDTH);
    json_object_set_int_member(argobj, MSG_PRMKEY_HEIGHT, TST_HEIGHT);
    json_object_set_int_member(argobj, MSG_PRMKEY_RAISE, TST_RAISE);
    json_object_set_int_member(argobj, MSG_PRMKEY_VISIBLE, TST_VISIBLE);
    json_object_set_int_member(argobj, MSG_PRMKEY_ACTIVE, TST_ACTIVE);
    json_object_set_int_member(argobj, MSG_PRMKEY_STRIDE, TST_STRIDE);
    json_object_set_int_member(argobj, MSG_PRMKEY_FORMAT, TST_FORMAT);

    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_winctl_move_msg(const void *data, size_t len)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_CHANGE_ATTR);
    json_object_set_string_member(argobj, MSG_PRMKEY_WINNAME, "move");
    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, TST_SURFACE);
    json_object_set_int_member(argobj, MSG_PRMKEY_NODE, TST_NODE);
    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, TST_LAYER);
    if (_check_zone(data, len) == ZONE_EXIST) {
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_X, 0);
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_Y, 0);
    }
    else {
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_X, 10);
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_Y, 10);
    }
    json_object_set_int_member(argobj, MSG_PRMKEY_WIDTH, TST_WIDTH);
    json_object_set_int_member(argobj, MSG_PRMKEY_HEIGHT, TST_HEIGHT);
    json_object_set_int_member(argobj, MSG_PRMKEY_RAISE, TST_RAISE);
    json_object_set_int_member(argobj, MSG_PRMKEY_VISIBLE, TST_VISIBLE);
    json_object_set_int_member(argobj, MSG_PRMKEY_ACTIVE, TST_ACTIVE);
    json_object_set_int_member(argobj, MSG_PRMKEY_STRIDE, TST_STRIDE);
    json_object_set_int_member(argobj, MSG_PRMKEY_FORMAT, TST_FORMAT);

    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_winctl_layer_msg(int cmd)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_CHANGE_LAYER_ATTR);
    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, TST_LAYER);
    if (cmd == MSG_CMD_SHOW_LAYER) {
        json_object_set_int_member(argobj, MSG_PRMKEY_VISIBLE, TST_VISIBLE);
    }
    else {
        json_object_set_int_member(argobj, MSG_PRMKEY_VISIBLE, TST_INVISIBLE);
    }

    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}


static msg_t
_create_userctl_msg(int cmd)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;
    JsonArray *array    = NULL;

    /* create json object and array */
    obj = json_object_new();
    argobj = json_object_new();
    array = json_array_new();
    if (obj == NULL || argobj == NULL || array == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);
    json_object_set_int_member(argobj, MSG_PRMKEY_USER_NUM, 3);

    json_array_add_string_element(array, TST_USER_A);
    json_array_add_string_element(array, TST_USER_B);
    json_array_add_string_element(array, TST_USER_C);
    json_object_set_array_member(argobj, MSG_PRMKEY_USER_LIST, array);

    json_object_set_string_member(argobj, MSG_PRMKEY_USER_LOGIN, TST_USER_A);
    json_object_set_string_member(argobj, MSG_PRMKEY_LASTINFO, TST_LASTINFO);

    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_resctl_msg(int cmd)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonObject *window_obj  = NULL;
    JsonObject *sound_obj  = NULL;
    JsonObject *input_obj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    resobj = json_object_new();
    window_obj = json_object_new();
    sound_obj = json_object_new();
    input_obj = json_object_new();
    if (obj == NULL || resobj == NULL || window_obj == NULL
        || sound_obj == NULL || input_obj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);

    /* window */
    json_object_set_string_member(window_obj, MSG_PRMKEY_RES_ZONE, TST_ZONE_A);
    json_object_set_string_member(window_obj, MSG_PRMKEY_RES_NAME, TST_NAME_A);
    json_object_set_string_member(window_obj, MSG_PRMKEY_RES_ID, TST_ID_A);
    /* sound */
    json_object_set_string_member(sound_obj, MSG_PRMKEY_RES_ZONE, TST_ZONE_B);
    json_object_set_string_member(sound_obj, MSG_PRMKEY_RES_NAME, TST_NAME_B);
    json_object_set_string_member(sound_obj, MSG_PRMKEY_RES_ID, TST_ID_B);
    json_object_set_int_member(sound_obj, MSG_PRMKEY_RES_ADJUST, TST_ADJUST);
    /* input */
    json_object_set_string_member(input_obj, MSG_PRMKEY_RES_NAME, TST_NAME_C);
    json_object_set_int_member(input_obj, MSG_PRMKEY_RES_EV, TST_INPUT_EV);
    /* type */
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_TYPE, TST_RES_TYPE);

    /* set object */
    json_object_set_object_member(resobj, MSG_PRMKEY_RES_WINDOW, window_obj);
    json_object_set_object_member(resobj, MSG_PRMKEY_RES_SOUND, sound_obj);
    json_object_set_object_member(resobj, MSG_PRMKEY_RES_INPUT, input_obj);
    json_object_set_object_member(obj, MSG_PRMKEY_RES, resobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_resctl_region_msg(int cmd)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object and array */
    obj = json_object_new();
    resobj = json_object_new();
    if (obj == NULL || resobj == NULL) {
        return NULL;
    }

    /* set message */
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, TST_APPID);
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);

    /* region */
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_SURFACE, TST_SURFACE);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_X, TST_POS_X);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_Y, TST_POS_Y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_WIDTH, TST_REG_WIDTH);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HEIGHT, TST_REG_HEIGHT);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HOT_X, TST_REG_HOT_X);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HOT_Y, TST_REG_HOT_Y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_X, TST_REG_CUR_X);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_Y, TST_REG_CUR_Y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_WIDTH, TST_REG_CUR_WIDTH);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_HEIGHT, TST_REG_CUR_HEIGHT);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_ATTR, TST_REG_ATTR);

    /* set object */
    json_object_set_object_member(obj, MSG_PRMKEY_REGION, resobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

static msg_t
_create_msg(const void *data, size_t len)
{
    int cmd             = -1;
    JsonGenerator *gen  = NULL;

    /* get command */
    cmd = _get_command(data, len);
    if (cmd < 0) {
        return NULL;
    }

    switch (cmd) {
    case MSG_CMD_CREATE:
    case MSG_CMD_DESTROY:
    case MSG_CMD_CHANGE_ACTIVE:
        gen = _create_winctl_msg(cmd);
        break;
    case MSG_CMD_SHOW:
        gen = _create_winctl_attr_msg(MSG_CMD_CHANGE_ATTR, "show");
        break;
    case MSG_CMD_HIDE:
        gen = _create_winctl_attr_msg(MSG_CMD_CHANGE_ATTR, "hide");
        break;
    case MSG_CMD_MOVE:
        gen = _create_winctl_move_msg(data, len);
        break;
    case MSG_CMD_CHANGE_LAYER:
        gen = _create_winctl_attr_msg(MSG_CMD_CHANGE_ATTR, "change layer");
        break;
    case MSG_CMD_MAP_THUMB:
    case MSG_CMD_UNMAP_THUMB:
        gen = _create_winctl_msg(cmd);
        break;
    case MSG_CMD_SHOW_LAYER:
    case MSG_CMD_HIDE_LAYER:
        gen = _create_winctl_layer_msg(cmd);
        break;
    case MSG_CMD_ADD_INPUT:
    case MSG_CMD_DEL_INPUT:
#if 0           /* SystemController 0.9.21(with GENIVI ivi-shell) not support send_input */
    case MSG_CMD_SEND_INPUT:
#endif
        gen = NULL;
        break;
    case MSG_CMD_CHANGE_USER:
    case MSG_CMD_GET_USERLIST:
        gen = _create_userctl_msg(cmd);
        break;
    case MSG_CMD_ACQUIRE_RES:
    case MSG_CMD_RELEASE_RES:
    case MSG_CMD_DEPRIVE_RES:
    case MSG_CMD_WAITING_RES:
    case MSG_CMD_REVERT_RES:
        gen = _create_resctl_msg(cmd);
        break;
    case MSG_CMD_SET_REGION:
    case MSG_CMD_UNSET_REGION:
        gen = _create_resctl_region_msg(cmd);
        break;
    default:
        gen = NULL;
        break;
    }

    return gen;
}

/* event callback */
static void
tst_uws_callback(const struct ico_uws_context *context,
                 const ico_uws_evt_e event,
                 const void *id,
                 const ico_uws_detail *detail,
                 void *user_data)
{
    JsonNode *node  = NULL;
    msg_t msg       = NULL;
    msg_str_t data  = NULL;
    size_t  len     = 0;

    switch (event) {
    case ICO_UWS_EVT_RECEIVE:
        msg = _create_msg((const void *)detail->_ico_uws_message.recv_data,
                          detail->_ico_uws_message.recv_len);
        if (msg == NULL) break;
        data = json_generator_to_data(msg, &len);
        if (data == NULL) break;

        /* send return message */
        ico_uws_send((struct ico_uws_context *)context,
                     (void *)id,
                     (unsigned char *)data, len);
        printf("send: %s\n", (char *)data);
        /* free */
        g_free(data);
        node = json_generator_get_root(msg);
        json_object_unref(json_node_get_object(node));
        json_node_free(node);
        usleep(50);
        break;
    case ICO_UWS_EVT_CLOSE:
        num_close++;
        break;
    case ICO_UWS_EVT_OPEN:
    case ICO_UWS_EVT_ERROR:
    case ICO_UWS_EVT_ADD_FD:
    case ICO_UWS_EVT_DEL_FD:
    default:
        /* other event is not test */
        break;
    }

    return;
}

/* test main (server) */
static int
test_server(void)
{
    struct ico_uws_context *context;

    /* create context */
    context = ico_uws_create_context(SRV_URI, ICO_SYC_PROTOCOL);

    if (context) {
        (void)ico_uws_set_event_cb(context, tst_uws_callback, NULL);

        /* service (loop) */
        while (num_close < 5) {
            ico_uws_service(context);
            usleep(50);
        }

        /* close */
        ico_uws_close(context);
    }

    return 1;
}

/* ----------------------------------------------- */
/* Main                                            */
/* ----------------------------------------------- */
static GMainLoop *g_mainloop = NULL;

static gboolean
exit_program(gpointer data)
{
    g_main_loop_quit(g_mainloop);

    return FALSE;
}

/* main */
int
main(int argc, char **argv)
{
    g_setenv("PKG_NAME", "org.test.ico.tst_server", 1);
    g_mainloop = g_main_loop_new(NULL, 0);

    test_server();

    g_timeout_add_seconds(2, exit_program, NULL);
    g_main_loop_run(g_mainloop);

    return 0;
}
/* vim: set expandtab ts=4 sw=4: */
