/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   client library for communicate to AppsController in HomeScreen
 *
 * @date    Feb-28-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <libwebsockets.h>
#include "ico_apf_private.h"
#include "ico_uxf_conf.h"

/*==============================================================================*/
/* define static function prototype                                             */
/*==============================================================================*/
static ico_apf_com_handle_t *ico_apf_alloc_handle(void);
static int ico_apf_com_connect(ico_apf_com_handle_t *handle);
static int ico_apf_com_realsend(ico_apf_com_handle_t *handle,
                                ico_apf_com_buffer_t *msg);
static int ico_apf_callback_http(struct libwebsocket_context *context,
                                 struct libwebsocket *wsi,
                                 enum libwebsocket_callback_reasons reason,
                                 void *user, void *in, size_t len);
static int ico_apf_callback_websock(struct libwebsocket_context *this,
                                    struct libwebsocket *wsi,
                                    enum libwebsocket_callback_reasons reason,
                                    void *user, void *in, size_t len);
static void ico_apf_put_recvmsg(const int cmd, const int res, const int pid,
                                const char *appid, char *msg, const int len,
                                ico_apf_com_handle_t *handle);
static int ico_apf_poll_fd_add(const int fd, const int flag);
static void ico_apf_poll_fd_del(const int fd);
static void ico_apf_poll_write_event(ico_apf_com_handle_t *handle, const int write);

/*==============================================================================*/
/* variable & table                                                             */
/*==============================================================================*/
/* variable that control program name in Linux  */
extern const char *program_invocation_name;

/* application framework control handle */
static struct libwebsocket_context  *global_lwscontext = NULL;
static ico_apf_com_handle_t *handles = NULL;
static ico_apf_com_handle_t *freehandles = NULL;

/* file descriptor controll table       */
static ico_apf_com_poll_t       *com_polls = NULL;
static ico_apf_com_poll_t       *freecom_polls = NULL;
static ico_apf_com_pollfd_cb   ico_apf_pollfd_func = NULL;

/* global user listener                 */
static ico_apf_com_eventlistener    global_listener = NULL;
static void                         *global_user_data = NULL;

/* flag for callback from libwebsockets */
static int      lws_callbacked = 0;

/* libwebsockets's protocol number      */
enum appsctl_protocols {
    PROTOCOL_HTTP,                      /* HTTP handshake(Certainly required)   */
    PROTOCOL_APPSCONTROLLER,            /* AppsController protocol              */
    PROTOCOL_APPSEND                    /* everytime means final                */
};
enum soundmgr_protocols {
    PROTOCOL_SOUNDMGR,                  /* Multi Sound Manager  protocol        */
    PROTOCOL_SOUNDEND                   /* everytime means final                */
};

/* list of libwebsockets protocol for AppsController        */
static struct libwebsocket_protocols apps_protocols[] = {
                                        /* HTTP handshake(Certainly required)   */
    { "http_only", ico_apf_callback_http, 0 },
                                        /* callback from websocket to appscontroller*/
    { ICO_PROTOCOL_APPSCONTROLLER, ico_apf_callback_websock, sizeof(void *) },
    { NULL, NULL, 0 }                   /* everytime means final                */
};

/* list of libwebsockets protocol for pulse-audio plugin    */
static struct libwebsocket_protocols sound_protocols[] = {
                                        /* callback from websocket to soundmanager*/
    { ICO_PROTOCOL_MULTISOUNDMANAGER, ico_apf_callback_websock, sizeof(void *) },
    { NULL, NULL, 0 }                   /* everytime means final                */
};

/* command/event string                 */
const char  *ico_apf_cmd_table[] = {
    ICO_APF_SRESOURCE_STATE_ACQUIRED,       /* acquired         */
    ICO_APF_SRESOURCE_STATE_DEPRIVED,       /* deprived         */
    ICO_APF_SRESOURCE_STATE_WAITTING,       /* waitting         */
    ICO_APF_SRESOURCE_STATE_RELEASED,       /* released         */

    ICO_APF_SRESOURCE_COMMAND_GET,          /* get command      */
    ICO_APF_SRESOURCE_COMMAND_RELEASE,      /* release command  */
    ICO_APF_SRESOURCE_COMMAND_ADD,          /* add command      */
    ICO_APF_SRESOURCE_COMMAND_CHANGE,       /* change command   */
    ICO_APF_SRESOURCE_COMMAND_DELETE,       /* delete command   */

    ICO_APF_SRESOURCE_REPLY_OK,             /* OK reply         */
    ICO_APF_SRESOURCE_REPLY_NG,             /* NG reply         */
    ICO_APF_SRESOURCE_STATE_CONNECTED,      /* connected        */
    ICO_APF_SRESOURCE_STATE_DISCONNECTED    /* disconnected     */
};
const char  *ico_apf_res_table[] = {
    ICO_APF_SRESID_BASIC_SCREEN,            /* basic screen     */
    ICO_APF_SRESID_INT_SCREEN,              /* interrupt screen */
    ICO_APF_SRESID_ON_SCREEN,               /* onscreeen        */
    ICO_APF_SRESID_BASIC_SOUND,             /* basic sound      */
    ICO_APF_SRESID_INT_SOUND,               /* interrupt sound  */
    ICO_APF_SRESID_INPUT_DEV                /* input device     */
};
const char  *ico_apf_sound_table[] = {
    ICO_APF_SSOUND_COMMAND_MUTEON,          /* mute on          */
    ICO_APF_SSOUND_COMMAND_MUTEOFF,         /* mute off         */
    ICO_APF_SSOUND_COMMAND_CANCEL,          /* cancel           */
    ICO_APF_SSOUND_COMMAND_GETLIST,         /* get stream list  */
    ICO_APF_SSOUND_COMMAND_FRESH,           /* fresh            */
    ICO_APF_SSOUND_COMMAND_ALLMUTEON,       /* mute set all stream*/
    ICO_APF_SSOUND_COMMAND_ALLMUTEOFF,      /* mute reset all stream*/
    ICO_APF_SSOUND_EVENT_NEW,               /* created new stream*/
    ICO_APF_SSOUND_EVENT_FREE,              /* destoryed stream */
    ICO_APF_SSOUND_REPLY_LIST               /* reply of stream list*/
};

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_init_client
 *          This function connects to AppsController in HomeScreen.
 *          If you use AppsController's function, you must call this function.
 *
 * @param[in]   uri                 connection URI, NULL is host myself
 * @param[in]   type                connection type
 * @return      handle address
 * @retval      handle address      success
 * @retval      NULL                error(failed connect AppsController)
 */
