/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Application Resource Control API
 *          for general applications
 *
 * @date    Aug-7-2013
 */

#include <string.h>
#include <unistd.h>

#include <aul/aul.h>

#include "ico_syc_application.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static ico_syc_res_window_t *_create_res_window(char *zone,
                                                char *name, char *id);
static ico_syc_res_sound_t *_create_res_sound(char *zone, char *name, char *id,
                                              int adjust);
static ico_syc_res_input_t *_create_res_input(char *name, int event);
static void _free_res_window(ico_syc_res_window_t *window);
static void _free_res_sound(ico_syc_res_sound_t *sound);
static void _free_res_input(ico_syc_res_input_t *input);
static struct ico_syc_res_context * _create_context(
                                    char *appid,
                                    const ico_syc_res_window_t *window,
                                    const ico_syc_res_sound_t *sound,
                                    const ico_syc_res_input_t *input,
                                    int type);
/* create send message */
static JsonObject *_create_window_msg(ico_syc_res_window_t *window);
static JsonObject *_create_sound_msg(ico_syc_res_sound_t *sound);
static JsonObject *_create_input_msg(ico_syc_res_input_t *input);
static msg_t _create_acquire_res_msg(const struct ico_syc_res_context *context);
static msg_t _create_release_res_msg(const struct ico_syc_res_context *context);
static msg_t _create_set_region_msg(const char *appid,
                                    const ico_syc_input_region_t *input,
                                    int attr);
static msg_t _create_unset_region_msg(const char *appid,
                                      const ico_syc_input_region_t *input);

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_res_window
 *          Allocate memory for window resource information.
 *
 * @param[in]   zone                    window zone
 * @param[in]   name                    windo name
 * @param[in]   id                      window id
 * @return      resource info
 * @retval      address                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static ico_syc_res_window_t *
