/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Message API (Internal Function)
 *
 * @date    July-31-2013
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <ico_uws.h>

#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"
#include "ico_syc_type.h"

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static void _store_data(msg_str_t msg, size_t len);
static void _send_stored_data(void);

/*============================================================================*/
/* variable & table                                                           */
/*============================================================================*/
/* queue for send data */
GQueue *send_info_q = NULL;
GQueue *send_free_q = NULL;

/* pthread mutex initialize */
static pthread_mutex_t in_out_mutex = PTHREAD_MUTEX_INITIALIZER;

/* send data info table */
typedef struct _send_info {
    char    data[1024];
    size_t  len;
} send_info_t;

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _store_data
 *          Enqueue data that could not send.
 *
 * @param[in]   data                    data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_store_data(msg_str_t data, size_t len)
{
    send_info_t *send_data;

    /* mutex lock */
    pthread_mutex_lock(&in_out_mutex);
    if (g_queue_is_empty(send_free_q) == TRUE) {
        /* alloc memory */
        send_data = calloc(1, sizeof(send_info_t));
        if (send_data == NULL) {
            /* mutex unlock */
            pthread_mutex_unlock(&in_out_mutex);
            _ERR("calloc failed");
            return;
        }
    }
    else {
        /* get free memory */
        send_data = g_queue_pop_head(send_free_q);
        if (send_data == NULL) {
            /* mutex unlock */
            pthread_mutex_unlock(&in_out_mutex);
            _ERR("g_queue_pop_head failed");
            return;
        }
        /* clear data */
        memset(send_data, 0, sizeof(send_info_t));
    }

    /* set data */
    snprintf(send_data->data, sizeof(send_data->data), "%s", (char *)data);
    send_data->len = len;
    _DBG("store [%s]", send_data->data);

    /* enqueue */
    g_queue_push_tail(send_info_q, send_data);
    /* mutex unlock */
    pthread_mutex_unlock(&in_out_mutex);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _send_stored_data
 *          Send data that could not send before.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_send_stored_data(void)
{
    struct ico_uws_context *context = NULL;
    void *id                        = NULL;
    send_info_t *info               = NULL;

    /* get ico_uws context and id */
    context = ico_syc_get_context();
    id = ico_syc_get_id();
    if (context == NULL || id == NULL) {
        _ERR("connection error (ico_uws context or id is NULL)");
        return;
    }

    while (g_queue_is_empty(send_info_q) != TRUE) {
        /* mutex lock */
        pthread_mutex_lock(&in_out_mutex);
        /* pop data */
        info = g_queue_pop_head(send_info_q);
        /* mutex unlock */
        pthread_mutex_unlock(&in_out_mutex);
        _INFO("send stored data [%s]", info->data);

        /* send data */
        ico_uws_service(context);
        ico_uws_send(context, id, (unsigned char *)info->data, info->len);

        /* mutex lock */
        pthread_mutex_lock(&in_out_mutex);
        /* push free_queue */
        g_queue_push_tail(send_free_q, info);
        /* mutex unlock */
        pthread_mutex_unlock(&in_out_mutex);
    }

    return;
}

/*============================================================================*/
/* internal common function                                                   */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_init_msg
 *          Initialize to create and send message.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_init_msg(void)
{
    /* init queue */
    send_info_q = g_queue_new();
    send_free_q = g_queue_new();

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_send_msg
 *          Send message to System Controller.
 *          If set argument NULL, send stored message to System Controller.
 *
 * @param[in]   msg                     json object (msg_t)
 * @return      result
 * @return      0                       success
 * @return      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_send_msg(msg_t msg)
{
    struct ico_uws_context *context = NULL;
    void *id                        = NULL;
    msg_str_t data                  = NULL;
    size_t len                      = 0;

    /* check stored data exists */
    if (g_queue_is_empty(send_info_q) != TRUE) {
        /* send stored data */
        _send_stored_data();
    }

    /* if msg is NULL, exit this method */
    if (msg == NULL) {
        return ICO_SYC_ERR_NONE;
    }

    /* mutex lock */
    pthread_mutex_lock(&in_out_mutex);
    /* get data */
    data = json_generator_to_data(msg, &len);
    if (data == NULL) {
        /* mutex unlock */
        pthread_mutex_unlock(&in_out_mutex);
        _ERR("json_generator_to_data failed");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* get ico_uws context and id */
    context = ico_syc_get_context();
    id = ico_syc_get_id();
    if (context == NULL || id == NULL) {
        /* mutex unlock */
        pthread_mutex_unlock(&in_out_mutex);
        _store_data(data, len);
        g_free(data);
        _WARN("stored data (retry to send when connecting)");
        return ICO_SYC_ERR_CONNECT;
    }
    _INFO("send data [%s]", data);

    /* send data */
    ico_uws_service(context);
    ico_uws_send(context, id, (unsigned char *)data, len);

    /* free */
    g_free(data);
    /* mutex unlock */
    pthread_mutex_unlock(&in_out_mutex);

    return ICO_SYC_ERR_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_command
 *          Get command from message data.
 *
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      command
 * @return      command                 success
 * @return      -1                      error
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_get_command(const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    int cmd             = -1;

    /* check argument */
    if (data == NULL || len == 0) {
        _ERR("invalid parameter (data is NULL or length is 0)");
        return ICO_SYC_ERR_INVALID_PARAM;
    }

    /* start parser */
    parser = json_parser_new();
    gbool = json_parser_load_from_data(parser, data, len, &error);
    if (gbool == FALSE) {
        g_object_unref(parser);
        _ERR("json_parser_load_from_data failed");
        return -1;
    }

    /* get root node */
    root = json_parser_get_root(parser);
    if (root == NULL) {
        g_object_unref(parser);
        _ERR("json_parser_get_root failed (root is NULL)");
        return -1;
    }

    /* get object from root */
    obj = json_node_get_object(root);
    /* get command */
    cmd = json_object_get_int_member(obj, MSG_PRMKEY_CMD);

    /* unref */
    g_object_unref(parser);

    return cmd;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_str_member
 *          Get string member from json object.
 *
 * @param[in]   obj                     json object
 * @param[in]   name                    json object member name
 * @return      string data
 * @return      not NULL                success
 * @return      NULL                    error
 */
/*--------------------------------------------------------------------------*/
char *
ico_syc_get_str_member(JsonObject *obj, const gchar *name)
{
    char *str   = NULL;

    /* member exists or not */
    if (json_object_has_member(obj, name) == FALSE) {
        return NULL;
    }

    /* null check */
    if (json_object_get_null_member(obj, name) == TRUE) {
        return NULL;
    }

    /* get string member */
    str = (char *)json_object_get_string_member(obj, name);

    return str;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_int_member
 *          Get int member from json object.
 *
 * @param[in]   obj                     json object
 * @param[in]   name                    json object member name
 * @return      integer data
 * @return      num                     success
 * @return      -1                      error
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_get_int_member(JsonObject *obj, const gchar *name)
{
    int num   = -1;

    /* member exists or not */
    if (json_object_has_member(obj, name) == FALSE) {
        return -1;
    }

    /* null check */
    if (json_object_get_null_member(obj, name) == TRUE) {
        return -1;
    }

    /* get int member */
    num = json_object_get_int_member(obj, name);

    return num;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_free_msg
 *          Free the json object.
 *
 * @param[in]   msg                     json object (msg_t)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_free_msg(msg_t msg)
{
    /* check argument */
    if (msg == NULL) {
        _INFO("msg is NULL");
        return;
    }

    JsonNode *node = json_generator_get_root(msg);
    json_object_unref(json_node_get_object(node));
    json_node_free(node);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_term_msg
 *          Terminate to create and send message.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_term_msg(void)
{
    send_info_t *info;

    /* free queue */
    while (g_queue_is_empty(send_info_q) != TRUE) {
        /* free send_info_t */
        info = g_queue_pop_head(send_info_q);
        free(info);
    }
    g_queue_free(send_info_q);

    while (g_queue_is_empty(send_free_q) != TRUE) {
        /* free send_info_t */
        info = g_queue_pop_head(send_free_q);
        free(info);
    }
    g_queue_free(send_free_q);

    return;
}
/* vim: set expandtab ts=4 sw=4: */