/*--------------------------------------------------------------------------*/
ico_apf_com_handle_t *
ico_apf_com_init_client(const char *uri, const int type)
{
    int i;
    int port;
    char* address;
    ico_apf_com_poll_t *p;
    ico_apf_com_handle_t *handle;
    Ico_Uxf_Sys_Config *sysconf;
    Ico_Uxf_Sys_Config *ifsysconf = (Ico_Uxf_Sys_Config *)0xffffffff;
    char *port_env;
    char *host_env;

    apfw_trace("ico_apf_com_init_client: Enter(%s,%d)",
                uri ? uri : "(NULL)", type);

    ifsysconf = (Ico_Uxf_Sys_Config *)ico_uxf_ifGetSysConfig();

    if (ifsysconf)  {
        /* Another module already reads config file                             */
        /* In this case, you don't release sysconf at finished this function.   */
        sysconf = (Ico_Uxf_Sys_Config *)ifsysconf;
    }
    else    {
        /* never read a system config                   */
        sysconf = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    }
    if (!sysconf)   {
        apfw_error("ico_apf_com_init_client: Leave(can not read configuration files)");
        return NULL;
    }

    /* If URI is host name, find address from config.   */
    /* If node is a null character string, it regards NULL. */
    Ico_Uxf_conf_host *phost =
        (Ico_Uxf_conf_host *) ico_uxf_getSysHostById(ico_uxf_getSysHostByName(uri));
    if (phost)  {
        address = phost->address;
    }
    else    {
        /* If don't find host name from config, regards ipaddress is set as node.*/
        address = (char *)uri;
    }

    /* environment variable for port & host */
    if (type == ICO_APF_COM_TYPE_APPSCTL)   {
        port = sysconf->misc.appscontrollerport;
        port_env = getenv("ICO_APF_APPSCTL_PORT");
        host_env = getenv("ICO_APF_APPSCTL_HOST");
    }
    else    {
        port = sysconf->misc.soundpluginport;
        port_env = getenv("ICO_APF_SOUNDMGR_PORT");
        host_env = getenv("ICO_APF_SOUNDMGR_HOST");
    }
    if (port_env) {
        i = atoi(port_env);
        if (i > 0)  port = i;
    }
    if (host_env)   {
        address = host_env;
    }

    /* get a port number from system configuration(system/system.conf)  */
    apfw_trace("ico_apf_com_init_client: connect Host=%s Port=%d", address, port);

    handle = handles;
    while (handle)  {
        if ((strcmp(handle->ip, address) == 0) &&
            (handle->port == port) && (handle->type == type))   {
            break;
        }
        handle = handle->next;
    }

    if (handle) {
        apfw_trace("ico_apf_com_init_client: Leave(Connected)");
        return handle;
    }

    handle = ico_apf_alloc_handle();
    if (! handle)   {
        apfw_trace("ico_apf_com_init_client: Leave(No Memory)");
        return NULL;
    }

    handle->type = type;
    handle->port = port;
    strncpy(handle->ip, address, ICO_APF_RESOURCE_IP_LEN-1);
    handle->ip[ICO_APF_RESOURCE_IP_LEN-1] = 0;

    /* connect to AppsController            */
    i = ico_apf_com_connect(handle);

    if (! ifsysconf)    {
        ico_uxf_closeSysConfig();
    }

    if (i != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_com_init_client: Leave(RESOURCE_E_INIT_COM_FAILD)");
        (void) ico_apf_com_term_client(handle);
        return NULL;
    }

    /* Try to connection        */
    ico_apf_com_dispatch(handle, 0);
    handle->fd = libwebsocket_get_socket_fd(handle->wsi_connection);

    /* add poll callback if fd is exist */
    if (handle->fd > 0) {
        handle->service_on = 1;
        (void)ico_apf_poll_fd_add(handle->fd, POLLIN);
    }

    p = com_polls;
    while (p)   {
        if (p->fd == handle->fd)    {
            p->apf_fd_control = (void *)handle;
            handle->poll = p;
        }
        p = p->next;
    }
    apfw_trace("ico_apf_com_init_client: Leave(OK,fd=%d)", handle->fd);
    return handle;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_init_server
 *          This function connects to aplicationto in AppsController.
 *
 * @param[in]   uri                 my URI, NULL is host myself
 * @param[in]   type                connection type
 * @return      handle address
 * @retval      handle address      success
 * @retval      NULL                error(failed connect AppsController)
 */
/*--------------------------------------------------------------------------*/
ico_apf_com_handle_t *
ico_apf_com_init_server(const char *uri, const int type)
{
    int i;
    int port;
    int svrtype;
    char* address;
    ico_apf_com_handle_t *handle;
    Ico_Uxf_Sys_Config *sysconf;
    Ico_Uxf_Sys_Config *ifsysconf = (Ico_Uxf_Sys_Config *)0xffffffff;
    char *port_env;

    apfw_trace("ico_apf_com_init_server: Enter(%s,%d)",
               uri ? uri : "(NULL)", type);

    ifsysconf = (Ico_Uxf_Sys_Config *)ico_uxf_ifGetSysConfig();

    if (ifsysconf)  {
        /* Another module already reads config file                             */
        /* In this case, you don't release sysconf at finished this function.   */
        sysconf = (Ico_Uxf_Sys_Config *)ifsysconf;
    }
    else    {
        /* never read a system config                   */
        sysconf = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    }
    if (!sysconf)   {
        apfw_error("ico_apf_com_init_server: Leave(can not read configuration files)");
        return NULL;
    }

    /* currently server support only Application Controler  */
    svrtype = ICO_APF_COM_TYPE_SVR_APPSCTL;

    /* If URI is host name, find address from config.   */
    /* If node is a null character string, it regards NULL. */
    Ico_Uxf_conf_host *phost =
        (Ico_Uxf_conf_host *) ico_uxf_getSysHostById(ico_uxf_getSysHostByName(uri));
    if (phost)  {
        address = phost->address;
    }
    else    {
        /* If don't find host name from config, regards ipaddress is set as node.*/
        address = (char *)uri;
    }

    /* get a port number from system configuration(system/system.conf)  */
    port = sysconf->misc.appscontrollerport;
    apfw_trace("ico_apf_com_init_server: Config, Host=%s Port=%d", address, port);

    /* environment variable for port        */
    port = sysconf->misc.appscontrollerport;
    port_env = getenv("ICO_APF_APPSCTL_PORT");
    if (port_env) {
        i = atoi(port_env);
        if (i > 0)  port = i;
    }

    handle = handles;
    while (handle)  {
        if ((strcmp(address, handle->ip) == 0) &&
            (handle->port == port) && (handle->type == svrtype))    break;
        handle = handle->next;
    }

    if (handle) {
        apfw_trace("ico_apf_com_init_server: Leave(Created)");
        return handle;
    }

    handle = ico_apf_alloc_handle();
    if (! handle)   {
        apfw_error("ico_apf_com_init_server: Leave(No Memory)");
        return NULL;
    }

    handle->type = svrtype;
    handle->port = port;
    strncpy(handle->ip, address, ICO_APF_RESOURCE_IP_LEN-1);
    handle->ip[ICO_APF_RESOURCE_IP_LEN-1] = 0;

    /* connect to AppsController            */
    handle->wsi_context = libwebsocket_create_context(port, NULL, apps_protocols,
                                                      libwebsocket_internal_extensions,
                                                      NULL, NULL, -1, -1, 0);
    if (! handle->wsi_context)  {
        apfw_error("ico_apf_com_init_server: Leave(libwebsockets create Error)");
        (void) ico_apf_com_term_server(handle);
        return NULL;
    }
    apfw_trace("ico_apf_com_init_server: create server context 0x%08x",
               (int)handle->wsi_context);

    global_lwscontext = handle->wsi_context;

    ico_apf_com_dispatch(handle, 0);

    apfw_trace("ico_apf_com_init_server: Leave(OK)");
    return handle;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_alloc_handle
 *          Allocate ccommunication handle.
 *
 * @param       none
 * @return      handle address
 * @retval      addres      success, return handle address
 * @retval      NULL        error(no memory)
 */
/*--------------------------------------------------------------------------*/
static ico_apf_com_handle_t *
ico_apf_alloc_handle(void)
{
    ico_apf_com_handle_t *handle;
    int     i;

    if (! freehandles)  {

        handle = malloc(sizeof(ico_apf_com_handle_t));
        if (! handle)   {
            return NULL;
        }
        memset(handle, 0, sizeof(ico_apf_com_handle_t));

        /* Queue buffers initialize.                */
        for (i = 0; i < ICO_APF_RESOURCE_WSOCK_BUFR; i++)   {
            handle->rbuf[i] = calloc(sizeof(ico_apf_com_buffer_t), 1);
            if (! handle->rbuf[i])  {
                return NULL;
            }
        }
        for (i = 0; i < ICO_APF_RESOURCE_WSOCK_BUFS; i++)   {
            handle->sbuf[i] = calloc(sizeof(ico_apf_com_buffer_t), 1);
            if (! handle->sbuf[i])  {
                return NULL;
            }
        }
    }
    else    {
        handle = freehandles;
        freehandles = freehandles->next;
        handle->next = NULL;
        handle->fd = 0;
        handle->pid = 0;
        handle->wsi_context = NULL;
        handle->wsi_connection = NULL;
        handle->service_on = 0;
        handle->retry = 0;
        handle->stoprecv = 0;
        handle->shead = 0;
        handle->stail = 0;
        handle->rhead = 0;
        handle->rtail = 0;
        handle->listener = NULL;
        handle->user_data = NULL;
        handle->type = 0;
    }

    if (handles)    {
        handle->next = handles;
    }
    handles = handle;

    return handle;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_connect
 *          This function connects to AppsController.
 *
 * @param[in]   handle          Communication handle
 * @return      result status
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_INIT_COM_FAILD   error(connect failed)
 */
/*--------------------------------------------------------------------------*/
static int
ico_apf_com_connect(ico_apf_com_handle_t *handle)
{
    int     rep;

    apfw_trace("ico_apf_com_connect: Enter(type=%d)", handle->type);

    int ietf_version = -1;      /* latest */
    char origin[1024];
    sprintf(origin,"%d %s",getpid(),program_invocation_name);

    apfw_trace("ico_apf_com_connect: libwebsocket_create_context");
    handle->wsi_context = libwebsocket_create_context(
                                    CONTEXT_PORT_NO_LISTEN, NULL,
                                    (handle->type & ICO_APF_COM_TYPE_CONNECTION)
                                            == ICO_APF_COM_TYPE_SOUNDMGR ?
                                        sound_protocols : apps_protocols,
                                    libwebsocket_internal_extensions,
                                    NULL, NULL, -1, -1, 0);
    if (handle->wsi_context == NULL) {
        apfw_error("ico_apf_com_connect: Leave(RESOURCE_E_INIT_COM_FAILD)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }
    apfw_trace("ico_apf_com_connect: create client context 0x%08x",
               (int)handle->wsi_context);

    /* connect to Application Manager by WebSocket      */
    apfw_trace("ico_apf_com_connect: libwebsocket_client_connect %s:%d",
               handle->ip, handle->port);
    for (rep = 0; rep < (1000/50); rep++)   {
        handle->wsi_connection = libwebsocket_client_connect(
                                        handle->wsi_context,
                                        handle->ip, handle->port, 0,
                                        "/", handle->ip, origin,
                                        (handle->type == ICO_APF_COM_TYPE_APPSCTL) ?
                                        apps_protocols[PROTOCOL_APPSCONTROLLER].name :
                                        sound_protocols[PROTOCOL_SOUNDMGR].name,
                                        ietf_version);
        if (handle->wsi_connection) break;
        usleep(50*1000);
    }
    if (handle->wsi_connection == NULL) {
        apfw_warn("ico_apf_com_connect: Leave(RESOURCE_E_INIT_COM_FAILD)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }
    handle->fd = libwebsocket_get_socket_fd(handle->wsi_connection);
    if (handle->fd > 0) {
        (void)ico_apf_poll_fd_add(handle->fd, POLLIN|POLLOUT);
    }
    ico_apf_com_dispatch(handle, 0);

    apfw_trace("ico_apf_com_connect: Leave(OK, fd=%d)", handle->fd);
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_isconnected
 *          return a connection status with AppsController
 *
 * @param[in]   handle      AppsController's handle
 * @return      connection status
 * @retval      1           connect
 * @retval      0           not connetc
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_isconnected(ico_apf_com_handle_t *handle)
{
    return((handle != NULL) && (handle->service_on != 0));
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_dispatch
 *          The accumulated transmitted and received data is processed.
 *          Connecting AppsController program must call this function.
 *
 * @param[in]   handle      connect handle, if NULL target is all connect
 * @param[in]   timeoutms   maximum wait time on miri-sec.
 *                          0 is no wait, -1 is wait forever.
 * @return      non
 */
/*--------------------------------------------------------------------------*/
void
ico_apf_com_dispatch(ico_apf_com_handle_t *handle, const int timeoutms)
{
    int rh;
    int n;
    ico_apf_com_handle_t *p;

    if (! handles)  {
        /* If program has not connected, nothing is done.   */
        apfw_warn("ico_apf_com_dispatch: handle dose not exist");
        return;
    }

    if (handle) {
        p = handle;
    }
    else    {
        p = handles;
    }

    while (p)   {
        if (libwebsocket_service(p->wsi_context, timeoutms) < 0)    {
            apfw_warn("ico_apf_com_dispatch: fd=%d is done", p->fd);
        }

        /* If received data is suspended, it processes.     */
        while (p->rtail != p->rhead)    {
            rh = p->rhead;
            if (p->rhead >= (ICO_APF_RESOURCE_WSOCK_BUFR-1))    {
                p->rhead = 0;
            }
            else    {
                p->rhead ++;
            }
            n = p->rtail - p->rhead;
            if (n < 0) {
                n = ICO_APF_RESOURCE_WSOCK_BUFR + n;
            }
            if ((p->stoprecv != 0) &&
                (n < (ICO_APF_RESOURCE_WSOCK_BUFR/2)))  {
                /* If suspending received data is bellow half, request a send process */
                p->stoprecv = 0;
                apfw_trace("ico_apf_com_dispatch: Flow Control(Start) %d", n);
                libwebsocket_rx_flow_control(p->wsi_connection, 1);
                libwebsocket_service(p->wsi_context, 0);
            }

            if (p->listener != NULL) {
                handle->listener(p, p->rbuf[rh]->cmd, p->rbuf[rh]->res, p->rbuf[rh]->pid,
                                 p->rbuf[rh]->appid, p->rbuf[rh]->msg, p->user_data);
                libwebsocket_service(p->wsi_context, 0);
            }
            else if (global_listener != NULL) {
                global_listener(p, p->rbuf[rh]->cmd, p->rbuf[rh]->res, p->rbuf[rh]->pid,
                                p->rbuf[rh]->appid, p->rbuf[rh]->msg, global_user_data);
                libwebsocket_service(p->wsi_context, 0);
            }
        }
        if (handle) break;
        p = p->next;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_term_client
 *          finish to connect to AppsController
 *          It recommends call this function before end of program
 *          that used AppsController.
 *
 * @param[in]   handle      AppsController's handle
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(already finished)
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_term_client(ico_apf_com_handle_t *handle)
{
    ico_apf_com_handle_t    *p;

    apfw_trace("ico_apf_com_term_client: Enter");

    if ((handle == NULL) || (handles == NULL))  {
        apfw_trace("ico_apf_com_term_client: Leave(RESOURCE_E_NOT_INITIALIZED)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }
    handle->service_on = 0;

    if (handle == handles)  {
        handles = handles->next;
    }
    else    {
        p = handles;
        while (p)   {
            if (p->next == handle)  break;
            p = p->next;
        }
        if (! p)    {
            apfw_trace("ico_apf_com_term_client: Leave(RESOURCE_E_NOT_INITIALIZED)");
            return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
        }
        p->next = handle->next;
    }

    if (handle->wsi_context)    {
        libwebsocket_context_destroy(handle->wsi_context);
    }
    handle->next = freehandles;
    freehandles = handle;

    apfw_trace("ico_apf_com_term_client: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_term_server
 *          finish to connect to application
 *
 * @param[in]   handle      AppsController's handle
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(already finished)
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_term_server(ico_apf_com_handle_t *handle)
{
    ico_apf_com_handle_t    *p;

    apfw_trace("ico_apf_com_term_server: Enter");

    if ((handle == NULL) || (handles == NULL))  {
        apfw_trace("ico_apf_com_term_server: Leave(RESOURCE_E_NOT_INITIALIZED)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }
    handle->service_on = 0;

    if (handle == handles)  {
        handles = handles->next;
    }
    else    {
        p = handles;
        while (p)   {
            if (p->next == handle)  break;
            p = p->next;
        }
        if (! p)    {
            apfw_trace("ico_apf_com_term_server: Leave(RESOURCE_E_NOT_INITIALIZED)");
            return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
        }
        p->next = handle->next;
    }

    if (handle->wsi_context)    {
        libwebsocket_context_destroy(handle->wsi_context);
    }
    handle->next = freehandles;
    freehandles = handle;

    apfw_trace("ico_apf_com_term_server: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_send
 *          Accumulates the data send to AppsController/application.
 *
 * @param[in]   handle      AppsController's handle
 * @param[in]   cmd         command
 * @param[in]   res         resource
 * @param[in]   appid       application id
 * @param[in]   msg         send message
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(not connect)
 * @retval      ICO_APF_RESOURCE_E_COMMUNICATION    error(cannot send)
 * @retval      ICO_APF_RESOURCE_E_INVAL            error(invalid parameter)
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_send(ico_apf_com_handle_t *handle,
                 const int cmd, const int res, const char *appid, char *msg)
{
    int     st;
    int     len;

    apfw_trace("ico_apf_com_send: Enter(%08x, %d, %d)", (int)handle, cmd, res);

    if ((handle == NULL) || (! ico_apf_com_isconnected(handle)))    {
        apfw_warn("ico_apf_com_send: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }
    if ((handle->type & ICO_APF_COM_TYPE_CONNECTION) != ICO_APF_COM_TYPE_SOUNDMGR)  {
        /* send to AppsController               */
        if ((cmd < ICO_APF_RESOURCE_COMMAND_MIN) ||
            (cmd > ICO_APF_RESOURCE_COMMAND_MAX) ||
            (res < ICO_APF_RESOURCE_RESID_MIN) ||
            (res > ICO_APF_RESOURCE_RESID_MAX))   {
            apfw_warn("ico_apf_com_send: Leave(invalid command for resource)");
            return ICO_APF_RESOURCE_E_INVAL;
        }
    }
    else    {
        /* send to Multi Sound Manager          */
        if ((cmd < ICO_APF_SOUND_COMMAND_MIN) ||
            (cmd > ICO_APF_SOUND_COMMAND_CMD))  {
            apfw_warn("ico_apf_com_send: Leave(invalid command for sound control)");
            return ICO_APF_RESOURCE_E_INVAL;
        }
    }

    if (msg)    {
        len = strlen(msg);
        if (len >= ICO_APF_RESOURCE_MSG_LEN)    {
            apfw_warn("ico_apf_com_send: Leave(invalid message length)");
            return ICO_APF_RESOURCE_E_INVAL;
        }
    }
    else    {
        len = 0;
        msg = "(NULL)";
    }

    st = handle->stail;

    if (st >= (ICO_APF_RESOURCE_WSOCK_BUFS-1))  {
        st = 0;
    }
    else    {
        st ++;
    }
    if (st == handle->shead)    {
        /* send buffer is full                  */
        ico_apf_poll_write_event(handle, 3);

        apfw_error("ico_apf_com_send: Leave(send buffer overflow)");
        ico_apf_poll_fd_del(handle->fd);
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    /* accumulate send buffer                   */
    if ((! appid) || (*appid == 0)) {
        handle->sbuf[handle->stail]->pid = getpid();
        handle->sbuf[handle->stail]->appid[0] = 0;
    }
    else    {
        handle->sbuf[handle->stail]->pid = 0;
        strncpy(handle->sbuf[handle->stail]->appid, appid, ICO_UXF_MAX_PROCESS_NAME);
        handle->sbuf[handle->stail]->appid[ICO_UXF_MAX_PROCESS_NAME] = 0;
    }
    apfw_trace("ico_apf_com_send: Send.%d:%d %d %d(%s) <%s>",
               handle->stail, cmd, res, handle->sbuf[handle->stail]->pid,
               handle->sbuf[handle->stail]->appid, msg);
    handle->sbuf[handle->stail]->cmd = cmd;
    handle->sbuf[handle->stail]->res = res;
    memcpy(handle->sbuf[handle->stail]->msg, msg, len);
    handle->sbuf[handle->stail]->msg[len] = 0;
    handle->stail = st;

    libwebsocket_callback_on_writable(handle->wsi_context, handle->wsi_connection);

    if (lws_callbacked) {
        /* send call from libwensockets callback, delayed send  */
        ico_apf_poll_write_event(handle, 1);
    }
    else    {
        /* not call from callback, direct send                  */
        ico_apf_com_dispatch(handle, 0);        /* try to service           */
    }
    apfw_trace("ico_apf_com_send: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_realsend
 *          send messege to AppsController
 *
 * @param[in]   handle      AppsController's handle
 * @param[in]   msg         Send message
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(not connect)
 * @retval      ICO_APF_RESOURCE_E_COMMUNICATION    error(cannot send)
 * @retval      ICO_APF_RESOURCE_E_INVAL            error(invalid parameter)
 */
/*--------------------------------------------------------------------------*/
static int
ico_apf_com_realsend(ico_apf_com_handle_t *handle, ico_apf_com_buffer_t *msg)
{
    unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + 512 +
                          LWS_SEND_BUFFER_POST_PADDING];
    unsigned char *pt = &buf[LWS_SEND_BUFFER_PRE_PADDING];
    int     n;

    apfw_trace("ico_apf_com_realsend: Enter");
    if ((handle == NULL) || (! ico_apf_com_isconnected(handle)))    {
        apfw_warn("ico_apf_com_realsend: Leave(Not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    if ((handle->type & ICO_APF_COM_TYPE_CONNECTION) == ICO_APF_COM_TYPE_SOUNDMGR)  {
        /* send to Multi Sound Manager          */
        if (msg->res)   {
            sprintf((char *)pt, "%s %d",
                    ico_apf_sound_table[msg->cmd-ICO_APF_SOUND_COMMAND_MIN], msg->res);
        }
        else    {
            strcpy((char *)pt, ico_apf_sound_table[msg->cmd-ICO_APF_SOUND_COMMAND_MIN]);
        }
    }
    else    {
        /* send tp AppsController               */
        if (msg->appid[0])  {
            sprintf((char *)pt, "%s %s %s %s",
                    ico_apf_cmd_table[msg->cmd-ICO_APF_RESOURCE_COMMAND_MIN],
                    ico_apf_res_table[msg->res-ICO_APF_RESOURCE_RESID_MIN],
                    msg->appid, msg->msg);
        }
        else    {
            sprintf((char *)pt, "%s %s %d %s",
                    ico_apf_cmd_table[msg->cmd-ICO_APF_RESOURCE_COMMAND_MIN],
                    ico_apf_res_table[msg->res-ICO_APF_RESOURCE_RESID_MIN],
                    msg->pid, msg->msg);
        }
    }

    apfw_trace("ico_apf_com_realsend: libwebsocket_write[%s]", pt);
    n = libwebsocket_write(handle->wsi_connection,
                           pt, strlen((char *)pt), LWS_WRITE_TEXT);
    if (n < 0)  {
        apfw_error("ico_apf_com_realsend: write error[%d]", n);
        ico_apf_poll_fd_del(handle->fd);
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }
    usleep(200);    /* According to libwebsockets's sample, this is required    */
    apfw_trace("ico_apf_com_realsend: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_addeventlistener
 *          register a callback function called receive message from AppsController
 *
 * @param[in]   handle      AppsComtroller's handle
 * @param[in]   listener    callback function
 * @param[in]   user_data   Arbitrary data
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(not connect)
 * @retval      ICO_APF_RESOURCE_E_INVAL            error(already registed)
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_addeventlistener(ico_apf_com_handle_t *handle,
                             ico_apf_com_eventlistener listener, void* user_data)
{
    if (handle == NULL) {
        if (global_listener)    {
            return ICO_APF_RESOURCE_E_INVAL;
        }
        global_listener = listener;
        global_user_data = user_data;
        return ICO_APF_RESOURCE_E_NONE;
    }

    if (handle->listener)   {
        return ICO_APF_RESOURCE_E_INVAL;
    }
    handle->user_data = user_data;
    handle->listener = listener;

    if (! global_listener)  {
        /* save listener        */
        global_listener = listener;
    }
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_removeeventlistener
 *          unregister a callback function called receive message
 *
 * @param[in]   handle      AppsComtroller's handle
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_NOT_INITIALIZED  error(not connect)
 * @retval      ICO_APF_RESOURCE_E_INVAL            error(do not registed)
 */
/*--------------------------------------------------------------------------*/
int
ico_apf_com_removeeventlistener(ico_apf_com_handle_t *handle)
{
    if (handle == NULL) {
        if (! global_listener)  {
            return ICO_APF_RESOURCE_E_INVAL;
        }
        global_listener = NULL;
        global_user_data = NULL;
        return ICO_APF_RESOURCE_E_NONE;
    }

    if (! handle->listener) {
        return ICO_APF_RESOURCE_E_INVAL;
    }
    handle->listener = NULL;
    handle->user_data = NULL;

    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_callback_http
 *          Connection status is notified from libwebsockets.
 *
 * @param[in]   context     libwebsockets context
 * @param[in]   wsi         libwebsockets management table
 * @param[in]   reason      event type
 * @param[in]   user        intact
 * @param[in]   in          receive message
 * @param[in]   len         message size[BYTE]
 * @return      result
 * @retval      =0          success
 * @retval      =1          error
 */
/*--------------------------------------------------------------------------*/
static int
ico_apf_callback_http(struct libwebsocket_context *context,
                      struct libwebsocket *wsi,
                      enum libwebsocket_callback_reasons reason,
                      void *user, void *in, size_t len)
{
    int fd;

    fd = libwebsocket_get_socket_fd(wsi);
    lws_callbacked ++;

    switch (reason) {
    case LWS_CALLBACK_FILTER_NETWORK_CONNECTION:
                                            /* a connection demand is received  */
        apfw_trace("ico_apf_callback_http: LWS_CALLBACK_FILTER_NETWORK_CONNECTION fd=%d", fd);

        /* If connection is refused, it will return with values other than zero.*/
        /* At present, since access control is not introduced,                  */
        /* connection is always permitted.                                      */
        break;

    case LWS_CALLBACK_ADD_POLL_FD:          /* add connection socket            */
        apfw_trace("ico_apf_callback_http: LWS_CALLBACK_ADD_POLL_FD(%d) %d,%x", fd,
                   (int)user, len);
        (void)ico_apf_poll_fd_add((int)user, (int)len);
        break;

    case LWS_CALLBACK_DEL_POLL_FD:          /* delete connection socket         */
        apfw_trace("ico_apf_callback_http: LWS_CALLBACK_DEL_POLL_FD(%d) %d,%x", fd,
                   (int)user, len);
        ico_apf_poll_fd_del((int)user);
        break;

    case LWS_CALLBACK_SET_MODE_POLL_FD:     /* set status                       */
        apfw_trace("ico_apf_callback_http: LWS_CALLBACK_SET_MODE_POLL_FD(%d) %d,%x", fd,
                   (int)user, len);
        (void)ico_apf_poll_fd_add((int)user, (int)len);
        break;

    case LWS_CALLBACK_CLEAR_MODE_POLL_FD:   /* clear status                     */
        apfw_trace("ico_apf_callback_http: LWS_CALLBACK_CLEAR_MODE_POLL_FD(%d) %d,%x", fd,
                   (int)user, len);
        (void)ico_apf_poll_fd_add((int)user, -((int)len));
        break;

    default:
        apfw_trace("ico_apf_callback_http: Unknown reason(%d, fd=%d)", reason, fd);
        break;
    }
    lws_callbacked --;

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_callback_websock
 *          this callback function notified from libwebsockets.
 *
 * @param[in]   context     libwebsockets context
 * @param[in]   wsi         libwebsockets management table
 * @param[in]   reason      event type
 * @param[in]   user        intact
 * @param[in]   in          receive message
 * @param[in]   len         message size[BYTE]
 * @return      result
 * @retval      =0          success
 * @retval      =1          error
 */
/*--------------------------------------------------------------------------*/
static int
ico_apf_callback_websock(struct libwebsocket_context * this,
                         struct libwebsocket *wsi,
                         enum libwebsocket_callback_reasons reason,
                         void *user, void *in, size_t len)
{
    int     wsifd;
    int     st;
    int     cmd;
    int     res;
    int     pid;
    int     i;
    int     strlen;
    char    *msg;
    ico_apf_com_poll_t *p;
    ico_apf_com_handle_t *handle;
    struct sockaddr_in  sin;
    unsigned int        sin_len;
    char    appid[ICO_UXF_MAX_PROCESS_NAME+1];
    char    strname[ICO_UXF_MAX_STREAM_NAME+1];
    int     nparam;
    struct  {
        short   start;
        short   end;
    }       param[10];

    wsifd = libwebsocket_get_socket_fd(wsi);

    lws_callbacked ++;

    if (reason == LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION)  {
        apfw_trace("ico_apf_callback_websock: server side connected from client");

        /* connect from client      */
        handle = handles;
        while (handle)  {
            if (handle->fd == wsifd)    break;
            handle = handle->next;
        }
        if (! handle)   {
            handle = ico_apf_alloc_handle();
            if (! handle)   {
                lws_callbacked --;
                apfw_error("ico_apf_callback_websock: No Memory");
                return 1;
            }
        }
        handle->type = ICO_APF_COM_TYPE_APPSCTL;
        handle->wsi_context = global_lwscontext;
        handle->wsi_connection = wsi;
        handle->fd = wsifd;
        handle->service_on = 0;
        handle->stoprecv = 0;

        sin_len = sizeof(sin);
        if (getpeername(wsifd, (struct sockaddr *) &sin, &sin_len) >= 0)  {
            sprintf(handle->ip, "%u.%u.%u.%u",
                    (sin.sin_addr.s_addr) & 0x0ff,
                    (sin.sin_addr.s_addr >> 8) & 0x0ff,
                    (sin.sin_addr.s_addr >> 16) & 0x0ff,
                    (sin.sin_addr.s_addr >> 24) & 0x0ff);
        }
        else    {
            strcpy(handle->ip, "127.0.0.1");
        }

        p = com_polls;
        while (p)   {
            if (p->fd == handle->fd)    {
                p->apf_fd_control = (void *)handle;
                handle->poll = p;
            }
            p = p->next;
        }
        lws_callbacked --;
        apfw_trace("ico_apf_callback_websock: server side connected");
        return 0;
    }

    handle = handles;
    while (handle)  {
        if (handle->wsi_connection == wsi) break;
        handle = handle->next;
    }

    if (! handle)   {
        lws_callbacked --;
        apfw_trace("ico_apf_callback_websock: Handle not exist");
        return 0;
    }

    switch (reason) {
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
    case LWS_CALLBACK_ESTABLISHED:
        apfw_trace("ico_apf_callback_websock: LWS_CALLBACK_ESTABLISHED");
        handle->service_on = 1;
        handle->stoprecv = 0;

        /* save receive message to receive buffer       */
        ico_apf_put_recvmsg(ICO_APF_RESOURCE_STATE_CONNECTED, 0,
                            handle->pid, "\0", NULL, 0, handle);
        break;

    case LWS_CALLBACK_ADD_POLL_FD:          /* add connection socket            */
        (void)ico_apf_poll_fd_add((int)user, (int)len);
        break;

    case LWS_CALLBACK_DEL_POLL_FD:          /* delete connection socket         */
        ico_apf_poll_fd_del((int)user);
        break;

    case LWS_CALLBACK_SET_MODE_POLL_FD:     /* set status                       */
        (void)ico_apf_poll_fd_add((int)user, (int)len);
        break;

    case LWS_CALLBACK_CLEAR_MODE_POLL_FD:   /* clear status                     */
        (void)ico_apf_poll_fd_add((int)user, -((int)len));
        break;

    case LWS_CALLBACK_CLOSED:
        apfw_trace("ico_apf_callback_websock: LWS_CALLBACK_CLOSED");
        handle->service_on = 0;

        /* save receive message to receive buffer       */
        ico_apf_put_recvmsg(ICO_APF_RESOURCE_STATE_DISCONNECTED, 0,
                            handle->pid, "\0", NULL, 0, handle);
        break;

    case LWS_CALLBACK_CLIENT_WRITEABLE:
    case LWS_CALLBACK_SERVER_WRITEABLE:
        if (handle->stail != handle->shead)    {
            st = handle->shead;
            if (handle->shead >= (ICO_APF_RESOURCE_WSOCK_BUFS-1))  {
                handle->shead = 0;
            }
            else    {
                handle->shead ++;
            }
            if (ico_apf_com_realsend(handle, handle->sbuf[st])
                        != ICO_APF_RESOURCE_E_NONE) {
                apfw_warn("ico_apf_callback_websock: ico_apf_com_realsend Error");
                handle->shead = st;
            }
            if (handle->stail != handle->shead)    {
                libwebsocket_callback_on_writable(handle->wsi_context,
                                                  handle->wsi_connection);
            }
        }
        break;

    case LWS_CALLBACK_CLIENT_RECEIVE:
    case LWS_CALLBACK_RECEIVE:
        msg = (char *)in;
        msg[len] = 0;
        if ((len <= 9) || (len >= (ICO_APF_RESOURCE_MSG_LEN+9)))    {
            lws_callbacked --;
            apfw_warn("ico_apf_callback_websock: Receive Length Error, Len=%d", len);
            return 0;
        }
        apfw_trace("ico_apf_callback_websock: LWS_CALLBACK_RECEIVE[%s]", msg);

        /* analize event code               */
        if ((handle->type & ICO_APF_COM_TYPE_CONNECTION)
                == ICO_APF_COM_TYPE_SOUNDMGR)   {
            /* Multi Sound Manager          */
            i = 0;
            for (nparam = 0; nparam < 10; nparam++) {
                for (; msg[i]; i++) {
                    if (msg[i] != ' ')  break;
                }
                if (msg[i] == 0)    break;
                param[nparam].start = i;
                for (; msg[i]; i++) {
                    if (msg[i] == ' ')  break;
                }
                param[nparam].end = i;
            }
            if (nparam <= 0)    {
                lws_callbacked --;
                apfw_warn("ico_apf_callback_websock: Illegal Message Format(no param)");
                return 0;
            }
            for (cmd = ICO_APF_SOUND_COMMAND_CMD+1;
                 cmd <= ICO_APF_SOUND_COMMAND_MAX; cmd++)    {
                if (memcmp(&msg[param[0].start],
                           ico_apf_sound_table[cmd-ICO_APF_SOUND_COMMAND_MIN],
                           param[0].end - param[0].start) == 0) break;
            }
            if (cmd > ICO_APF_SOUND_COMMAND_MAX)    {
                lws_callbacked --;
                apfw_warn("ico_apf_callback_websock: Receive Event Error(cmd=%d)", cmd);
                return 0;
            }
            res = ICO_APF_RESID_BASIC_SOUND;
            pid = 0;
            appid[0] = 0;
            strname[0] = 0;
            strlen = 0;
            for (i = 1; i < nparam; i++)    {
                if (memcmp(&msg[param[i].start],
                           "#pid#", param[i].end - param[i].start) == 0)    {
                    i ++;
                    if (i < nparam) {
                        pid = strtol(&msg[param[i].start], (char **)0, 0);
                        if (ico_apf_get_app_id(pid, appid) != ICO_APP_CTL_E_NONE)   {
                            lws_callbacked --;
                            apfw_trace("ico_apf_callback_websock: Unknown pid=%d", pid);
                            return 0;
                        }
                    }
                }
                else if (memcmp(&msg[param[i].start],
                                "#stream_name#", param[i].end - param[i].start) == 0) {
                    i ++;
                    /* no need stream name      */
                }
                else if (memcmp(&msg[param[i].start],
                                "#app_name#", param[i].end - param[i].start) == 0) {
                    i ++;
                    if (i < nparam) {
                        strlen = param[i].end - param[i].start;
                        if (strlen >= ((int)sizeof(strname)))
                            strlen = sizeof(strname) - 1;
                        strncpy(strname, &msg[param[i].start], strlen);
                        strname[strlen] = 0;
                    }
                }
                else if (memcmp(&msg[param[i].start],
                                "#stream_state#", param[i].end - param[i].start) == 0) {
                    i ++;
                    /* no need stream_state     */
                }
            }
            apfw_trace("ico_apf_callback_websock: SoundMgr evt=%d res=%d(%s.%d) str=%s",
                       cmd, res, appid, pid, strname);
            /* save receive message to receive buffer       */
            ico_apf_put_recvmsg(cmd, res, pid, appid, strname, strlen, handle);
        }
        else    {
            /* AppsController               */
            for (cmd = ICO_APF_RESOURCE_COMMAND_MIN;
                 cmd <= ICO_APF_RESOURCE_COMMAND_MAX; cmd++)    {
                if (memcmp(msg,
                           ico_apf_cmd_table[cmd-ICO_APF_RESOURCE_COMMAND_MIN], 3)
                        == 0)   break;
            }
            if (cmd > ICO_APF_RESOURCE_COMMAND_MAX) {
                lws_callbacked --;
                apfw_warn("ico_apf_callback_appsctl: Receive Event Error(cmd=%d)", cmd);
                return 0;
            }
            for (res = ICO_APF_RESOURCE_RESID_MIN;
                 res <= ICO_APF_RESOURCE_RESID_MAX; res++)  {
                if (memcmp(&msg[4],
                    ico_apf_res_table[res-ICO_APF_RESOURCE_RESID_MIN], 4) == 0) break;
            }
            if (res > ICO_APF_RESOURCE_RESID_MAX)   {
                lws_callbacked --;
                apfw_warn("ico_apf_callback_websock: Receive Resource Error(resid=%d)",
                          res);
                return 0;
            }

            pid = 0;
            for (st = 9; msg[st]; st++) {
                if (msg[st] == ' ') break;
                if (pid >= 0)   {
                    if ((msg[st] >= '0') && (msg[st] <= '9'))   {
                        pid = pid * 10 + msg[st] - '0';
                    }
                    else    {
                        pid = -1;
                    }
                }
            }
            if (pid >= 0)   {
                appid[0] = 0;
            }
            else    {
                pid = st - 9;
                if (pid > ICO_UXF_MAX_PROCESS_NAME )
                    pid = ICO_UXF_MAX_PROCESS_NAME;
                strncpy(appid, &msg[9], pid);
                appid[pid] = 0;
                pid = 0;
            }
            if (msg[st] == ' ') st++;
            apfw_trace("ico_apf_callback_websock: AppsCtl evt=%d res=%d pid=%d(%s) msg=%s",
                       cmd, res, pid, appid, &msg[st]);
            /* save receive message to receive buffer       */
            ico_apf_put_recvmsg(cmd, res, pid, appid, &msg[st], len-st, handle);
        }
        break;
    default:
        apfw_trace("ico_apf_callback_websock: UnKnown reason=%d", reason);
        break;
    }
    lws_callbacked --;

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_put_recvmsg
 *          put receive message into receive buffer
 *
 * @param[in]   cmd         receive commnad or event
 * @param[in]   res         receive resource
 * @param[in]   pid         request pid
 * @param[in]   appid       application id (for Web Application)
 * @param[in]   msg         receive message
 * @param[in]   len         receive message length
 * @param[out]  handle      connection handle
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_put_recvmsg(const int cmd, const int res, const int pid, const char *appid,
                    char *msg, const int len, ico_apf_com_handle_t *handle)
{
    int     i;
    int     nexttail;

    /* check receive buffer overflow        */
    if (handle->rtail >= (ICO_APF_RESOURCE_WSOCK_BUFR-1))  {
        nexttail = 0;
    }
    else    {
        nexttail = handle->rtail + 1;
    }
    if (handle->rhead == nexttail)  {
        /* receive buffer overflow, drops all message   */
        apfw_warn("ico_apf_put_recvmsg: receive buffer overflow");
        handle->rhead = 0;
        handle->rtail = 0;
        nexttail = 1;
    }

    /* receive message is put into queue.     */
    handle->rbuf[handle->rtail]->cmd = cmd;
    handle->rbuf[handle->rtail]->res = res;
    handle->rbuf[handle->rtail]->pid = pid;
    strncpy(handle->rbuf[handle->rtail]->appid, appid, ICO_UXF_MAX_PROCESS_NAME);
    handle->rbuf[handle->rtail]->appid[ICO_UXF_MAX_PROCESS_NAME] = 0;
    if (msg)    {
        memcpy(handle->rbuf[handle->rtail]->msg, msg, len);
        handle->rbuf[handle->rtail]->msg[len] = 0;
    }
    else    {
        handle->rbuf[handle->rtail]->msg[0] = 0;
    }
    handle->rtail = nexttail;
    i = handle->rtail - handle->rhead;
    if (i <= 0)    {
        i = ICO_APF_RESOURCE_WSOCK_BUFR + i;
    }
    if ((handle->stoprecv == 0) &&
        (i > ((ICO_APF_RESOURCE_WSOCK_BUFR * 3) / 4)))    {
        /* request to stop sending if an opening is less than 25%.   */
        handle->stoprecv = 1;
        apfw_trace("ico_apf_put_recvmsg: Flow Control(Stop) %d", i);
        libwebsocket_rx_flow_control(handle->wsi_connection, 0);
    }
    ico_apf_poll_write_event(handle, 1);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   apf_com_poll_fd_add: add file descriptor for poll
 *
 * @param[in]   fd          file descriptor
 * @param[in]   flag        POLL flag(POLLIN,POLLOUT)
 *                          negative means reset
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 * @retval      ICO_APF_RESOURCE_E_INVAL            error(out of memory)
 */
/*--------------------------------------------------------------------------*/
static int
ico_apf_poll_fd_add(const int fd, const int flag)
{
    ico_apf_com_poll_t      *p;
    ico_apf_com_poll_t      *fds[1];
    ico_apf_com_handle_t    *handle;

    p = com_polls;
    while (p)   {
        if (p->fd == fd)    {
            break;
        }
        p = p->next;
    }

    if (! p)    {
        /* new file descriptor              */
        if (freecom_polls)   {
            p = freecom_polls;
            freecom_polls = p->next;
            p->apf_fd_control = NULL;
            p->user_data = NULL;
        }
        else    {
            p = calloc(sizeof(ico_apf_com_poll_t), 1);
            if (! p)    {
                /* out of memory            */
                apfw_error("ico_apf_poll_fd_add: can not allocate fd control table"
                           "(No Memory)");
                return ICO_APF_RESOURCE_E_INVAL;
            }
        }

        p->fd = fd;
        p->next = com_polls;
        com_polls = p;
    }
    p->flags |= ICO_APF_COM_POLL_ERROR;
    if (flag >= 0)  {
        if (flag & (POLLIN|POLLPRI))    {
            p->flags |= ICO_APF_COM_POLL_READ;
        }
        if (flag & POLLOUT) {
            p->flags |= ICO_APF_COM_POLL_WRITE;
        }
    }
    else    {
        if ((-flag) & (POLLIN|POLLPRI)) {
            p->flags &= ~ICO_APF_COM_POLL_READ;
        }
        if ((-flag) & POLLOUT)  {
            p->flags &= ~ICO_APF_COM_POLL_WRITE;
        }
    }

    handle = handles;
    while (handle)  {
        if (handle->fd == fd)   break;
        handle = handle->next;
    }
    if (handle) {
        p->apf_fd_control = (void *)handle;
        handle->poll = p;
    }
    else    {
        apfw_trace("ico_apf_poll_fd_add: fd=%d dose not exist in handles", fd);
        p->apf_fd_control = NULL;
    }

    /* regist callback function */
    if (ico_apf_pollfd_func)    {
        fds[0] = p;

        (*ico_apf_pollfd_func)(fds, 1);
    }
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   apf_com_poll_fd_del: delete file descriptor for poll
 *
 * @param[in]   fd          file descriptor
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_poll_fd_del(const int fd)
{
    ico_apf_com_poll_t      *p;
    ico_apf_com_poll_t      *bp;
    ico_apf_com_poll_t      *fds[1];

    /* search file descriptor       */
    p = com_polls;
    bp = NULL;
    while (p)   {
        if (p->fd == fd)    {
            if (bp) {
                bp->next = p->next;
            }
            else    {
                com_polls = p->next;
            }

            if (p->apf_fd_control)  {
                ((ico_apf_com_handle_t *)(p->apf_fd_control))->poll = NULL;
            }

            /* call callback function       */
            if (ico_apf_pollfd_func)    {
                p->flags = 0;
                fds[0] = p;

                (*ico_apf_pollfd_func)(fds, 1);
            }
            p->next = freecom_polls;
            freecom_polls = p;
            break;
        }
        p = p->next;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_poll_fd_control: Set or Unset callback of file descriptors
 *
 * @param[in]   poll_fd_cb      The callback function.
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_com_poll_fd_control(ico_apf_com_pollfd_cb poll_fd_cb)
{
    ico_apf_com_poll_t *p;
    ico_apf_com_poll_t **fds;
    int     nfds;
    int     count;

    /* regist callback function     */
    ico_apf_pollfd_func = poll_fd_cb;

    if (ico_apf_pollfd_func)    {
        /* if regist, call callback for all file descriptors    */

        /* count number of file descriptors */
        p = com_polls;
        count = 0;
        while (p)   {
            count ++;
            p = p->next;
        }
        if (count > 0)  {
            fds = malloc(sizeof(ico_apf_com_poll_t *) * count);
        }
        else    {
            fds = NULL;
        }

        p = com_polls;
        nfds = 0;
        while (fds && p)   {
            fds[nfds++] = p;
            p = p->next;
        }

        apfw_trace("ico_apf_com_poll_fd_control: nfds=%d", nfds);
        if (nfds > 0)   {
            (*ico_apf_pollfd_func)(fds, nfds);
        }
        if (fds)    free(fds);
    }
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_com_poll_fd_event: Notify the changes of file descriptor's state
 *
 * @param[in]   fd_ctl      The structure of file descriptor's controller
 * @param[in]   flags       The user data to be passed to the callback function
 * @return      result
 * @retval      ICO_APF_RESOURCE_E_NONE             success
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_com_poll_fd_event(ico_apf_com_poll_t *fd_ctl, int flags)
{
    int     st;
    ico_apf_com_handle_t    *handle = (ico_apf_com_handle_t *)fd_ctl->apf_fd_control;

    if (handle) {
        ico_apf_com_dispatch(handle, 0);        /* try to service           */
    }
    else    {
        ico_apf_com_dispatch(NULL, 0);          /* try to service for server*/
    }

    if (handle) {
        /* send if send data exist      */
        if ((handle->stail != handle->shead) &&
            (handle->wsi_context != NULL) &&
            (handle->wsi_connection != NULL))   {
            st = handle->shead;
            if (handle->shead >= (ICO_APF_RESOURCE_WSOCK_BUFS-1))  {
                handle->shead = 0;
            }
            else    {
                handle->shead ++;
            }
            if (ico_apf_com_realsend(handle, handle->sbuf[st])
                        != ICO_APF_RESOURCE_E_NONE) {
                apfw_warn("ico_apf_com_poll_fd_event: ico_apf_com_realsend Error");
                handle->shead = st;
                ico_apf_com_dispatch(handle, 10);   /* try to service       */
            }
            if (handle->stail != handle->shead)    {
                libwebsocket_callback_on_writable(handle->wsi_context,
                                                  handle->wsi_connection);
            }
        }

        /* start/stop writable event        */
        if ((handle->stail == handle->shead) ||
            (handle->wsi_context == NULL) ||
            (handle->wsi_connection == NULL))   {
            ico_apf_poll_write_event(handle, 0);
        }
        else    {
            libwebsocket_callback_on_writable(handle->wsi_context,
                                              handle->wsi_connection);
            ico_apf_poll_write_event(handle, 1);
        }
    }
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_apf_poll_write_event: Write event control for poll
 *
 * @param[in]   handle          AppsController's handle
 * @param[in]   write           set/reset write event
 *                              = 0 : reset if not reset
 *                              = 1 : set if not set
 *                              = 2 : force reset
 *                              = 3 : force set
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_poll_write_event(ico_apf_com_handle_t *handle, const int write)
{
    ico_apf_com_poll_t  *fds[1];

    if (handle->poll)   {
        if ((write == 3) || (write == 1)) {
            /* set write poll           */
            handle->poll->flags |= ICO_APF_COM_POLL_WRITE;
            if (ico_apf_pollfd_func)    {
                fds[0] = handle->poll;
                (*ico_apf_pollfd_func)(fds, 1);
            }
        }
        else if ((write == 2) || (write == 0)) {
            /* reset write poll         */
            handle->poll->flags &= ~ICO_APF_COM_POLL_WRITE;
            if (ico_apf_pollfd_func)    {
                fds[0] = handle->poll;
                (*ico_apf_pollfd_func)(fds, 1);
            }
        }
    }
}