_create_res_window(char *zone, char *name, char *id)
{
    ico_syc_res_window_t *info  = NULL;

    if (zone == NULL || name == NULL) {
        _ERR("invalid parameter (zone, name is NULL)");
        return NULL;
    }

    /* alloc memory */
    info = calloc(1, sizeof(ico_syc_res_window_t));
    if (info == NULL) {
        _ERR("calloc failed");
        return NULL;
    }

    /* set element */
    info->zone = strdup(zone);
    info->name = strdup(name);
    if (id != NULL) {
        info->id = strdup(id);
    }

    return info;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_res_sound
 *          Allocate memory for sound resource information.
 *
 * @param[in]   zone                    sound zone
 * @param[in]   name                    sound name
 * @param[in]   id                      sound id
 * @param[in]   adjust                  adjust type
 * @return      resource info
 * @retval      address                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static ico_syc_res_sound_t *
_create_res_sound(char *zone, char *name, char *id, int adjust)
{
    ico_syc_res_sound_t *info   = NULL;

    if (zone == NULL || name == NULL) {
        _ERR("invalid parameter (zone, name is NULL)");
        return NULL;
    }

    /* alloc memory */
    info = calloc(1, sizeof(ico_syc_res_sound_t));
    if (info == NULL) {
        _ERR("calloc failed");
        return NULL;
    }

    /* set element */
    info->zone = strdup(zone);
    info->name = strdup(name);
    if (id != NULL) {
        info->id = strdup(id);
    }
    info->adjust = adjust;

    return info;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_res_input
 *          Allocate memory for input resource information.
 *
 * @param[in]   name                    input device name
 * @param[in]   event                   input event
 * @return      resource info
 * @retval      address                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static ico_syc_res_input_t *
_create_res_input(char *name, int event)
{
    ico_syc_res_input_t *info   = NULL;

    if (name == NULL) {
        _ERR("invalid parameter (name is NULL)");
        return NULL;
    }

    /* alloc memory */
    info = calloc(1, sizeof(ico_syc_res_input_t));
    if (info == NULL) {
        _ERR("calloc failed");
        return NULL;
    }

    /* set element */
    info->name = strdup(name);
    info->event = event;

    return info;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _free_res_window
 *          Free memory of window resource information.
 *
 * @param[in]   window                  window resource's information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_free_res_window(ico_syc_res_window_t *window)
{
    if (window == NULL) {
        return;
    }

    /* free element */
    if (window->zone != NULL) free(window->zone);
    if (window->name != NULL) free(window->name);
    if (window->id != NULL) free(window->id);
    /* free */
    free(window);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _free_res_sound
 *          Free memory of sound resource information.
 *
 * @param[in]   sound                   sound resource's information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_free_res_sound(ico_syc_res_sound_t *sound)
{
    if (sound == NULL) {
        return;
    }

    /* free element */
    if (sound->zone != NULL) free(sound->zone);
    if (sound->name != NULL) free(sound->name);
    if (sound->id != NULL) free(sound->id);
    /* free */
    free(sound);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _free_res_input
 *          Free memory of input resource information.
 *
 * @param[in]   input                   input resource's information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_free_res_input(ico_syc_res_input_t *input)
{
    if (input == NULL) {
        return;
    }

    /* free element */
    if (input->name != NULL) free(input->name);
    /* free */
    free(input);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_context
 *          Create resource information context.
 *
 * @param[in]   appid                   application id
 * @param[in]   window                  window resource's information
 * @param[in]   sound                   sound resource's information
 * @param[in]   input                   input resource's information
 * @param[in]   type                    window and sound resource type
 *                                      (basic or interruption)
 * @return      resource's context address
 * @retval      address                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static struct ico_syc_res_context *
_create_context(char *appid, const ico_syc_res_window_t *window,
                const ico_syc_res_sound_t *sound,
                const ico_syc_res_input_t *input,
                int type)
{
    struct ico_syc_res_context *context = NULL;

    context = (struct ico_syc_res_context *)
                              calloc(1, sizeof(struct ico_syc_res_context));
    if (context == NULL) {
        _ERR("calloc failed");
        return NULL;
    }

    /* set appid */
    strcpy(context->appid, appid);

    /* set window info */
    if (window != NULL) {
        context->window = _create_res_window(window->zone, window->name,
                                             window->id);
    }
    /* set sound info */
    if (sound != NULL) {
        context->sound = _create_res_sound(sound->zone, sound->name,
                                           sound->id, sound->adjust);
    }
    /* set input info */
    if (input != NULL) {
        context->input = _create_res_input(input->name, input->event);
    }

    /* set resource type */
    if (window != NULL || sound != NULL) {
        context->type = type;
    }

    return context;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_window_msg
 *          Create the JsonObject to set the window resource information.
 *
 * @param[in]   window                  window resource information
 * @return      json object
 * @retval      json object             success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static JsonObject *
_create_window_msg(ico_syc_res_window_t *window)
{
    JsonObject *info    = NULL;

    if (window == NULL) {
        _ERR("invalid parameter (window is NULL)");
        return NULL;
    }

    /* create json object */
    info = json_object_new();
    if (info == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set member */
    json_object_set_string_member(info, MSG_PRMKEY_RES_ZONE, window->zone);
    json_object_set_string_member(info, MSG_PRMKEY_RES_NAME, window->name);
    json_object_set_string_member(info, MSG_PRMKEY_RES_ID, window->id);

    return info;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_sound_msg
 *          Create the JsonObject to set the sound resource information.
 *
 * @param[in]   sound                   sound resource information
 * @return      json object
 * @retval      json object             success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static JsonObject *
_create_sound_msg(ico_syc_res_sound_t *sound)
{
    JsonObject *info    = NULL;

    if (sound == NULL) {
        _ERR("invalid parameter (sound is NULL)");
        return NULL;
    }

    /* create json object */
    info = json_object_new();
    if (info == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set member */
    json_object_set_string_member(info, MSG_PRMKEY_RES_ZONE, sound->zone);
    json_object_set_string_member(info, MSG_PRMKEY_RES_NAME, sound->name);
    json_object_set_string_member(info, MSG_PRMKEY_RES_ID, sound->id);
    json_object_set_int_member(info, MSG_PRMKEY_RES_ADJUST, sound->adjust);

    return info;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_input_msg
 *          Create the JsonObject to set the input resource information.
 *
 * @param[in]   input                   input resource information
 * @return      json object
 * @retval      json object             success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static JsonObject *
_create_input_msg(ico_syc_res_input_t *input)
{
    JsonObject *info    = NULL;

    if (input == NULL) {
        _ERR("invalid parameter (input is NULL)");
        return NULL;
    }

    /* create json object */
    info = json_object_new();
    if (info == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set member */
    json_object_set_string_member(info, MSG_PRMKEY_RES_NAME, input->name);
    json_object_set_int_member(info, MSG_PRMKEY_RES_EV, input->event);

    return info;
}


/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_acquire_res_msg
 *          Create the message to acquire the resource (window/sound/input).
 *
 * @param[in]   context                 resource context
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_acquire_res_msg(const struct ico_syc_res_context *context)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonObject *info    = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    resobj = json_object_new();
    if (obj == NULL || resobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_ACQUIRE_RES);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, context->appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    /* window resource */
    if (context->window != NULL) {
        info = _create_window_msg(context->window);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_WINDOW, info);
    }

    /* sound resource */
    if (context->sound != NULL) {
        info = _create_sound_msg(context->sound);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_SOUND, info);
    }

    /* input resource */
    if (context->input != NULL) {
        info = _create_input_msg(context->input);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_INPUT, info);
    }

    /* resource type */
    if (context->window != NULL || context->sound != NULL) {
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_TYPE, context->type);
    }
    /* set resource object */
    json_object_set_object_member(obj, MSG_PRMKEY_RES, resobj);

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
 * @brief   _create_release_res_msg
 *          Create the message to release the resource (window/sound/input).
 *
 * @param[in]   context                 resource context
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_release_res_msg(const struct ico_syc_res_context *context)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonObject *info    = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    resobj = json_object_new();
    if (obj == NULL || resobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_RELEASE_RES);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, context->appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    /* window resource */
    if (context->window != NULL) {
        info = _create_window_msg(context->window);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_WINDOW, info);
    }

    /* sound resource */
    if (context->sound != NULL) {
        info = _create_sound_msg(context->sound);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_SOUND, info);
    }

    /* input resource */
    if (context->input != NULL) {
        info = _create_input_msg(context->input);
        /* set object */
        json_object_set_object_member(resobj, MSG_PRMKEY_RES_INPUT, info);
    }
    /* resource type */
    if (context->window != NULL || context->sound != NULL) {
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_TYPE, context->type);
    }
    /* set resource object */
    json_object_set_object_member(obj, MSG_PRMKEY_RES, resobj);

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
 * @brief   _create_set_region_msg
 *          Create the message to set the input region.
 *
 * @param[in]   appid                   application id
 * @param[in]   input                   input region's information
 * @param[in]   attr                    input region's attribute
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_set_region_msg(const char *appid, const ico_syc_input_region_t *input,
                       int attr)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    resobj = json_object_new();
    if (obj == NULL || resobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_SET_REGION);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_string_member(resobj, MSG_PRMKEY_WINNAME, input->winname);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_X, input->pos_x);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_Y, input->pos_y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_WIDTH, input->width);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HEIGHT, input->height);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HOT_X, input->hotspot_x);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_HOT_Y, input->hotspot_y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_X, input->cursor_x);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_Y, input->cursor_y);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_WIDTH, input->cursor_width);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_CUR_HEIGHT, input->cursor_height);
    json_object_set_int_member(resobj, MSG_PRMKEY_RES_ATTR, attr);
    json_object_set_object_member(obj, MSG_PRMKEY_REGION, resobj);

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
 * @brief   _create_unset_region_msg
 *          Create the message to unset the input region.
 *
 * @param[in]   appid                   application id
 * @param[in]   input                   input region's information
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_unset_region_msg(const char *appid, const ico_syc_input_region_t *input)
{
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    resobj = json_object_new();
    if (obj == NULL || resobj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_UNSET_REGION);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    if (input)  {
        json_object_set_string_member(resobj, MSG_PRMKEY_WINNAME, input->winname);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_X, input->pos_x);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_Y, input->pos_y);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_WIDTH, input->width);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_HEIGHT, input->height);
    }
    else    {
        json_object_set_string_member(resobj, MSG_PRMKEY_WINNAME, " ");
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_X, 0);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_POS_Y, 0);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_WIDTH, 0);
        json_object_set_int_member(resobj, MSG_PRMKEY_RES_HEIGHT, 0);
    }
    json_object_set_object_member(obj, MSG_PRMKEY_REGION, resobj);

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
 * @brief   ico_syc_cb_res
 *          Execute callback function. (ICO_SYC_EV_RES_ACQUIRE
 *                                      ICO_SYC_EV_RES_DEPRIVE
 *                                      ICO_SYC_EV_RES_WAITING
 *                                      ICO_SYC_EV_RES_REVERT
 *                                      ICO_SYC_EV_RES_RELEASE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               pased data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_res(ico_syc_callback_t callback, void *user_data,
               int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;
    JsonObject *info    = NULL;

    ico_syc_res_info_t *res_info    = NULL;
    char *zone, *name, *id;
    int adjust, input_ev;

    /* alloc memory */
    res_info = calloc(1, sizeof(ico_syc_res_info_t));
    if (res_info == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(res_info);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(res_info);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check object */
    if (json_object_has_member(obj, MSG_PRMKEY_RES) == FALSE) {
        _ERR("invalid message");
        return;
    }
    resobj = json_object_get_object_member(obj, MSG_PRMKEY_RES);

    /* window resource */
    if (json_object_has_member(resobj, MSG_PRMKEY_RES_WINDOW) == TRUE) {
        info = json_object_get_object_member(resobj, MSG_PRMKEY_RES_WINDOW);
        zone = ico_syc_get_str_member(info, MSG_PRMKEY_RES_ZONE);
        name = ico_syc_get_str_member(info, MSG_PRMKEY_RES_NAME);
        id = ico_syc_get_str_member(info, MSG_PRMKEY_RES_ID);

        res_info->window = _create_res_window(zone, name, id);
    }

    /* sound resource */
    if (json_object_has_member(resobj, MSG_PRMKEY_RES_SOUND) == TRUE) {
        info = json_object_get_object_member(resobj, MSG_PRMKEY_RES_SOUND);
        zone = ico_syc_get_str_member(info, MSG_PRMKEY_RES_ZONE);
        name = ico_syc_get_str_member(info, MSG_PRMKEY_RES_NAME);
        id = ico_syc_get_str_member(info, MSG_PRMKEY_RES_ID);
        adjust = ico_syc_get_int_member(info, MSG_PRMKEY_RES_ADJUST);

        res_info->sound = _create_res_sound(zone, name, id, adjust);
    }

    /* input resource */
    if (json_object_has_member(resobj, MSG_PRMKEY_RES_INPUT) == TRUE) {
        info = json_object_get_object_member(resobj, MSG_PRMKEY_RES_INPUT);
        name = ico_syc_get_str_member(info, MSG_PRMKEY_RES_NAME);
        input_ev = ico_syc_get_int_member(info, MSG_PRMKEY_RES_EV);

        res_info->input = _create_res_input(name, input_ev);
    }

    /* exec callback */
    callback(event, res_info, user_data);

    /* free memory */
    g_object_unref(parser);
    _free_res_window(res_info->window);
    _free_res_sound(res_info->sound);
    _free_res_input(res_info->input);
    free(res_info);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_region
 *          Execute callback function.  (ICO_SYC_EV_INPUT_SET
 *                                       ICO_SYC_EV_INPUT_UNSET)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               pased data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_region(ico_syc_callback_t callback, void *user_data,
                  int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *resobj  = NULL;

    ico_syc_input_region_t *region  = NULL;

    /* alloc memory */
    region = calloc(1, sizeof(ico_syc_input_region_t));
    if (region == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(region);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(region);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check object */
    if (json_object_has_member(obj, MSG_PRMKEY_REGION) == FALSE) {
        _ERR("invalid message");
        return;
    }
    resobj = json_object_get_object_member(obj, MSG_PRMKEY_REGION);

    /* get input region information */
    char *p = ico_syc_get_str_member(resobj, MSG_PRMKEY_WINNAME);
    if (p)  {
        strncpy(region->winname, p, ICO_SYC_MAX_WINNAME_LEN-1);
    }
    region->pos_x = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_POS_X);
    region->pos_y = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_POS_Y);
    region->width = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_WIDTH);
    region->height = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_HEIGHT);
    region->hotspot_x = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_HOT_X);
    region->hotspot_y = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_HOT_Y);
    region->cursor_x = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_CUR_X);
    region->cursor_y = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_CUR_Y);
    region->cursor_width = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_CUR_WIDTH);
    region->cursor_height = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_CUR_HEIGHT);
    region->attr = ico_syc_get_int_member(resobj, MSG_PRMKEY_RES_ATTR);

    /* exec callback */
    callback(event, region, user_data);

    /* free memory */
    g_object_unref(parser);
    free(region);

    return;
}

