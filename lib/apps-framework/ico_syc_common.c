/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Common API to connect System Controller
 *          for privilege and general applications
 *
 * @date    Sep-4-2013
 */

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <aul.h>
#include <glib.h>
#include <ico_uws.h>

#include <Ecore.h>
#include <Eina.h>

#include "ico_syc_common.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_msg.h"
#include "ico_syc_private.h"

/*============================================================================*/
/* define static function prototype                                           */
/*============================================================================*/
static msg_t _create_appid_msg(void);
static void _syc_ev_callback(const struct ico_uws_context *uws_context,
                             const ico_uws_evt_e event,
                             const void *id,
                             const ico_uws_detail *detail,
                             void *user_data);
static void _add_queue(void *data, size_t len);
static int _get_event_from_cmd(int command);
static void _exec_callback(void *user_data);
static Eina_Bool _ecore_fd_cb(void *data, Ecore_Fd_Handler *handler);
static void _add_poll_fd(int fd);
static void _del_poll_fd(int fd);
static void *_poll_fd_thread(void *args);
static int _connect_client(ico_syc_callback_t callback, void *user_data);
static void _disconnect_client(void);
static void ico_syc_cb_notify_changed_state(ico_syc_callback_t callback,
                                            void *user_data, int event,
                                            const void *data, size_t len);

/*============================================================================*/
/* variable & table                                                           */
/*============================================================================*/
static char syc_appid[ICO_SYC_MAX_LEN];

static struct ico_uws_context *uws_context  = NULL;
static void *uws_id                         = NULL;
static ico_syc_callback_t syc_callback      = NULL;
static void *syc_user_data                  = NULL;
static Ecore_Fd_Handler *ecore_fd_handler   = NULL;

/* queue for receive data */
GQueue *recv_info_q = NULL;
GQueue *recv_free_q = NULL;

/* receive data info table */
typedef struct _recv_info {
    char    data[1024];
    size_t  len;
} recv_info_t;

/* pthread mutex initialize */
static pthread_mutex_t q_mutex  = PTHREAD_MUTEX_INITIALIZER;

/* semaphore */
static sem_t recv_sem;

/*============================================================================*/
/* static function                                                            */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   _create_appid_msg
 *          Create the message to send application id.
 *
 * @return      json generator
 * @retval      json generator          success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
