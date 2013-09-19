/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   User Control API
 *          for privilege applications
 *
 * @date    Sep-4-2013
 */

#include <string.h>

#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"
#include "ico_syc_privilege.h"

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static msg_t _create_change_user_msg(const char *appid, const char *name,
                                     const char *password);
static msg_t _create_get_userlist_msg(const char *appid);
static msg_t _create_get_lastinfo_msg(const char *appid);
static msg_t _create_set_lastinfo_msg(const char *appid, const char *lastinfo);

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_change_user_msg
 *          Create the message to change the login user.
 *
 * @param[in]   appid                   application id
 * @param[in]   name                    user name
 * @param[in]   password                user's password
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_change_user_msg(const char *appid, const char *name,
                        const char *password)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_CHANGE_USER);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_string_member(argobj, MSG_PRMKEY_USER, name);
    json_object_set_string_member(argobj, MSG_PRMKEY_PASSWD, password);
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
 * @brief   _create_get_userlist_msg
 *          Create the message to get the user list.
 *
 * @param[in]   appid                   application id
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_get_userlist_msg(const char *appid)
{
    JsonObject *obj     = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    if (obj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_GET_USERLIST);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());
    json_object_set_null_member(obj, MSG_PRMKEY_ARG);

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
 * @brief   _create_get_lastinfo_msg
 *          Create the message to get the application's last information.
 *
 * @param[in]   appid                   application id
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_get_lastinfo_msg(const char *appid)
{
    JsonObject *obj     = NULL;
    JsonGenerator *gen  = NULL;
    JsonNode *root      = NULL;

    /* create json object */
    obj = json_object_new();
    if (obj == NULL) {
        _ERR("json_object_new failed");
        return NULL;
    }

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_GET_LASTINFO);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());
    json_object_set_null_member(obj, MSG_PRMKEY_ARG);

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
 * @brief   _create_set_lastinfo_msg
 *          Create the message to set the application's last information.
 *
 * @param[in]   appid                   application id
 * @param[in]   lastinfo                application's last information
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_set_lastinfo_msg(const char *appid, const char *lastinfo)
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
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_GET_LASTINFO);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    json_object_set_string_member(argobj, MSG_PRMKEY_LASTINFO, lastinfo);
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
 * @brief   ico_syc_cb_auth
 *          Execute callback function. (ICO_SYC_EV_AUTH_FAIL)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_cb_auth(ico_syc_callback_t callback, void *user_data, int event)
{
    /* exec callback (no detail data) */
    callback(event, NULL, user_data);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_userlist
 *          Execute callback function. (ICO_SYC_EV_USERLIST)
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
ico_syc_cb_userlist(ico_syc_callback_t callback, void *user_data,
                    int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;
    JsonArray *array    = NULL;
    char **list         = NULL;
    int id, num;

    ico_syc_userlist_t  *userlist   = NULL;

    /* alloc memory */
    userlist = calloc(1, sizeof(ico_syc_userlist_t));
    if (userlist == NULL) {
        _ERR("calloc failed");
        return;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        free(userlist);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        free(userlist);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check object */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        /* free */
        free(userlist);
        _ERR("invalid message");
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);
    /* get user num */
    num = ico_syc_get_int_member(argobj, MSG_PRMKEY_USER_NUM);
    userlist->user_num = num;

    /* get userlist */
    if (json_object_has_member(argobj, MSG_PRMKEY_USER_LIST) == FALSE) {
        userlist->userlist = NULL;
        _WARN("userlist does not exist");
    }
    else {
        /* alloc memory */
        list = calloc(1, sizeof(char *) * num);
        if (list == NULL) {
            g_object_unref(parser);
            free(userlist);
            _ERR("calloc failed");
            return;
        }
        /* set user name */
        array = json_object_get_array_member(argobj, MSG_PRMKEY_USER_LIST);
        for (id = 0; id < num; id++) {
            list[id] = strdup(json_array_get_string_element(array, id));
        }
        userlist->userlist = list;
    }

    /* set login user name */
    userlist->user_login = strdup(ico_syc_get_str_member
                                              (argobj, MSG_PRMKEY_USER_LOGIN));

    /* exec callback */
    callback(event, userlist, user_data);

    /* free memory */
    g_object_unref(parser);
    if (list != NULL) {
        for (id = 0; id < num; id++) {
            free(list[id]);
        }
        free(list);
    }
    free(userlist->user_login);
    free(userlist);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_lastinfo
 *          Execute callback function. (ICO_SYC_EV_LASTINFO)
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
ico_syc_cb_lastinfo(ico_syc_callback_t callback, void *user_data, int event,
                    const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    char *lastinfo      = NULL;

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        _ERR("json_parser_load_from_data failed");
        return;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        _ERR("json_parser_get_root failed (root is NULL)");
        return;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* check message */
    if (json_object_has_member(obj, MSG_PRMKEY_ARG) == FALSE) {
        g_object_unref(parser);
        _INFO("last information does not exist");
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);

    /* check message */
    if (json_object_has_member(argobj, MSG_PRMKEY_LASTINFO) == FALSE) {
        g_object_unref(parser);
        _INFO("last information does not exist");
        return;
    }

    /* set data */
    lastinfo = strdup(ico_syc_get_str_member(argobj, MSG_PRMKEY_LASTINFO));

    /* exec callback */
    callback(event, lastinfo, user_data);

    /* free memory */
    g_object_unref(parser);
    if (lastinfo != NULL) free(lastinfo);

    return;
}

/*============================================================================*/
/* public interface function                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_user
 *          Change the login user.
 *          If fails to authenticate the user,
 *          callback function notifies the result.
 *
 * @param[in]   name                    user name
 * @param[in]   password                user's password
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_change_user(const char *name, const char *password)
{
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_change_user_msg(appid, name, password);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_get_userlist
 *          Get the user list.
 *          Callback function notifies the user list.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_get_userlist(void)
{
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_get_userlist_msg(appid);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_get_lastinfo
 *          Get the application's last information.
 *          Callback function notifies the application's last information.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_get_lastinfo(void)
{
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_get_lastinfo_msg(appid);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_set_lastinfo
 *          Set the application's last information.
 *
 * @param[in]   lastinfo                application's last information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_set_lastinfo(const char *lastinfo)
{
    msg_t msg;
    char *appid;

    /* get appid */
    appid = ico_syc_get_appid();

    /* make message */
    msg = _create_set_lastinfo_msg(appid, lastinfo);
    /* send message */
    (void)ico_syc_send_msg(msg);
    /* free send message */
    ico_syc_free_msg(msg);

    return;
}
/* vim: set expandtab ts=4 sw=4: */
