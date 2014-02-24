/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Window Control API
 *          for privilege applications
 *
 * @date    Feb-21-2014
 */

#include <string.h>

#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"
#include "ico_syc_privilege.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define _CMD_SHOW_WIN           0
#define _CMD_HIDE_WIN           1
#define _CMD_SHOW_LAYER         10
#define _CMD_HIDE_LAYER         11
#define _CMD_SET_LAYER_ATTR     12

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static msg_t _create_win_msg(const char *appid, int surface,
                             const ico_syc_animation_t *animation,
                             int type);
static msg_t _create_win_move_msg(const char *appid, int surface,
                                  const ico_syc_win_move_t *move,
                                  const ico_syc_animation_t *animation);
static msg_t _create_active_win_msg(const char *appid, int surface);
static msg_t _create_change_layer_msg(const char *appid, int surface, int layer);
static msg_t _create_map_get_msg(const char *appid, int surface, const char *filepath);
static msg_t _create_map_thumb_msg(const char *appid, int surface, int framerate,
                                   const char *filepath);
static msg_t _create_unmap_thumb_msg(const char *appid, int surface);
static msg_t _create_layer_msg(const char *appid, int layer, int type);

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_win_msg
 *          Create the message to show/hide the application window.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   animation               animation information
 * @param[in]   type                    type of command
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_win_msg(const char *appid, int surface,
                const ico_syc_animation_t *animation, int type)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;
    int cmd             = -1;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set command */
    if (type == _CMD_SHOW_WIN) {
        cmd = MSG_CMD_SHOW;
    }
    else if (type == _CMD_HIDE_WIN) {
        cmd = MSG_CMD_HIDE;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    /* set animation info */
    if (animation != NULL) {
        json_object_set_string_member(argobj, MSG_PRMKEY_ANIM_NAME,
                                      animation->name);
        json_object_set_int_member(argobj, MSG_PRMKEY_ANIM_TIME,
                                   animation->time);
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_win_move_msg
 *          Create the message to move the application window.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   move                    move information (zone/position/size)
 * @param[in]   animation               animation information
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_win_move_msg(const char *appid, int surface,
                     const ico_syc_win_move_t *move,
                     const ico_syc_animation_t *animation)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_MOVE);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    /* set move info */
    if (move->zone != NULL) {
        json_object_set_string_member(argobj, MSG_PRMKEY_ZONE, move->zone);
        json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, move->layer);
    }
    else {
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_X, move->pos_x);
        json_object_set_int_member(argobj, MSG_PRMKEY_POS_Y, move->pos_y);
        json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, -1);
    }
    json_object_set_int_member(argobj, MSG_PRMKEY_WIDTH, move->width);
    json_object_set_int_member(argobj, MSG_PRMKEY_HEIGHT, move->height);
    /* set animation info */
    if (animation != NULL) {
        json_object_set_string_member(argobj, MSG_PRMKEY_ANIM_NAME,
                                      animation->name);
        json_object_set_int_member(argobj, MSG_PRMKEY_ANIM_TIME,
                                   animation->time);
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_active_win_msg
 *          Create the message to change active window.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_active_win_msg(const char *appid, int surface)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_CHANGE_ACTIVE);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_change_layer_msg
 *          Create the message to change application window's layer.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   layer                   window's layer id
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_change_layer_msg(const char *appid, int surface, int layer)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_CHANGE_LAYER);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, layer);
    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_map_get_msg
 *          Create the message to map get.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   filepath                pixel image file path
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_map_get_msg(const char *appid, int surface, const char *filepath)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_MAP_GET);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    json_object_set_string_member(argobj, MSG_PRMKEY_ANIM_NAME, filepath);
    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_map_thumb_msg
 *          Create the message to map thumbnail.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   framerate               notify cycle [frames par sec]
 * @param[in]   filepath                pixel image file path
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_map_thumb_msg(const char *appid, int surface, int framerate, const char *filepath)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_MAP_THUMB);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    json_object_set_int_member(argobj, MSG_PRMKEY_RATE, framerate);
    if ((filepath != NULL) && (*filepath != 0) && (*filepath != ' '))   {
        json_object_set_string_member(argobj, MSG_PRMKEY_ANIM_NAME, filepath);
    }
    else    {
        json_object_set_string_member(argobj, MSG_PRMKEY_ANIM_NAME, " ");
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_unmap_thumb_msg
 *          Create the message to unmap thumbnail.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_unmap_thumb_msg(const char *appid, int surface)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_UNMAP_THUMB);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_SURFACE, surface);
    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_layer_msg
 *          Create the message to show/hide layer or to set layer attribute.
 *
 * @param[in]   appid                   application id
 * @param[in]   layer                   window's layer id
 * @param[in]   type                    type of command
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_layer_msg(const char *appid, int layer, int type)
{
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;
    int cmd             = -1;

    /* create json object */
    obj = json_object_new();
    argobj = json_object_new();
    if (obj == NULL || argobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set command */
    if (type == _CMD_SHOW_LAYER) {
        cmd = MSG_CMD_SHOW_LAYER;
    }
    else if (type == _CMD_HIDE_LAYER) {
        cmd = MSG_CMD_HIDE_LAYER;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, cmd);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_LAYER, layer);
    json_object_set_object_member(obj, MSG_PRMKEY_ARG, argobj);

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*============================================================================*/
/* internal common function                                                   */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_win
 *          Execute callback function. (ICO_SYC_EV_WIN_ACTIVE
 *                                      ICO_SYC_EV_WIN_CREATE
 *                                      ICO_SYC_EV_WIN_DESTROY)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_win(ico_syc_callback_t callback, void *user_data,
               int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    ico_syc_win_info_t *win_info    = NULL;

    /* alloc memory */
    win_info = calloc(1, sizeof(ico_syc_win_info_t));
    if (win_info == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(win_info);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(win_info);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check message */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        g_object_unref(parser);
        free(win_info);
        _ERR("invalid message" );
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);

    /* set data */
    win_info->appid = strdup(ico_syc_get_str_member(obj, MSG_PRMKEY_APPID));
    win_info->name = strdup(ico_syc_get_str_member(argobj, MSG_PRMKEY_WINNAME));
    win_info->surface = ico_syc_get_int_member(argobj, MSG_PRMKEY_SURFACE);

    /* exec callback */
    callback(event, win_info, user_data);

    /* free memory */
    g_object_unref(parser);
    if (win_info->appid != NULL) free(win_info->appid);
    if (win_info->name != NULL) free(win_info->name);
    free(win_info);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_win_attr
 *          Execute callback function. (ICO_SYC_EV_WIN_ATTR_CHANGE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               :assed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_win_attr(ico_syc_callback_t callback, void *user_data,
                    int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    ico_syc_win_attr_t *win_attr    = NULL;

    /* alloc memory */
    win_attr = calloc(1, sizeof(ico_syc_win_attr_t));
    if (win_attr == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(win_attr);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(win_attr);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check message */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        g_object_unref(parser);
        free(win_attr);
        _ERR("invalid message" );
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);

    /* set data */
    win_attr->appid = strdup(ico_syc_get_str_member(obj, MSG_PRMKEY_APPID));
    win_attr->name = strdup(ico_syc_get_str_member(argobj, MSG_PRMKEY_WINNAME));
    win_attr->zone = strdup(ico_syc_get_str_member(argobj, MSG_PRMKEY_ZONE));
    win_attr->surface = ico_syc_get_int_member(argobj, MSG_PRMKEY_SURFACE);
    win_attr->nodeid = ico_syc_get_int_member(argobj, MSG_PRMKEY_NODE);
    win_attr->layer = ico_syc_get_int_member(argobj, MSG_PRMKEY_LAYER);
    win_attr->pos_x = ico_syc_get_int_member(argobj, MSG_PRMKEY_POS_X);
    win_attr->pos_y = ico_syc_get_int_member(argobj, MSG_PRMKEY_POS_Y);
    win_attr->width = ico_syc_get_int_member(argobj, MSG_PRMKEY_WIDTH);
    win_attr->height = ico_syc_get_int_member(argobj, MSG_PRMKEY_HEIGHT);
    win_attr->raise = ico_syc_get_int_member(argobj, MSG_PRMKEY_RAISE);
    win_attr->visible = ico_syc_get_int_member(argobj, MSG_PRMKEY_VISIBLE);
    win_attr->active = ico_syc_get_int_member(argobj, MSG_PRMKEY_ACTIVE);

    /* exec callback */
    callback(event, win_attr, user_data);

    /* free memory */
    g_object_unref(parser);
    if (win_attr->appid != NULL) free(win_attr->appid);
    if (win_attr->name != NULL) free(win_attr->name);
    if (win_attr->zone != NULL) free(win_attr->zone);
    free(win_attr);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_thumb
 *          Execute callback function. (ICO_SYC_EV_THUMB_ERROR
 *                                      ICO_SYC_EV_THUMB_CHANGE
 *                                      ICO_SYC_EV_THUMB_UNMAP)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_thumb(ico_syc_callback_t callback, void *user_data,
                 int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    ico_syc_thumb_info_t *thumb_info    = NULL;

    /* alloc memory */
    thumb_info = calloc(1, sizeof(ico_syc_thumb_info_t));
    if (thumb_info == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(thumb_info);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(thumb_info);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check message */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        g_object_unref(parser);
        free(thumb_info);
        _ERR("invalid message" );
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);

    /* set data */
    thumb_info->appid = strdup(ico_syc_get_str_member(obj, MSG_PRMKEY_APPID));

    thumb_info->surface = ico_syc_get_int_member(argobj,
                                                 MSG_PRMKEY_SURFACE);
    thumb_info->type = ico_syc_get_int_member(argobj, MSG_PRMKEY_ATTR);
    thumb_info->width = ico_syc_get_int_member(argobj, MSG_PRMKEY_WIDTH);
    thumb_info->height = ico_syc_get_int_member(argobj, MSG_PRMKEY_HEIGHT);
    thumb_info->stride = ico_syc_get_int_member(argobj, MSG_PRMKEY_STRIDE);
    thumb_info->format = ico_syc_get_int_member(argobj, MSG_PRMKEY_FORMAT);

    /* exec callback */
    callback(event, thumb_info, user_data);

    /* free memory */
    g_object_unref(parser);
    free(thumb_info->appid);
    free(thumb_info);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_layer
 *          Execute callback function. (ICO_SYC_EV_LAYER_ATTR_CHANGE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_layer(ico_syc_callback_t callback, void *user_data,
                 int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    ico_syc_layer_attr_t *layer_attr    = NULL;

    /* alloc memory */
    layer_attr = calloc(1, sizeof(ico_syc_layer_attr_t));
    if (layer_attr == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(layer_attr);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(layer_attr);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check message */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        g_object_unref(parser);
        free(layer_attr);
        _ERR("invalid message" );
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);
    /* set data */
    layer_attr->layer = ico_syc_get_int_member(argobj, MSG_PRMKEY_LAYER);
    layer_attr->visible = ico_syc_get_int_member(argobj,
                                                     MSG_PRMKEY_VISIBLE);

    /* exec callback */
    callback(event, layer_attr, user_data);

    /* free memory */
    g_object_unref(parser);
    free(layer_attr);

    return;
}

/*============================================================================*/
/* public interface function                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_show
 *          Show the application window with animation.
 *          If user sets argument surface "ICO_SYC_WIN_SHOW_ALL",
 *          show all of the application windows.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_show(const char *appid, int surface,
             const ico_syc_animation_t *animation)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL) {
        _ERR("invalid parameter (appid is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_win_msg(appid, surface, animation, _CMD_SHOW_WIN);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_hide
 *          Hide the application window with animation.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_hide(const char *appid, int surface,
             const ico_syc_animation_t *animation)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL) {
        _ERR("invalid parameter (appid is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_win_msg(appid, surface, animation, _CMD_HIDE_WIN);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_move
 *          Move the application window with animation.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   move                    move information (zone/position/size)
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_move(const char *appid, int surface,
             const ico_syc_win_move_t *move,
             const ico_syc_animation_t *animation)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL) {
        _ERR("invalid parameter (appid is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_win_move_msg(appid, surface, move, animation);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_active
 *          Change the active window which receives the win-event notification
 *          from System Controller.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_change_active(const char *appid, int surface)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL) {
        _ERR("invalid parameter (appid is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_active_win_msg(appid, surface);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_layer
 *          Change the window's layer.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_change_layer(const char *appid, int surface, int layer)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL) {
        _ERR("invalid parameter (appid is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_change_layer_msg(appid, surface, layer);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_map_get
 *          Get the surface thumbnail pixel image to the file.
 *
 * @param[in]   surface                 window's surface id
 * @param[in]   filepath                surface image pixel file path
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_map_get(int surface, const char *filepath)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_map_get_msg(appid, surface, filepath);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_map_thumb
 *          Prepare the thumbnail data
 *
 * @param[in]   surface                 window's surface id
 * @param[in]   framerate               notify cycle [ms]
 * @param[in]   filepath                pixel image file path
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_map_thumb(int surface, int framerate, const char *filepath)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_map_thumb_msg(appid, surface, framerate, filepath);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_unmap_thumb
 *          Unmap the thumbnail data.
 *          User calls this API when receiving the notification that
 *          terminated the application.
 *
 * @param[in]   surface                 window's surface id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_unmap_thumb(int surface)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_unmap_thumb_msg(appid, surface);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_show_layer
 *          Show the layer.
 *
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_show_layer(int layer)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_layer_msg(appid, layer, _CMD_SHOW_LAYER);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_hide_layer
 *          Hide the layer.
 *
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_hide_layer(int layer)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_layer_msg(appid, layer, _CMD_HIDE_LAYER);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}
/* vim: set expandtab ts=4 sw=4: */
