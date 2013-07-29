/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   library to communicate with outer process
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <glib.h>
#include <stdbool.h>
#include <sys/epoll.h>

#include <getopt.h>

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Edje.h>

#include <ico_uws.h>

#include "ico_uxf.h"
#include "ico_uxf_conf_common.h"

#include "home_screen_parser.h"
#include "home_screen.h"
#include "home_screen_lib.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define WEBSOCKET_MAX_BUFFER    (4096)
typedef struct _hs_lib_poll hs_lib_poll_t;
typedef struct _hs_lib_handle hs_lib_handle_t;
typedef struct _hs_lib_msg hs_lib_msg_t;

/* management connection */
struct _hs_lib_handle {
    hs_lib_handle_t *next; /* next handle                          */
    int fd;                /* Socket file descriptor               */
    int service_on;
    int type;
    struct ico_uws_context *uws_context;
    /* Context of libwebsockets             */
    void *id;              /* id                                   */
    hs_lib_poll_t *poll;
    char appid[ICO_UXF_MAX_PROCESS_NAME];
};

struct _hs_lib_poll {
    hs_lib_poll_t *next;
    int fd;
    int flags;
    void *user_data;
    hs_lib_handle_t *handle;
};

struct _hs_lib_msg {
    hs_lib_msg_t *next;
    char *data;
    int len;
    int type;
    hs_lib_handle_t *handle;
};

/*============================================================================*/
/* variabe                                                                    */
/*============================================================================*/
static struct ico_uws_context *hs_wsicontext = NULL;
static hs_lib_msg_t *hs_send_msg = NULL;
static hs_lib_msg_t *hs_recv_msg = NULL;
static hs_lib_msg_t *hs_free_msg = NULL;
static hs_lib_handle_t *hs_handles = NULL;
static hs_lib_handle_t *hs_handles_free = NULL;
static hs_lib_poll_t *hs_polls = NULL;
static hs_lib_poll_t *hs_polls_free = NULL;

/*============================================================================*/
/* Function prototype for static(internal) functions                          */
/*============================================================================*/
static hs_lib_handle_t *hs_lib_setup_server(const char *uri, 
        const char *protocol, ico_uws_evt_cb callback);
static void hs_lib_callback_uws(const struct ico_uws_context *context,
                    const ico_uws_evt_e event, const void *id,
                    const ico_uws_detail *detail, void *data);