/*============================================================================*/
/* public interface function                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_acquire_res
 *          Acquire the resources(window/sound/input).
 *          User can choose window and sound resource's type
 *          "basic" or "interruption".
 *          If user want to acquire the sound resource only,
 *          user sets NULL to argument "window" and  "input".
 *
 * @param[in]   window                  window resource's information
 * @param[in]   sound                   sound resource's information
 * @param[in]   input                   input resource's information
 * @param[in]   type                    window and sound resource's type
 *                                      (basic or interruption)
 * @return      resource's context address
 * @retval      address                 success
 * @retval      NULL                    error
 * @see         ico_syc_res_type_e
 */
/*--------------------------------------------------------------------------*/
ICO_API struct ico_syc_res_context *
ico_syc_acquire_res(const ico_syc_res_window_t *window,
                    const ico_syc_res_sound_t *sound,
                    const ico_syc_res_input_t *input,
                    int type)
{
    msg_t msg;
    char *appid;
    struct ico_syc_res_context *context = NULL;

    /* check argument */
    if (window == NULL && sound == NULL && input == NULL) {
        _ERR("invalid parameter (all resources is NULL)");
        return NULL;
    }

    /* get appid */
    appid = ico_syc_get_appid();

    /* create context */
    context = _create_context(appid, window, sound, input, type);
    if (context == NULL) {
        _ERR("context is NULL");
        return NULL;
    }

    /* make message */
    msg = _create_acquire_res_msg(context);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return context;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_release_res
 *          Release the resources(window/sound/input).
 *
 * @param[in]   context                 resource context
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_release_res(struct ico_syc_res_context *context)
{
    msg_t msg;

    /* check argument */
    if (context == NULL) {
        _ERR("invalid parameter (context is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_release_res_msg(context);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    /* free resource info */
    _free_res_window(context->window);
    _free_res_sound(context->sound);
    _free_res_input(context->input);
    /* free context */
    free(context);

    return ICO_SYC_ERR_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_set_input_region
 *          Set the input region. (for haptic device)
 *          Callback function notifies the result of setting the input region.
 *
 * @param[in]   input                   input region's information
 * @param[in]   attr                    input region's attribute
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_set_input_region(const ico_syc_input_region_t *input, int attr)
{
    msg_t msg;
    char *appid;

    /* check argument */
    if (input == NULL) {
        _ERR("invalid parameter (ico_syc_input_region_t is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_set_region_msg(appid, input, attr);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ICO_SYC_ERR_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_unset_input_region
 *          Unset the input region. (for haptic device)
 *          Callback function notifies the result of unsetting the input region.
 *
 * @param[in]   input                   input region's information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_unset_input_region(const ico_syc_input_region_t *input)
{
    msg_t msg;
    char *appid;

    /* check argument */
    if (input == NULL) {
        _ERR("invalid parameter (ico_syc_input_region_t is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_unset_region_msg(appid, input);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ICO_SYC_ERR_NONE;
}

/* vim: set expandtab ts=4 sw=4: */
