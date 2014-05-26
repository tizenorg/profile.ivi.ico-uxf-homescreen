/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Input Control API
 *          for privilege applications
 *
 * @date    Feb-21-2014
 */

#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"
#include "ico_syc_privilege.h"

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static msg_t _create_add_input_msg(const char *appid, const char *device,
                                  int input, int fix, int keycode);
static msg_t _create_del_input_msg(const char *appid,
                                  const char *device, int input);
static msg_t _create_send_key_event_msg(const char *winname, int code, int value);
static msg_t _create_send_pointer_event_msg(const char *winname, int code, int value);

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_add_input_msg
 *          Create the message to add input device.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @param[in]   fix                     fixed allocation or variable allocation
 * @param[in]   keycode                 keycode
 *                                      (If set "0", not allocate to keycode)
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_add_input_msg(const char *appid, const char *device,
                     int input, int fix, int keycode)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_ADD_INPUT);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_string_member(argobj, MSG_PRMKEY_DEV, device);
    json_object_set_int_member(argobj, MSG_PRMKEY_INPUT, input);
    json_object_set_int_member(argobj, MSG_PRMKEY_ALLOC, fix);
    json_object_set_int_member(argobj, MSG_PRMKEY_KEY, keycode);
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
 * @brief   _create_del_input_msg
 *          Create the message to delete input device.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_del_input_msg(const char *appid, const char *device, int input)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_DEL_INPUT);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_string_member(argobj, MSG_PRMKEY_DEV, device);
    json_object_set_int_member(argobj, MSG_PRMKEY_INPUT, input);
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
 * @brief   _create_send_key_event_msg
 *          Create the message to send keyboard input event.
 *
 * @param[in]   winname                 window name
 * @param[in]   code                    input event code
 * @param[in]   value                   input event value
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_send_key_event_msg(const char *winname, int code, int value)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_SEND_KEY_EVENT);
    if (winname)    {
        json_object_set_string_member(obj, MSG_PRMKEY_APPID, winname);
    }
    else    {
        json_object_set_string_member(obj, MSG_PRMKEY_APPID, " ");
    }
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_EV_CODE, code);
    json_object_set_int_member(argobj, MSG_PRMKEY_EV_VAL, value);
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
 * @brief   _create_send_pointer_event_msg
 *          Create the message to send pointer input event.
 *
 * @param[in]   winname                 window name
 * @param[in]   code                    input event code
 * @param[in]   value                   input event value
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_send_pointer_event_msg(const char *winname, int code, int value)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_SEND_POINTER_EVENT);
    if (winname)    {
        json_object_set_string_member(obj, MSG_PRMKEY_APPID, winname);
    }
    else    {
        json_object_set_string_member(obj, MSG_PRMKEY_APPID, " ");
    }
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, winname);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_int_member(argobj, MSG_PRMKEY_EV_CODE, code);
    json_object_set_int_member(argobj, MSG_PRMKEY_EV_VAL, value);
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
/* public interface function                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_add_input
 *          Add the input event to the application.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @param[in]   fix                     fixed allocation or variable allocation
 * @param[in]   keycode                 keycode
 *                                      (If set "0", not allocate to keycode)
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 * @see         ico_syc_input_alloc_e
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_add_input(const char *appid, const char *device,
                  int input, int fix, int keycode)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL || device == NULL) {
        _ERR("invalid parameter (appid or device is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_add_input_msg(appid, device, input, fix, keycode);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_delete_input
 *          Delete the input event assigned to the application.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_delete_input(const char *appid, const char *device, int input)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* check argument */
    if (appid == NULL || device == NULL) {
        _ERR("invalid parameter (appid or device is NULL)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* make message */
    msg = _create_del_input_msg(appid, device, input);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_send_key_event
 *          Send the key input event to the application.
 *
 * @param[in]   winname                 window name([name][@appid])
 * @param[in]   code                    input event code
 * @param[in]   value                   input event value
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_send_key_event(const char *winname, int code, int value)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* make message */
    msg = _create_send_key_event_msg(winname, code, value);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_send_pointer_event
 *          Send the pointer input event to the application.
 *
 * @param[in]   winname                 window name([name][@appid])
 * @param[in]   code                    input event code
 * @param[in]   value                   input event value
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_send_pointer_event(const char *winname, int code, int value)
{
    int ret = ICO_SYC_ERR_NONE;
    msg_t msg;

    /* make message */
    msg = _create_send_pointer_event_msg(winname, code, value);
    /* send message */
    ret = ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return ret;
}
/* vim: set expandtab ts=4 sw=4: */