static void hs_lib_handle_command(hs_lib_msg_t *msg);
static void hs_lib_handle_application(hs_lib_msg_t *msg);
static void hs_lib_handle_onscreen(hs_lib_msg_t *msg);
static hs_lib_msg_t *hs_lib_alloc_msg(char *data, int len);
static void hs_lib_free_msg(hs_lib_msg_t *msg);
static void hs_lib_free_handle(hs_lib_handle_t *handle);
static hs_lib_handle_t *hs_lib_alloc_handle(void);
static void hs_lib_realsend(hs_lib_msg_t *msg);
static hs_lib_msg_t *hs_lib_get_sendmsg(int type);
static int hs_lib_put_sendmsg(hs_lib_msg_t *send);
static hs_lib_msg_t *hs_lib_get_recvmsg(void);
static void hs_lib_put_recvmsg(hs_lib_msg_t *recv);
static void hs_lib_com_dispatch(hs_lib_handle_t *_handle, int timeoutms);
static void hs_lib_poll_fd_event(hs_lib_poll_t *poll, int flags);
static Eina_Bool hs_lib_ecore_fdevent(void *data, Ecore_Fd_Handler *handler);
static void hs_lib_control_fd(hs_lib_poll_t *fd_ctl[], const int num_fds);
static int hs_lib_add_poll_fd(int fd, int flags);
static void hs_lib_del_poll_fd(int fd);
static void hs_lib_set_mode_poll_fd(int fd, int flags);
static void hs_lib_clear_mode_poll_fd(int fd, int flags);
static char *get_parsed_str(char *in, char *out, int len, int arg_idx);
static char *getFileName(char *filepath, int len);

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_handle_command
 *          callback at received message from external command tools,
 *          and handle the message.
 *
 * @param[in]   msg                 received message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_handle_command(hs_lib_msg_t *msg)
{
    int cnt = 0;
    int ret;
    char buff[ICO_HS_MSG_SIZE];
    hs_lib_msg_t *send;

    uifw_trace("hs_lib_handle_command: Enter");

    if (strcmp(msg->data, HS_REQ_ANS_HELLO) == 0) {
        memset(buff, 0, sizeof(buff));

        send = hs_lib_alloc_msg("ANS HELLO", strlen("ANS HELLO"));
        if (!send) {
            uifw_warn("hs_lib_handle_command: ERROR(allocate send msg)");
            return;
        }
        send->type = ICO_HS_PROTOCOL_TYPE_CM;

        hs_lib_put_sendmsg(send);

        uifw_trace("hs_lib_handle_command: Leave");

        return;
    }

    /* data sent by command is parsed and controlled for the window */
    ret = hs_parse_form(msg->data, msg->len);
    if (hs_req_touch == ICO_HS_TOUCH_TOBE_HIDE) {
        hs_show_appscreen(NULL);
        hs_control_app_screen();
    }
    else if (hs_req_touch == ICO_HS_TOUCH_TOBE_SHOW) {
        hs_get_tilescreendata();
        hs_dump_app();
        hs_req_ctlr_tile();
    }

    /* return message */
    memset(buff, 0, sizeof(buff));
    if (ret == 0) {
        cnt = snprintf(buff, sizeof(buff), "ANS %s OK", msg->data);
    }
    else {
        cnt = snprintf(buff, sizeof(buff), "ERR %s FAILED", msg->data);
    }

    if (cnt > 0) {
        send = hs_lib_alloc_msg(buff, cnt);
        if (!send) {
            uifw_warn("hs_lib_handle_command: ERROR(allocate send msg)");
            return;
        }
        send->type = ICO_HS_PROTOCOL_TYPE_CM;

        hs_lib_put_sendmsg(send);
    }

    uifw_trace("hs_lib_handle_command: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_handle_application
 *          callback at received message from a application, and handle the
 *          message.
 *
 * @param[in]   msg                 received message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_handle_application(hs_lib_msg_t *msg)
{
    char *app_id;
    char msg_data[ICO_HS_MSG_SIZE];
    char tmp_buf[ICO_HS_MSG_SIZE];
    char *data = msg->data;
    hs_lib_msg_t *send;

    uifw_trace("hs_lib_handle_application: Enter");

    if (strncmp("OPEN", data, 4) == 0) {
        strncpy(msg_data, get_parsed_str(data, tmp_buf, sizeof(tmp_buf), 2),
                sizeof(msg_data));
        app_id = msg_data;
        uifw_trace("hs_lib_callback_app: app_id = %s, msg_data - %s, in - %s",
                   app_id, msg_data, data);
        strncpy(hs_active_onscreen, app_id, sizeof(hs_active_onscreen));
        strncpy(msg->handle->appid, app_id, sizeof(hs_active_onscreen));

        send = hs_lib_alloc_msg(data, msg->len);
        if (!send) {
            uifw_warn("hs_lib_callback_app: ERROR(allocate send msg)");
            return;
        }
        send->type = ICO_HS_PROTOCOL_TYPE_OS;

        hs_lib_put_sendmsg(send);
    }
    else if (strncmp("CLOSE", data, 5) == 0) {
        /* native apps only */
        hs_hide_onscreen();
        memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
    }
    uifw_trace("hs_lib_handle_application: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_handle_onscreen
 *          callback at received message from onscreen, and handle the
 *          message.
 *
 * @param[in]   msg                 received message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_handle_onscreen(hs_lib_msg_t *msg)
{
    char msg_buf[ICO_HS_MSG_SIZE];
    char msg_data[ICO_HS_MSG_SIZE];
    char tmp_buf[ICO_HS_MSG_SIZE];
    char *p_msg_data;
    char *ptr;
    int ret, idx;
    const Ico_Uxf_conf_application *appConf = NULL;
    char *data = msg->data;
    hs_lib_msg_t *send;

    uifw_trace("hs_lib_handle_onscreen: Enter");

    /* init buffers */
    memset(msg_data, 0x00, sizeof(msg_data));
    p_msg_data = &msg_data[0];
    strncpy(p_msg_data, data, msg->len);

    uifw_trace("hs_lib_handle_onscreen: hs_active_onscreen = %s",
               hs_active_onscreen);
    /* parse message from OnScreen */
    if (strncmp("RESULT", p_msg_data, 6) == 0) {
        /* get 2nd phrase */
        ptr = get_parsed_str(p_msg_data, tmp_buf, sizeof(tmp_buf), 1);
        if (strncmp("SUCCESS", ptr, 7) == 0) {
            /* loaded edje: success */
            hs_show_onscreen();
            send = hs_lib_alloc_msg(data, msg->len);
            if (!send) {
                uifw_warn("hs_lib_handle_onscreen: ERROR(allocate send msg)");
                return;
            }
            send->type = ICO_HS_PROTOCOL_TYPE_APP;
            hs_lib_put_sendmsg(send);
        }
        else if (strncmp("FAILED", ptr, 6) == 0) {
            /* not loaded edje: failed */
            if (strcmp(hs_active_onscreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN)
                    == 0) {
                /* appli_list or appli_kill: raise touch layer */
                hs_hide_onscreen();
            }
            else {
                /* native apps: raise own */
                hs_hide_onscreen();
            }
            memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
        }
    }
    else if (strncmp("TOUCH", data, 5) == 0) {
        if (strlen(hs_active_onscreen) == 0) {
            /* do nothing */
        }
        else if (strcmp(hs_active_onscreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN)
                 == 0)  {
            /* get 2nd phrase */
            ptr = get_parsed_str(p_msg_data, tmp_buf, sizeof(tmp_buf), 1);
            if (strncmp(getFileName(ptr, strlen(ptr)),
                        ICO_HS_ONS_APPLI_LIST_NAME,
                        sizeof(ICO_HS_ONS_APPLI_LIST_NAME)) == 0) {
                /* get 3rd phrase */
                ptr = get_parsed_str(p_msg_data, tmp_buf, sizeof(tmp_buf), 2);
                appConf = ico_uxf_getAppByAppid(ptr);
                uifw_trace("hs_lib_handle_onscreen: hs_tile_get_index_app = %08x(%s)",
                           (int)appConf, ptr);
                if (appConf != NULL) {
                    sprintf(msg_buf, "FOCUS %s", ptr);
                    send = hs_lib_alloc_msg(msg_buf, strlen(msg_buf));
                    if (!send) {
                        uifw_warn("hs_lib_handle_onscreen: ERROR(allocate send msg)");
                        return;
                    }
                    send->type = ICO_HS_PROTOCOL_TYPE_CM;
                    ret = hs_lib_put_sendmsg(send);
                    if (ret < 0) {
                        ret = ico_uxf_process_execute(ptr);
                        hs_hide_onscreen();
                        if ((ret == ICO_UXF_EOK) || (ret == ICO_UXF_EBUSY)) {
                            idx = hs_tile_get_index_app(ptr);
                            if (idx < 0) {
                                idx = hs_tile_get_minchange();
                            }
                            hs_tile_set_app(idx, ptr);
                        }
                        hs_tile_show_screen();
                    }
                    else {
                        usleep(100000);
                        hs_hide_onscreen();
                    }
                    memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
                }
                else {
                    if (strncmp(ptr, "next_bt", 7) == 0) {
                        hs_get_ons_edj_path(tmp_buf, sizeof(tmp_buf));
                        sprintf(msg_buf, "OPEN %s%s %s", tmp_buf,
                                ICO_HS_ONS_APPLI_LIST_NAME,
                                ICO_UXF_PROC_DEFAULT_HOMESCREEN);
                        uifw_trace("send '%s' to onscreen", msg_buf);
                        send = hs_lib_alloc_msg(msg_buf, strlen(msg_buf));
                        if (!send) {
                            uifw_warn(
                                      "hs_lib_handle_onscreen: ERROR(allocate send msg)");
                            return;
                        }
                        send->type = ICO_HS_PROTOCOL_TYPE_OS;
                        (void)hs_lib_put_sendmsg(send);
                    }
                    else if (strncmp(ptr, "cancel_bt", 8) == 0) {
                        hs_hide_onscreen();
                        memset(hs_active_onscreen, 0,
                               sizeof(hs_active_onscreen));
                    }
                    else {
                        hs_hide_onscreen();
                        memset(hs_active_onscreen, 0,
                               sizeof(hs_active_onscreen));
                    }
                }
            }
            else if (strncmp(getFileName(ptr, strlen(ptr)),
                             ICO_HS_ONS_APPLI_KILL_NAME,
                             sizeof(ICO_HS_ONS_APPLI_KILL_NAME)) == 0) {
                /* get 3rd phrase */
                ptr = get_parsed_str(p_msg_data, tmp_buf, sizeof(tmp_buf), 2);
                if (strncmp(ptr, "ok_bt", 5) == 0) {
                    uifw_trace("%s: ok_bt", ICO_HS_ONS_APPLI_KILL_NAME);
                    if (strlen(hs_kill_appid) == 0) {
                        hs_hide_onscreen();
                        memset(hs_active_onscreen, 0,
                               sizeof(hs_active_onscreen));
                        return;
                    }
                    sprintf((char *)msg_buf, "FOCUS %s", hs_kill_appid);
                    uifw_trace("send '%s' to command", msg_buf);
                    send = hs_lib_alloc_msg(msg_buf, strlen(msg_buf));
                    if (!send) {
                        uifw_warn(
                                  "hs_lib_handle_onscreen: ERROR(allocate send msg)");
                        return;
                    }
                    send->type = ICO_HS_PROTOCOL_TYPE_CM;
                    ret = hs_lib_put_sendmsg(send);
                    if (ret < 0) {
                        hs_hide_onscreen();
                        hs_tile_kill_app(hs_kill_appid);
                        hs_tile_show_screen();
                    }
                    else {
                        hs_hide_onscreen();
                    }
                }
                else if (strncmp(ptr, "cancel_bt", 8) == 0) {
                    uifw_trace("%s: cancel_bt", ICO_HS_ONS_APPLI_KILL_NAME);
                    hs_hide_onscreen();
                    hs_tile_show_screen();
                }
                else {
                    uifw_trace("%s: else %s", ICO_HS_ONS_APPLI_KILL_NAME, ptr);
                    hs_hide_onscreen();
                }
                memset(hs_active_onscreen, 0, sizeof(hs_active_onscreen));
                memset(hs_kill_appid, 0, sizeof(hs_kill_appid));
            }
        }
        else {
            uifw_trace("send %s to app", data);
            send = hs_lib_alloc_msg(data, msg->len);
            if (!send) {
                uifw_warn("hs_lib_handle_onscreen: ERROR(allocate send msg)");
                return;
            }
            send->type = ICO_HS_PROTOCOL_TYPE_APP;
            hs_lib_put_sendmsg(send);
        }
    }

    uifw_trace("hs_lib_handle_onscreen: Leave");
}
/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_handle_statusbar
 *          callback at received message from statusbar, and handle the
 *          message.
 *
 * @param[in]   msg                 received message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_handle_statusbar(hs_lib_msg_t *msg)
{
    hs_lib_msg_t *send;
    char tmp_buf[ICO_HS_MSG_SIZE];
    char *cmd;
    Ico_Uxf_ProcessWin wins[1];
    Ico_Uxf_ProcessAttr attr;
    int ret;
    int idx;
    int cnt = 0;

    uifw_trace("hs_lib_handle_statusbar: Enter(%s)", msg->data);

    if (strncmp("CLICK ESCUTCHEON", msg->data, 16) == 0) {
        /* clicked escutcheon button and send a command to outer commander */
        hs_click_escutcheon();
        /* send "Receive OK" message to statusbar */
        send = hs_lib_alloc_msg("RECEIVE OK", strlen("RECEIVE OK"));
        if (send) {
            send->type = ICO_HS_PROTOCOL_TYPE_SB;
            hs_lib_put_sendmsg(send);
        }
    }
    else if (strncmp("OPEN", msg->data, 4) == 0) {
        /* forward the message to onscreen */
        if (hs_click_applist()) {
            send = hs_lib_alloc_msg(msg->data, msg->len);
            if (send) {
                strncpy(hs_active_onscreen, ICO_UXF_PROC_DEFAULT_HOMESCREEN,
                        sizeof(hs_active_onscreen));
                send->type = ICO_HS_PROTOCOL_TYPE_OS;
                hs_lib_put_sendmsg(send);
            }
        }
        /* send "Receive OK" message to statusbar */
        send = hs_lib_alloc_msg("RECEIVE OK", strlen("RECEIVE OK"));
        if (send) {
            send->type = ICO_HS_PROTOCOL_TYPE_SB;
            hs_lib_put_sendmsg(send);
        }
    }
    else if (strncmp("SHOW", msg->data, 4) == 0) {
        /* show the application on the application screen */
        cmd = get_parsed_str(msg->data, tmp_buf, sizeof(tmp_buf), 1);
        if (cmd) {
            /* wait for creating the application's window */
            memset(wins, 0, sizeof(wins));
            while (wins[0].window <= 0) {
                ret = ico_uxf_process_window_get(cmd, wins, 1);
                if (ret > 0) {
                    ret = ico_uxf_process_attribute_get(cmd, &attr);
                    if (wins[0].window > 0) {
                        break;
                    }
                    else if ((ret >= 0) && (attr.status != ICO_UXF_PROCSTATUS_RUN)) {
                        /* launch the application */
                        ret = ico_uxf_process_execute(cmd);
                    }
                }
                else {
                    /* unknown application */
                    break;
                }
                usleep(10000);
                ecore_main_loop_iterate();

                if (cnt > 100) {
                    break;
                }
                else {
                    cnt++;
                }
            }
            ret = ico_uxf_process_attribute_get(cmd, &attr);
            if ((ret >= 0) && (attr.status == ICO_UXF_PROCSTATUS_RUN)) {
                idx = hs_tile_get_index_app(cmd);
                if (idx < 0) {
                    idx = hs_tile_get_minchange();
                }
                hs_tile_set_app(idx, cmd);
            }

            /* show the application screen*/
            hs_show_appscreen(cmd);
        }
        /* send "Receive OK" message to statusbar */
        send = hs_lib_alloc_msg("RECEIVE OK", strlen("RECEIVE OK"));
        if (send) {
            send->type = ICO_HS_PROTOCOL_TYPE_SB;
            hs_lib_put_sendmsg(send);
        }
    }
    uifw_trace("hs_lib_handle_statusbar: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_alloc_msg
 *          Allocate a msg structure
 *
 * @param[in]   data                data
 * @param[in]   len                 data length
 * @return      address
 * @retval      > 0                 success
 * @retval      NULL                error
 */
/*--------------------------------------------------------------------------*/
static hs_lib_msg_t *
hs_lib_alloc_msg(char *data, int len)
{
    hs_lib_msg_t *msg;

    if (!hs_free_msg) {
        msg = malloc(sizeof(hs_lib_msg_t));
        if (!msg) {
            return NULL;
        }
    }
    else {
        msg = hs_free_msg;
        hs_free_msg = hs_free_msg->next;
    }
    memset(msg, 0, sizeof(hs_lib_msg_t));

    msg->len = len;
    msg->data = strdup(data);
    if (!msg->data) {
        free(msg);
        return NULL;
    }

    return msg;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_free_msg
 *          free the msg
 *
 * @param[in]   data                data
 * @param[in]   free                data lenght
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_free_msg(hs_lib_msg_t *msg)
{
    if (!msg) {
        return;
    }

    if (msg->data) {
        free(msg->data);
    }

    memset(msg, 0, sizeof(hs_lib_msg_t));

    msg->next = hs_free_msg;
    hs_free_msg = msg;

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_free_handle
 *          free the indecated handle.
 *
 * @param       handle              handle to free
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_free_handle(hs_lib_handle_t *handle)
{
    hs_lib_handle_t *free;
    hs_lib_handle_t *bfree;

    free = hs_handles;
    bfree = NULL;
    while (free) {
        if (free == handle)
            break;
        bfree = free;
        free = free->next;
    }
    if (free) {
        if (bfree) {
            bfree->next = free->next;
        }
        else {
            hs_handles = free->next;
        }
        free->next = hs_handles_free;
        hs_handles_free = free;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_alloc_handle
 *          Allocate handle.
 *
 * @param       none
 * @return      address
 * @retval      > 0                 success
 * @retval      NULL                error
 */
/*--------------------------------------------------------------------------*/
static hs_lib_handle_t *
hs_lib_alloc_handle(void)
{
    hs_lib_handle_t *handle;

    if (!hs_handles_free) {
        handle = malloc(sizeof(hs_lib_handle_t));
        if (!handle) {
            return NULL;
        }
    }
    else {
        handle = hs_handles_free;
        hs_handles_free = hs_handles_free->next;
    }
    memset(handle, 0, sizeof(hs_lib_handle_t));

    if (hs_handles) {
        handle->next = hs_handles;
    }
    hs_handles = handle;

    return handle;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_realsend
 *          write send message to websocket actually.
 *
 * @param[in]   msg                 write message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_realsend(hs_lib_msg_t *msg)
{
    uifw_trace("hs_lib_realsend: send(ctx=%x(%d), type=h:%d(m:%d), len=%d, msg=%s)",
               msg->handle->uws_context, msg->handle->id, 
               msg->handle->type, msg->type, msg->len, msg->data);
    ico_uws_send(msg->handle->uws_context, msg->handle->id,
                 (unsigned char *)msg->data, msg->len);

    hs_lib_free_msg(msg);

    uifw_trace("hs_lib_realsend: Leave");

#if 0               /* no need on websockets 1.2    */
    usleep(200);
#endif
    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_get_sendmsg
 *          get the send message from the send queue.
 *
 * @param[in]   type                receiver
 * @return      send message address
 * @retval      > 0                 success
 * @retval      NULL                error
 */
/*--------------------------------------------------------------------------*/
static hs_lib_msg_t *
hs_lib_get_sendmsg(int type)
{
    hs_lib_msg_t *msg;
    hs_lib_msg_t *bmsg;

    msg = hs_send_msg;
    bmsg = NULL;
    while (msg) {
        if (msg->type == type) {
            break;
        }
        bmsg = msg;
        msg = msg->next;
    }
    if (msg) {
        if (!bmsg) {
            /* top */
            hs_send_msg = msg->next;
            msg->next = NULL;
        }
        else {
            bmsg->next = msg->next;
            msg->next = NULL;
        }
    }

    return msg;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_put_sendmsg
 *          put the send message to the send queue end.
 *
 * @param[in]   data                send message
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
static int
hs_lib_put_sendmsg(hs_lib_msg_t *send)
{
    hs_lib_msg_t *msg;
    hs_lib_handle_t *handle;

    uifw_trace("hs_lib_put_sendmsg: Enter(%d:%s)", send->type, send->data);

    handle = hs_handles;
    while (handle) {
        if (handle->type == send->type) {
            break;
        }
        handle = handle->next;
    }
    if (!handle) {
        uifw_trace("hs_lib_put_sendmsg: Leave(target(type:%d) does not exist", send->type);
        hs_lib_free_msg(send);
        return ICO_HS_ERR;
    }

    msg = hs_send_msg;
    while (msg) {
        if (!msg->next) {
            break;
        }
        msg = msg->next;
    }
    if (!msg) {
        hs_send_msg = send;
    }
    else {
        msg->next = send;
    }

    if (handle) {
        send->handle = handle;
        uifw_trace("hs_lib_put_sendmsg: hs_lib_set_mode_poll_fd"
            "(uws_ctx=%08x, id=%08x", handle->uws_context, handle->id);
        hs_lib_set_mode_poll_fd(handle->fd, EPOLLOUT);

        hs_lib_com_dispatch(handle, 0);
    }

    uifw_trace("hs_lib_put_sendmsg: Leave");

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_get_recvmsg
 *          get the receive message from the receive queue end.
 *
 * @param       none
 * @return      recv message address
 * @retval      > 0                 success
 * @retval      NULL                error
 */
/*--------------------------------------------------------------------------*/
static hs_lib_msg_t *
hs_lib_get_recvmsg(void)
{
    hs_lib_msg_t *msg;

    msg = hs_recv_msg;
    if (msg) {
        hs_recv_msg = msg->next;
        msg->next = NULL;
    }

    return msg;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_put_recvmsg
 *          put the receive message to the receive queue end.
 *
 * @param[in]   data                receive message
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_put_recvmsg(hs_lib_msg_t *recv)
{
    hs_lib_msg_t *msg;

    msg = hs_recv_msg;
    while (msg) {
        if (!msg->next) {
            break;
        }
        msg = msg->next;
    }
    if (!msg) {
        hs_recv_msg = recv;
    }
    else {
        msg->next = recv;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_com_dispatch
 *          The accumulated data that transmitted or received is procecssed
 *
 * @param[in]   _handle             connect handle,
 *                                  if NULL target is all connection
 * @param[in]   timeoutms           maximum wait time on miri-sec.
 *                                  0 is no wait, -1 is wait forever.
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_com_dispatch(hs_lib_handle_t *_handle, int timeoutms)
{
    hs_lib_handle_t *handle;
    hs_lib_msg_t *msg;

    if (_handle) {
        handle = _handle;
    }
    else {
        handle = hs_handles;
    }

    while (handle) {
        ico_uws_service(handle->uws_context);

        /* treate received buffer */
        msg = hs_lib_get_recvmsg();
        while (msg) {
            switch (msg->type) {
            case ICO_HS_PROTOCOL_TYPE_CM:
                hs_lib_handle_command(msg);
                break;

            case ICO_HS_PROTOCOL_TYPE_SB:
                hs_lib_handle_statusbar(msg);
                break;

            case ICO_HS_PROTOCOL_TYPE_OS:
                hs_lib_handle_onscreen(msg);
                break;

            case ICO_HS_PROTOCOL_TYPE_APP:
                hs_lib_handle_application(msg);
                break;

            default:
                break;
            }
            hs_lib_free_msg(msg);
            msg = hs_lib_get_recvmsg();
        }

        if (_handle)
            break;
        handle = handle->next;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_poll_fd_event
 *          Notify the changes of file descriptor's state
 *
 * @param[in]   fd_ctl              The structure of file descriptor's controller
 * @param[in]   flags               Flags
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_poll_fd_event(hs_lib_poll_t *poll, int flags)
{
    hs_lib_handle_t *handle = poll->handle;
    hs_lib_msg_t *msg;

    uifw_trace("hs_lib_poll_fd_event: Enter(handle=%08x, poll=%08x flags=%08x)", 
                (int)handle, (int)poll, flags);

    /* try to ws service */
    hs_lib_com_dispatch(handle, 0);

    /* control polling fd's event? */
    if ((flags & EPOLLOUT) && (handle != NULL)) {
        hs_lib_realsend(hs_lib_get_sendmsg(handle->type));

        hs_lib_com_dispatch(handle, 0);

        hs_lib_clear_mode_poll_fd(handle->fd, EPOLLOUT);
        /* check send queue */
        msg = hs_send_msg;
        while (msg) {
            if (msg->handle->fd == handle->fd) {
                hs_lib_set_mode_poll_fd(handle->fd, EPOLLOUT);
                break;
            }
            msg = msg->next;
        }
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_ecore_fdevent
 *          callback function called by Ecore when the websocket's file
 *          descriptor had change
 *
 * @param[in]   data                user data(ico_apf_com_poll_t)
 * @param[in]   handler             Ecore file descriptor handler
 * @return      call back setting
 * @retval      ECORE_CALLBACK_RENEW    set callback
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
hs_lib_ecore_fdevent(void *data, Ecore_Fd_Handler *handler)
{
    int flags;

    uifw_trace("hs_lib_ecore_fdevent: Enter");

    flags = 
        (ecore_main_fd_handler_active_get(handler, ECORE_FD_READ)) ? EPOLLIN : 0;
    if (ecore_main_fd_handler_active_get(handler, ECORE_FD_WRITE)) {
        flags |= EPOLLOUT;
    }
    if (ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR)) {
        flags |= EPOLLERR;
    }

    hs_lib_poll_fd_event((hs_lib_poll_t *)data, flags);

    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_control_fd
 *          control file descriptors to add or delete ecore event handler
 *
 * @param[in]   fd_ctl              file descriptors
 * @param[in]   num_fds             number of file descriptors
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_control_fd(hs_lib_poll_t *fd_ctl[], const int num_fds)
{
    int ii;
    Ecore_Fd_Handler_Flags flags;

    for (ii = 0; ii < num_fds; ii++) {
        uifw_trace("hs_lib_control_fd: (handle=%08x, fd=%d, flg=%08x)",
                          (int)fd_ctl[ii]->handle, fd_ctl[ii]->fd, fd_ctl[ii]->flags);
        if (fd_ctl[ii]->flags) {
            flags = (fd_ctl[ii]->flags & EPOLLIN) ? ECORE_FD_READ : 0;
            if (fd_ctl[ii]->flags & EPOLLOUT) {
                flags |= ECORE_FD_WRITE;
            }
            if (fd_ctl[ii]->flags & (EPOLLERR | EPOLLPRI)) {
                flags |= ECORE_FD_ERROR;
            }
            if (!fd_ctl[ii]->user_data) {
                uifw_trace(
                           "hs_lib_control_fd: ADD(fd=%d, flg=%08x, eflg=%08x)",
                           fd_ctl[ii]->fd, fd_ctl[ii]->flags, flags);
                fd_ctl[ii]->user_data
                        = (void *)ecore_main_fd_handler_add(
                                                            fd_ctl[ii]->fd,
                                                            flags,
                                                            hs_lib_ecore_fdevent,
                                                            (void *)fd_ctl[ii],
                                                            NULL, NULL);
            }
            else {
                uifw_trace(
                           "hs_lib_control_fd: CHANGE(fd=%d, flg=%08x, eflg=%08x)",
                           fd_ctl[ii]->fd, fd_ctl[ii]->flags, flags);
                ecore_main_fd_handler_active_set(
                                                 (Ecore_Fd_Handler *)fd_ctl[ii]->user_data,
                                                 flags);
            }
        }
        else {
            /* remove file descriptor */
            uifw_trace("hs_lib_control_fd: DELL(fd=%d)", fd_ctl[ii]->fd);
            ecore_main_fd_handler_del((Ecore_Fd_Handler *)fd_ctl[ii]->user_data);
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_add_poll_fd
 *          add file descriptor of websocket for poll
 *
 * @param[in]   fd                  file descriptor
 * @param[in]   flags               event flag
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
static int
hs_lib_add_poll_fd(int fd, int flags)
{
    hs_lib_poll_t *poll;
    hs_lib_poll_t *fds[1];
    hs_lib_handle_t *handle;

    /* get management table */
    poll = hs_polls;
    while (poll) {
        if (poll->fd == fd) {
            break;
        }
        poll = poll->next;
    }
    if (!poll) {
        if (hs_polls_free) {
            poll = hs_polls_free;
            hs_polls_free = poll->next;
            poll->user_data = NULL;
        }
        else {
            poll = calloc(sizeof(hs_lib_poll_t), 1);
            if (!poll) {
                uifw_warn("hs_lib_add_poll_fd: ERR(allocate poll table failed)");
                return ICO_HS_ERR;
            }
        }
        poll->fd = fd;
        poll->next = hs_polls;
        hs_polls = poll;
    }
    poll->flags = flags;

    /* if the fd is same as uws_context, set the handle */
    handle = hs_handles;
    while (handle) {
        if (handle->fd == fd)
            break;
        handle = handle->next;
    }
    if (handle) {
        poll->handle = handle;
        handle->poll = poll;
    }
    else {
        poll->handle = NULL;
    }

    /* control fds */
    fds[0] = poll;
    hs_lib_control_fd(fds, 1);

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_del_poll_fd
 *          delete file descriptor of websocket for poll
 *
 * @param[in]   fd                  file descriptor
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_del_poll_fd(int fd)
{
    hs_lib_poll_t *poll;
    hs_lib_poll_t *fds[1];

    /* get management table */
    poll = hs_polls;
    while (poll) {
        if (poll->fd == fd) {
            hs_polls = poll->next;

            /* control fds */
            poll->flags = 0;
            fds[0] = poll;
            hs_lib_control_fd(fds, 1);

            /* add free polls */
            poll->next = hs_polls_free;
            hs_polls_free = poll;
            break;
        }
        poll = poll->next;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_set_mode_poll_fd
 *          set event to file descriptor of websocket for poll
 *
 * @param[in]   fd                  file descriptor
 * @param[in]   flags               event flag
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_set_mode_poll_fd(int fd, int flags)
{
    hs_lib_poll_t *poll;
    hs_lib_poll_t *fds[1];

    /* get management table */
    poll = hs_polls;
    while (poll) {
        if (poll->fd == fd) {
uifw_trace("hs_lib_set_mode_poll_fd: fd=%d (%d)", poll->fd, fd);
            /* control fds */
            poll->flags |= flags;
            fds[0] = poll;
            hs_lib_control_fd(fds, 1);
        }
        poll = poll->next;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_clear_mode_poll_fd
 *          clear event to file descriptor of websocket for poll
 *
 * @param[in]   fd                  file descriptor
 * @param[in]   flags               event flag
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
hs_lib_clear_mode_poll_fd(int fd, int flags)
{
    hs_lib_poll_t *poll;
    hs_lib_poll_t *fds[1];

    /* get management table */
    poll = hs_polls;
    while (poll) {
        if (poll->fd == fd) {
            /* control fds */
            poll->flags &= ~flags;
            fds[0] = poll;
            hs_lib_control_fd(fds, 1);
        }
        poll = poll->next;
    }

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   get_parsed_str
 *          extract word by the given arg_idx.
 *
 * @param[in]   in                  source char array
 * @param[in]   out                 char array store word
 * @param[in]   len                 char array length
 * @param[in]   arg_idx             index
 * @return      none
 * @retval      >=0                 pointer to extract word
 */
/*--------------------------------------------------------------------------*/
static char *
get_parsed_str(char *in, char *out, int len, int arg_idx)
{
    int ii;
    char *data;

    uifw_trace("get_parsed_str: %s arg = %d", in, arg_idx);

    memset(out, 0, len);
    strncpy(out, in, len);
    data = strtok(out, " ");
    for (ii = 0; ii < arg_idx; ii++) {
        data = strtok(NULL, " ");
    }

    uifw_trace("get_parsed_str data: %s", data);

    return data;
}

/* get filename from the full path */
static char *
getFileName(char *filepath, int len)
{
    int ii;
    char *name = filepath;

    for (ii = 0; ii < len - 1; ii++) {
        if (filepath[ii] == 0)
            break;
        if (filepath[ii] == '/')
            name = &filepath[ii + 1];
    }

    uifw_trace("getFileName name: %s", name);

    return name;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_event_message
 *          send message
 *
 * @param[in]   type                receiver
 * @param[in]   fromat              message to send
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
int
hs_lib_event_message(int type, char *format, ...)
{
    va_list list;
    char message[ICO_HS_MSG_SIZE];
    hs_lib_msg_t *send;

    memset(message, 0, sizeof(message));

    va_start(list, format);
    vsnprintf(message, sizeof(message), format, list);
    va_end(list);

    uifw_trace("hs_lib_event_message: message to %d, %s", type, message);
    send = hs_lib_alloc_msg(message, strlen(message));
    if (!send) {
        uifw_warn("hs_lib_event_message: ERROR(allocate send msg)");
        return ICO_HS_ERR;
    }
    send->type = type;

    return hs_lib_put_sendmsg(send);
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   hs_lib_callback_uws
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
hs_lib_callback_uws(const struct ico_uws_context *context,
                    const ico_uws_evt_e event,
                    const void *id,
                    const ico_uws_detail *detail,
                    void *data)
{
    char *in;
    int len;
    hs_lib_handle_t *handle;
    hs_lib_msg_t *msg;

    uifw_trace("hs_lib_callback_uws: context=%08x id=%08x", (int)context, (int)id);

    handle = hs_handles;
    while (handle) {
        if (handle->id == id)
            break;
        handle = handle->next;
    }
    if (!handle) {
        handle = hs_lib_alloc_handle();
        if (!handle) {
            uifw_warn("hs_lib_callback_uws: ERROR(allocate handle)");
            return;
        }
        handle->uws_context = (struct ico_uws_context *)context;
        handle->id = (void *)id;
        handle->service_on = 1;
    }

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_OPEN(id=%08x)", (int)id); 
        break;

    case ICO_UWS_EVT_CLOSE:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_CLOSE(id=%08x)", (int)id);
        hs_lib_free_handle(handle);
        break;

    case ICO_UWS_EVT_RECEIVE:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_RECEIVE(id=%08x, msg=%s, len=%d)", 
                   (int)id, (char *)detail->_ico_uws_message.recv_data,
                   detail->_ico_uws_message.recv_len);
        in = (char *)detail->_ico_uws_message.recv_data;
        len = detail->_ico_uws_message.recv_len;
        if (len < 4) {
            break;
        }
        if (strncmp(in, ICO_HS_MSG_HEAD_CM, 3) == 0) {
            handle->type = ICO_HS_PROTOCOL_TYPE_CM;
        }
        else if (strncmp(in, ICO_HS_MSG_HEAD_OS, 3) == 0) {
            handle->type = ICO_HS_PROTOCOL_TYPE_OS;
        }
        else if (strncmp(in, ICO_HS_MSG_HEAD_SB, 3) == 0) {
            handle->type = ICO_HS_PROTOCOL_TYPE_SB;
        }
        else if (strncmp(in, ICO_HS_MSG_HEAD_APP, 3) == 0) {
            handle->type = ICO_HS_PROTOCOL_TYPE_APP;
        }
        else {
            uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_RECEIVE(unknown)"); 
            break;
        }

        msg = hs_lib_alloc_msg(&in[4], len - 4);
        if (!msg) {
            uifw_trace("hs_lib_callback_uws: cannot allocate msg");
            break;
        }
        msg->type = handle->type;
        msg->handle = handle;

        hs_lib_put_recvmsg(msg);

        break;
    
    case ICO_UWS_EVT_ERROR:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_ERROR(id=%08x, err=%d)", 
                   (int)id, detail->_ico_uws_error.code);
        break;

    case ICO_UWS_EVT_ADD_FD:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_ADD_FD(id=%08x, fd=%d)",
                    (int)id, detail->_ico_uws_fd.fd);
        handle->fd = detail->_ico_uws_fd.fd;
        (void)hs_lib_add_poll_fd(detail->_ico_uws_fd.fd, EPOLLIN);
        break;

    case ICO_UWS_EVT_DEL_FD:
        uifw_trace("hs_lib_callback_uws: ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                    (int)id, detail->_ico_uws_fd.fd);
        hs_lib_del_poll_fd(detail->_ico_uws_fd.fd);
        break;

    default:
        break;
    }
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   hs_lib_setup_server
 *          setup server
 *
 * @param[in]   uri                 URI
 * @param[in]   protocol            websocket's protocol name
 * @param[in]   callback            callback function
 * @return      handle 
 * @retval      >0                  success
 * @retval      NULL                error
 */
/*--------------------------------------------------------------------------*/
static hs_lib_handle_t *
hs_lib_setup_server(const char *uri, const char *protocol, ico_uws_evt_cb callback)
{
    int ret;
    struct ico_uws_context *context = NULL;
    hs_lib_handle_t *handle = NULL;

    handle = hs_lib_alloc_handle();
    if (! handle) {
        uifw_trace("hs_lib_setup_server: cannot allocate handle");
        return NULL;
    }

    /* create context */
    context = ico_uws_create_context(uri, ICO_HS_PROTOCOL);
    handle->uws_context = context;
    if (! context) {
        uifw_trace("hs_lib_setup_server: cannot create context");
        hs_lib_free_handle(handle);
        return NULL;
    }
    uifw_trace("hs_lib_setup_server: create ctx=%08x", (int)context);
    ico_uws_service(context);

    /* set callback */
    ret = ico_uws_set_event_cb(context, callback, (void *)handle);
    if (ret != ICO_UWS_ERR_NONE) {
        uifw_trace("hs_lib_setup_server: cannnot set callback");
        hs_lib_free_handle(handle);
        return NULL;
    }

    return handle;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_lib_main
 *          initialize homescreen lib to connect statusbar and onscreen,
 *          external command tools.
 *
 * @param[in]   port                websocket port
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
int
hs_lib_main(int port)
{
    char uri[ICO_HS_TEMP_BUF_SIZE];
    hs_lib_handle_t *handle = NULL;

    /* set up URI ":PORT" */
    sprintf(uri, ":%d", port);
    
    /* set up server */
    handle = hs_lib_setup_server(uri, ICO_HS_PROTOCOL, hs_lib_callback_uws);
    if ((handle == NULL) || (handle->uws_context == NULL)) {
        uifw_warn("home_screen_lib_main: ERROR(hs_lib_setup_server failed.)");
        return ICO_HS_ERR;
    }
    hs_wsicontext = handle->uws_context;

    ico_uws_service(hs_wsicontext);

    return ICO_HS_OK;
}