static msg_t
_create_appid_msg(void)
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

    /* create root object */
    root = json_node_new(JSON_NODE_OBJECT);
    json_node_take_object(root, obj);

    /* set message */
    json_object_set_int_member(obj, MSG_PRMKEY_CMD, MSG_CMD_SEND_APPID);
    json_object_set_string_member(obj, MSG_PRMKEY_APPID, syc_appid);
    json_object_set_int_member(obj, MSG_PRMKEY_PID, getpid());

    /* create generator object */
    gen = json_generator_new();
    json_generator_set_root(gen, root);

    return gen;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _syc_ev_callback
 *          Callback Function.
 *
 * @param[in]   uws_context             ico_uws context
 * @param[in]   event                   event code
 * @param[in]   id                      unique id
 * @param[in]   detail                  detail information
 * @param[in]   user_data               passed data on called callback function
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
static void
_syc_ev_callback(const struct ico_uws_context *uws_context,
                 const ico_uws_evt_e event, const void *id,
                 const ico_uws_detail *detail, void *user_data)
{
    msg_t msg = NULL;

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        uws_id = (void *)id;
        /* send appid and retry to send message */
        msg = _create_appid_msg();
        ico_syc_send_msg(msg);
        break;
    case ICO_UWS_EVT_CLOSE:
        uws_id = NULL;
        break;
    case ICO_UWS_EVT_ERROR:
        break;
    case ICO_UWS_EVT_RECEIVE:
        _add_queue(detail->_ico_uws_message.recv_data,
                   detail->_ico_uws_message.recv_len);
        break;
    case ICO_UWS_EVT_ADD_FD:
        _add_poll_fd(detail->_ico_uws_fd.fd);
        break;
    case ICO_UWS_EVT_DEL_FD:
        _del_poll_fd(detail->_ico_uws_fd.fd);
        break;
    default:
        break;
    }

    /* free message */
    if (msg != NULL) {
        ico_syc_free_msg(msg);
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _add_queue
 *          Enqueue recieve data.
 *
 * @param[in]   data                    data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_add_queue(void *data, size_t len)
{
    recv_info_t *recv_data;

    /* mutex lock */
    pthread_mutex_lock(&q_mutex);
    if (g_queue_is_empty(recv_free_q) == TRUE) {
        /* alloc memory */
        recv_data = calloc(1, sizeof(recv_info_t));
        if (recv_data == NULL) {
            /* mutex unlock */
            pthread_mutex_unlock(&q_mutex);
            _ERR("calloc failed");
            return;
        }
    }
    else {
        /* get free memory */
        recv_data = g_queue_pop_head(recv_free_q);
        if (recv_data == NULL) {
            /* mutex unlock */
            pthread_mutex_unlock(&q_mutex);
            _ERR("g_queue_pop_head failed");
            return;
        }
        /* clear data */
        memset(recv_data, 0, sizeof(recv_info_t));
    }

    /* set data */
    snprintf(recv_data->data, sizeof(recv_data->data), "%s", (char *)data);
    recv_data->len = len;

    /* enqueue */
    g_queue_push_tail(recv_info_q, recv_data);
    /* mutex unlock */
    pthread_mutex_unlock(&q_mutex);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _get_event_from_cmd
 *          Get event id from command.
 *
 * @param[in]   command                 command
 * @return      event id
 * @retval      event id                success
 * @retval      -1                      error
 */
/*--------------------------------------------------------------------------*/
static int _get_event_from_cmd(int command)
{
    int event   = -1;
    switch (command) {
    case MSG_CMD_CREATE:
        event = ICO_SYC_EV_WIN_CREATE;
        break;
    case MSG_CMD_DESTROY:
        event = ICO_SYC_EV_WIN_DESTROY;
        break;
    case MSG_CMD_CHANGE_ATTR:
        event = ICO_SYC_EV_WIN_ATTR_CHANGE;
        break;
    case MSG_CMD_CHANGE_ACTIVE:
        event = ICO_SYC_EV_WIN_ACTIVE;
        break;
    case MSG_CMD_MAP_THUMB:
        event = ICO_SYC_EV_THUMB_CHANGE;
        break;
    case MSG_CMD_UNMAP_THUMB:
        event = ICO_SYC_EV_THUMB_UNMAP;
        break;
    case MSG_CMD_CHANGE_LAYER_ATTR:
        event = ICO_SYC_EV_LAYER_ATTR_CHANGE;
        break;
    case MSG_CMD_CHANGE_USER:
        event = ICO_SYC_EV_AUTH_FAIL;
        break;
    case MSG_CMD_GET_USERLIST:
        event = ICO_SYC_EV_USERLIST;
        break;
    case MSG_CMD_GET_LASTINFO:
        event = ICO_SYC_EV_LASTINFO;
        break;
    case MSG_CMD_ACQUIRE_RES:
        event = ICO_SYC_EV_RES_ACQUIRE;
        break;
    case MSG_CMD_DEPRIVE_RES:
        event = ICO_SYC_EV_RES_DEPRIVE;
        break;
    case MSG_CMD_WAITING_RES:
        event = ICO_SYC_EV_RES_WAITING;
        break;
    case MSG_CMD_REVERT_RES:
        event = ICO_SYC_EV_RES_REVERT;
        break;
    case MSG_CMD_RELEASE_RES:
        event = ICO_SYC_EV_RES_RELEASE;
        break;
    case MSG_CMD_SET_REGION:
        event = ICO_SYC_EV_INPUT_SET;
        break;
    case MSG_CMD_UNSET_REGION:
        event = ICO_SYC_EV_INPUT_UNSET;
        break;
    case MSG_CMD_NOTIFY_CHANGED_STATE:
        event = ICO_SYC_EV_STATE_CHANGE;
        break;
    case MSG_CMD_NAME:
        event = ICO_SYC_EV_WIN_NAME;
        break;
    default:
        break;
    }

    return event;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _exec_callback
 *          Execute callback function.
 *
 * @param[in]   user_data               passed data on called callback function
 * @return      result
 * @retval      0                       success (always success)
 */
/*--------------------------------------------------------------------------*/
static void
_exec_callback(void *user_data)
{
    recv_info_t *recv_data  = NULL;
    int cmd                 = -1;
    int event;

    if (syc_callback == NULL) {
        _DBG("not setting callback function");
        return;
    }

    while (g_queue_is_empty(recv_info_q) != TRUE) {
        /* mutex lock */
        pthread_mutex_lock(&q_mutex);
        recv_data = g_queue_pop_head(recv_info_q);
        /* mutex unlock */
        pthread_mutex_unlock(&q_mutex);

        /* exec callback */
        cmd = ico_syc_get_command(recv_data->data, recv_data->len);
        event = _get_event_from_cmd(cmd);
        switch (event) {
        case ICO_SYC_EV_WIN_CREATE:
        case ICO_SYC_EV_WIN_DESTROY:
        case ICO_SYC_EV_WIN_ACTIVE:
        case ICO_SYC_EV_WIN_NAME:
            ico_syc_cb_win(syc_callback, user_data, event,
                           recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_WIN_ATTR_CHANGE:
            ico_syc_cb_win_attr(syc_callback, user_data, event,
                                recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_THUMB_CHANGE:
        case ICO_SYC_EV_THUMB_UNMAP:
        case ICO_SYC_EV_THUMB_ERROR:
            ico_syc_cb_thumb(syc_callback, user_data, event,
                             recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_LAYER_ATTR_CHANGE:
            ico_syc_cb_layer(syc_callback, user_data, event,
                             recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_USERLIST:
            ico_syc_cb_userlist(syc_callback, user_data, event,
                                recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_LASTINFO:
            ico_syc_cb_lastinfo(syc_callback, user_data, event,
                                recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_AUTH_FAIL:
            ico_syc_cb_auth(syc_callback, user_data, event);
            break;
        case ICO_SYC_EV_RES_ACQUIRE:
        case ICO_SYC_EV_RES_DEPRIVE:
        case ICO_SYC_EV_RES_WAITING:
        case ICO_SYC_EV_RES_REVERT:
        case ICO_SYC_EV_RES_RELEASE:
            ico_syc_cb_res(syc_callback, user_data, event,
                           recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_INPUT_SET:
        case ICO_SYC_EV_INPUT_UNSET:
            ico_syc_cb_region(syc_callback, user_data, event,
                              recv_data->data, recv_data->len);
            break;
        case ICO_SYC_EV_STATE_CHANGE:
            ico_syc_cb_notify_changed_state(syc_callback, user_data, event,
                                            recv_data->data, recv_data->len);
            break;
        default:
            break;
        }

        /* mutex lock */
        pthread_mutex_lock(&q_mutex);
        /* recv_info push free_queue */
        g_queue_push_tail(recv_free_q, recv_data);
        /* mutex unlock */
        pthread_mutex_unlock(&q_mutex);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _ecore_fd cb
 *          ecore FD callback function.
 *
 * @return      result
 * @retval      ECORE_CALLBACK_RENEW    success (always success)
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
_ecore_fd_cb(void *data, Ecore_Fd_Handler *handler)
{
    int ret = 0;
    _DBG("_ecore_fd_cb: post semaphore");
    ret = sem_post(&recv_sem);
    if (-1 == ret) {
        _ERR("_ecore_fd_cb: sem_post failed(%d:%s)", errno, strerror(errno));
    }
    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _add_poll_fd
 *          Add file descriptor for poll.
 *
 * @param[in]   fd                      file descriptor
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_add_poll_fd(int fd)
{
    Ecore_Fd_Handler_Flags flags = 0;
    flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR);

    ecore_fd_handler = ecore_main_fd_handler_add(fd, flags, _ecore_fd_cb,
                                                 NULL, NULL, NULL);
    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _del_poll_fd
 *          Delete file descriptor for poll.
 *
 * @param[in]   fd                      file descriptor
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
_del_poll_fd(int fd)
{
    ecore_main_fd_handler_del(ecore_fd_handler);
    ecore_fd_handler = NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _poll_fd_thread
 *          Polling file descriptor thread.
 *
 * @param[in]   args                    file descriptor
 * @return      NULL
 */
/*--------------------------------------------------------------------------*/
static void *
_poll_fd_thread(void *args)
{
    int ret = 0;
    while (1) {
        _DBG("_poll_fd_thread: wait semaphore");
        ret = sem_wait(&recv_sem);
        if (-1 == ret) {
            _ERR("_poll_fd_thread: sem_wait failed(%d:%s)",
                 errno, strerror(errno));
            break;
        }
        /* recive message */
        ico_uws_service(uws_context);
        ecore_main_loop_thread_safe_call_async(_exec_callback, syc_user_data);
    }
    pthread_exit(0);

    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _connect_client
 *          Connect to System Controller.
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
static int
_connect_client(ico_syc_callback_t callback, void *user_data)
{
    int ret;

    /* connect to System Controller */
    uws_context = ico_uws_create_context(ICO_SYC_URI, ICO_SYC_PROTOCOL);
    if (uws_context == NULL) {
        _ERR("ico_uws_create_context failed");
        return ICO_SYC_ERR_CONNECT;
    }

    /* set callback */
    ret = ico_uws_set_event_cb(uws_context, _syc_ev_callback, NULL);
    if (ret != ICO_UWS_ERR_NONE) {
        _ERR("ico_uws_set_event_cb failed");
        return ICO_SYC_ERR_UNKNOWN;
    }

    /* set callback and user data */
    syc_callback = callback;
    syc_user_data = user_data;

    return ICO_SYC_ERR_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   _disconnect_client
 *          Disconnect from System Controller.
 *
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void 
_disconnect_client(void)
{
    /* unset callback */
    ico_uws_unset_event_cb(uws_context);
    syc_callback = NULL;

    /* close */
    ico_uws_close(uws_context);

    return;
}

/*============================================================================*/
/* internal common function                                                   */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_context
 *          Get the ico_uws context connected to System Controller.
 *
 * @return      uws_context
 * @retval      context                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
struct ico_uws_context *
ico_syc_get_context(void)
{
    return uws_context;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_id
 *          Get the ico_uws id to send message to System Controller.
 *
 * @return      uws_id
 * @retval      id                      success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
void *
ico_syc_get_id(void)
{
    return uws_id;
}

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_appid
 *          Get the application id.
 *
 * @return      appid
 * @retval      appid                   success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
char *
ico_syc_get_appid(void)
{
    return syc_appid;
}

/*============================================================================*/
/* public interface function                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_connect
 *          Connect to System Controller.
 *          and set the listener to notify the event of System Controller.
 *          If user does not want to set callback function,
 *          set NULL to argument "callback".
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
ICO_API int
ico_syc_connect(ico_syc_callback_t callback, void *user_data)
{
    pthread_t thread;
    int pid, ret;

    /* get appid from pid */
    memset(syc_appid, 0, sizeof(syc_appid));
    pid = getpid();
    ret = aul_app_get_appid_bypid(pid, syc_appid, sizeof(syc_appid));
    if (ret != AUL_R_OK) {
        _ERR("app_get_appid_bypid failed. pid=%d", pid);
        memset(syc_appid, 0, sizeof(syc_appid));
    }
    else {
        _INFO("get appid by pid (pid=%d, appid=%s)", pid, syc_appid);
    }

    /* initalize semaphore */
    ret = sem_init(&recv_sem, 0, 0);
    if (-1 == ret) {
        _ERR("ico_syc_connect: sem_init failed(%d:%s)",
             errno, strerror(errno));
        return ICO_SYC_ERR_SYSTEM;
    }

    /* initialize ecore lib */
    ecore_init();

    /* pthread start */
    ret = pthread_create(&thread, NULL, _poll_fd_thread, (void *)NULL);
    if (-1 == ret) {
        _ERR("ico_syc_connect: pthread_create failed(%d:%s)",
             errno, strerror(errno));
        return ICO_SYC_ERR_SYSTEM;
    }

    /* init to create message */
    ico_syc_init_msg();
    /* init queue */
    recv_info_q = g_queue_new();
    recv_free_q = g_queue_new();

    /* connect to System Controller */
    ret = _connect_client(callback, user_data);
    if (ret != ICO_SYC_ERR_NONE) {
        return ret;
    }

    return ICO_SYC_ERR_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_disconnect
 *          Disconnect the link from System Controller.
 *          and unset the listener to notify the event of System Controller.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_disconnect(void)
{
    recv_info_t *info;

    /* close the connection */
    if (uws_context != NULL) {
        _disconnect_client();
    }

    /* terminate */
    ico_syc_term_msg();

    /* free queue */
    while (g_queue_is_empty(recv_info_q) != TRUE) {
        /* free recv_info_t */
        info = g_queue_pop_head(recv_info_q);
        free(info);
    }
    g_queue_free(recv_info_q);

    while (g_queue_is_empty(recv_free_q) != TRUE) {
        /* free recv_info_t */
        info = g_queue_pop_head(recv_free_q);
        free(info);
    }
    g_queue_free(recv_free_q);

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_service
 *          Service for communication to System Controller.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_API void
ico_syc_service(void)
{
    /* close the connection */
    if (uws_context != NULL) {
        ico_uws_service(uws_context);
    }
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
 */
/*--------------------------------------------------------------------------*/
static void
ico_syc_cb_notify_changed_state(ico_syc_callback_t callback, void *user_data,
                                int event, const void *data, size_t len)
{
    JsonParser *parser  = NULL;
    GError *error       = NULL;
    gboolean gbool      = FALSE;
    JsonNode *root      = NULL;
    JsonObject *obj     = NULL;
    JsonObject *argobj  = NULL;

    ico_syc_state_info_t state_info;

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
        _ERR("invalid message" );
        return;
    }
    /* get object from obj */
    argobj = json_object_get_object_member(obj, MSG_PRMKEY_ARG);

    state_info.id    = ico_syc_get_int_member(argobj, MSG_PRMKEY_STATEID);
    state_info.state = ico_syc_get_int_member(argobj, MSG_PRMKEY_STATE);

    /* exec callback */
    callback(event, &state_info, user_data);

    /* free memory */
    g_object_unref(parser);

    return;
}
/* vim: set expandtab ts=4 sw=4: */
