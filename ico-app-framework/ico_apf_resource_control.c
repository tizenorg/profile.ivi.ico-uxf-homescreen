/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprication Framework (Resource Control)
 *
 * @date    Feb-28-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ico_apf_private.h"
#include "ico_uxf_conf.h"

/*  type user callback information          */
typedef struct _resmgr_user_cb_info {
    ico_apf_resource_notify_cb_t    callbacks;
    void                            *user_data;
} resmgr_user_cb_info_t;

/*  communication handle management table   */
typedef struct _remgr_com_handle    {
    struct _remgr_com_handle    *next;
    ico_apf_com_handle_t        *handle;    /* communication handle     */
    int                         pid;        /* client process id        */
    char                        appid[ICO_UXF_MAX_PROCESS_NAME+1];
                                            /* client application id    */
} remgr_com_handle_t;

/*  private variable                */
static ico_apf_com_handle_t     *appsctl_handle = NULL;
static ico_apf_com_handle_t     *soundmgr_handle = NULL;
static resmgr_user_cb_info_t    user_cb_info = {NULL, NULL};
static int                      resmgr_initialized = 0;
static remgr_com_handle_t       *handles = NULL;
static remgr_com_handle_t       *freehandles = NULL;
static Ico_Uxf_Sys_Config       *sysconf = NULL;
static Ico_Uxf_Sys_Config       *ifsysconf = NULL;

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Callback at received from AppsController for client
 *
 *  @param[in]  handle          communication handle
 *  @param[in]  cmd             command(event) code
 *  @param[in]  res             target resource
 *  @param[in]  pid             request client pid (server only)
 *  @param[in]  appid           request client application id (server only)
 *  @param[in]  msg             received message
 *  @param[in]  user_data       (unused)
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_resmgr_client_event(ico_apf_com_handle_t *handle, int cmd, int res,
                            int pid, char *appid, char *msg, void* user_data)
{
    int     i;
    ico_apf_resource_notify_info_t  info;

    apfw_trace("ico_apf_resmgr_client_event: cmd=%d res=%d pid=%d(%s) msg=<%s>",
               cmd, res, pid, appid, msg);

    if (user_cb_info.callbacks != NULL) {
        memset(&info, 0, sizeof(info));

        /* get device and id                */
        for (i = 0; msg[i]; i++)    {
            if( msg[i] == ' ')  break;
            if (i < ICO_UXF_MAX_DEVICE_NAME)    {
                info.device[i] = msg[i];
            }
        }
        if (msg[i] == ' ')  {
            info.id = strtol(&msg[i+1], (char **)0, 0);
        }
        info.state = (ico_apf_resource_state_e)cmd;
        info.resid = res;
        info.pid = pid;
        strncpy(info.appid, appid, ICO_UXF_MAX_PROCESS_NAME);
        user_cb_info.callbacks(&info, user_cb_info.user_data);
    }
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Callback at received from Application for server
 *
 *  @param[in]  handle          communication handle
 *  @param[in]  cmd             command(event) code
 *  @param[in]  res             target resource
 *  @param[in]  pid             request client pid (server only)
 *  @param[in]  appid           request client application id (server only)
 *  @param[in]  msg             received message
 *  @param[in]  user_data       (unused)
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
static void
ico_apf_resmgr_server_event(ico_apf_com_handle_t *handle, int cmd, int res,
                            int pid, char *appid, char *msg, void* user_data)
{
    int     i;
    ico_apf_resource_notify_info_t  info;
    remgr_com_handle_t  *p;
    remgr_com_handle_t  *bp;

    apfw_trace("ico_apf_resmgr_server_event: handle=%08x cmd=%d res=%d pid=%d(%s) msg=<%s>",
               (int)handle, cmd, res, pid, appid, msg);

    /* regist communication handle table    */
    p = handles;
    while (p)   {
        if (p->handle == handle)    break;
        p = p->next;
    }
    if (! p)    {
        apfw_trace("ico_apf_resmgr_server_event: new client");
        if (freehandles)    {
            p = freehandles;
            freehandles = p->next;
        }
        else    {
            p = malloc(sizeof(remgr_com_handle_t));
        }
        if (p)  {
            memset(p, 0, sizeof(remgr_com_handle_t));
            if (! handles)  {
                handles = p;
            }
            else    {
                p->next = handles;
                handles = p;
            }
            p->handle = handle;
            p->pid = pid;
        }
    }

    memset(&info, 0, sizeof(info));

    /* get device and id            */
    for (i = 0; msg[i] ; i++)   {
        if (msg[i] == ' ')  break;
        if (i < ICO_UXF_MAX_DEVICE_NAME)    {
            info.device[i] = msg[i];
        }
    }
    if (msg[i] == ' ')  {
        sscanf(&msg[i+1], "%d %d", &info.id, &info.bid);
    }
    info.state = (ico_apf_resource_state_e)cmd;
    info.resid = res;
    info.pid = pid;
    if (appid[0] )  {
        strncpy(info.appid, appid, ICO_UXF_MAX_PROCESS_NAME);
        strcpy(p->appid, info.appid);
    }
    else if (pid != 0)  {
        info.pid = pid;
        ico_apf_get_app_id(pid, info.appid);
        strcpy(p->appid, info.appid);
    }

    if (info.state == ICO_APF_RESOURCE_STATE_CONNECTED) {
        /* regist communication handle table    */
        p = handles;
        while (p)   {
            if (p->handle == handle)    break;
            p = p->next;
        }
        if (! p)    {
            if (freehandles)    {
                p = freehandles;
                freehandles = p->next;
            }
            else    {
                p = malloc(sizeof(remgr_com_handle_t));
            }
            if (p)  {
                memset(p, 0, sizeof(remgr_com_handle_t));
                if (! handles)  {
                    handles = p;
                }
                else    {
                    p->next = handles;
                    handles = p;
                }
                p->handle = handle;
                p->pid = pid;
                strncpy(p->appid, info.appid, ICO_UXF_MAX_PROCESS_NAME);
            }
        }
    }
    else if (info.state == ICO_APF_RESOURCE_STATE_DISCONNECTED) {
        /* destory communiction handle table    */
        p = handles;
        bp = NULL;
        while (p)   {
            if (p->handle == handle)    break;
            bp = p;
            p = p->next;
        }
        if (p)  {
            if (bp) {
                bp->next = p->next;
            }
            else    {
                handles = p->next;
            }
            p->next = freehandles;
            freehandles = p;
        }
    }
    if (user_cb_info.callbacks != NULL) {
        user_cb_info.callbacks(&info, user_cb_info.user_data);
    }
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize resouce manager component for client Application
 *
 *  @param[in]  uri         server URI
 *  @return result status
 *  @retval ICO_APF_RESOURCE_E_NONE                 success
 *  @retval ICO_APF_RESOURCE_E_INIT_COM_FAILD       can not create connection
 *  @retval ICO_APF_RESOURCE_E_INIT_COMMUNICATION   initialize communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_init_client(const char *uri)
{
    apfw_trace("ico_apf_resource_init_client: Enter(%s)", uri ? uri : "NULL");

    if (resmgr_initialized != 0) {
        apfw_warn("ico_apf_resource_init_client: Leave(already intialiezed)");
        return ICO_APF_RESOURCE_E_NONE;
    }

    /* initialize connection for AppsController */
    appsctl_handle = ico_apf_com_init_client(uri, ICO_APF_COM_TYPE_APPSCTL);

    if (! appsctl_handle)   {
        apfw_error("ico_apf_resource_init_client: Leave(can not create connection)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    /* get system configuration                 */
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
        apfw_error("ico_apf_resource_init_client: Leave(can not read configuration files)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    if (ico_apf_com_addeventlistener(NULL, ico_apf_resmgr_client_event, NULL)
            != ICO_APF_E_NONE) {
        apfw_error("ico_apf_resource_init_client: Leave(can not set callback)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    /* chanage resmgr_client_intialized flag */
    resmgr_initialized = 1;

    apfw_trace("ico_apf_resource_init_client: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame resouce manager component
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_resource_term_client(void)
{
    apfw_trace("ico_apf_resource_term_client: Enter");

    /* unset callback functions */
    ico_apf_resource_unset_event_cb();

    /* terminate resourece server communication */
    (void) ico_apf_com_term_client(appsctl_handle);
    appsctl_handle = NULL;

    /* close system configuration   */
    if (! ifsysconf)    {
        ico_uxf_closeSysConfig();
    }

    /* chanage resmgr_client_intialized flag */
    resmgr_initialized = 0;

    apfw_trace("ico_apf_resource_term_client: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Initialize resouce manager component for server Application
 *
 *  @param[in]  uri         my URI
 *  @return result status
 *  @retval ICO_APF_RESOURCE_E_NONE                 success
 *  @retval ICO_APF_RESOURCE_E_INIT_COM_FAILD       can not create connection
 *  @retval ICO_APF_RESOURCE_E_INIT_COMMUNICATION   initialize communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_init_server(const char *uri)
{
    apfw_trace("ico_apf_resource_init_server: Enter(%s)", uri ? uri : "NULL");

    if (resmgr_initialized != 0) {
        apfw_warn("ico_apf_resource_init_server: Leave(already intialiezed)");
        return ICO_APF_RESOURCE_E_NONE;
    }

    /* initialize connection for AppsController */
    appsctl_handle = ico_apf_com_init_server(uri, ICO_APF_COM_TYPE_APPSCTL);
    if (! appsctl_handle)   {
        apfw_error("ico_apf_resource_init_server: Leave(can not create connection)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    /* initialize connection for Multi Input Manager    */
    soundmgr_handle = ico_apf_com_init_client(NULL, ICO_APF_COM_TYPE_SOUNDMGR);
    if (! soundmgr_handle)  {
        apfw_error("ico_apf_resource_init_server: Leave(can not connect MSM)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    /* get system configuration                 */
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
        apfw_error("ico_apf_resource_init_server: Leave(can not read configuration files)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    if (ico_apf_com_addeventlistener(NULL, ico_apf_resmgr_server_event, NULL)
            != ICO_APF_E_NONE) {
        apfw_error("ico_apf_resource_init_server: Leave(can not set callback)");
        return ICO_APF_RESOURCE_E_INIT_COM_FAILD;
    }

    /* chanage resmgr_client_intialized flag */
    resmgr_initialized = 1;

    apfw_trace("ico_apf_resource_init_server: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Terminame resouce manager component
 *
 *  @param      none
 *  @return     none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_apf_resource_term_server(void)
{
    apfw_trace("ico_apf_resource_term_server: Enter");

    /* unset callback functions */
    ico_apf_resource_unset_event_cb();

    /* terminate resourece server communication */
    (void) ico_apf_com_term_server(appsctl_handle);
    appsctl_handle = NULL;

    /* terminate Multi Sound Manager client communication */
    (void) ico_apf_com_term_client(soundmgr_handle);
    soundmgr_handle = NULL;

    /* close system configuration   */
    if (! ifsysconf)    {
        ico_uxf_closeSysConfig();
    }

    /* chanage resmgr_client_intialized flag */
    resmgr_initialized = 0;

    apfw_trace("ico_apf_resource_term_server: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Set event callback function
 *
 *  @param[in]  callbacks   callback functions
 *  @param[in]  user_data   passed data on called callback function
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE     success
 *  @retval     ICO_APF_RESOURCE_E_INVAL    callbacks is null
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_set_event_cb(ico_apf_resource_notify_cb_t callbacks,
                              void *user_data)
{
    apfw_trace("ico_apf_resource_set_event_cb: Enter(0x%08x, 0x%08x)",
              (int)callbacks, (int)user_data);

    if (callbacks == NULL) {
        apfw_error("ico_apf_resource_set_event_cb: Leave(callbacks is NULL)");
        return ICO_APF_RESOURCE_E_INVAL;
    }
    user_cb_info.callbacks = callbacks;
    user_cb_info.user_data = user_data;

    apfw_trace("ico_apf_resource_set_event_cb: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Unset event callback function
 *
 *  @param      none
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE     success(At present, always give back this)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_unset_event_cb(void)
{
    apfw_trace("ico_apf_resource_unset_event_cb:");

    user_cb_info.callbacks = NULL;
    user_cb_info.user_data = NULL;

    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_get_screen_mode(const char* disp_dev, int scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_get_screen_mode: Enter(%s,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_get_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_GET,
                         ICO_APF_RESID_BASIC_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_get_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_get_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_release_screen_mode(const char* disp_dev, int scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_release_screen_mode: Enter(%s,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_release_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_RELEASE,
                         ICO_APF_RESID_BASIC_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_release_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_release_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_reply_screen_mode(const char* disp_dev, int scr_id, const int ok)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_reply_screen_mode: Enter(%s,%d,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id, ok);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_reply_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle,
                         (ok != 0) ? ICO_APF_RESOURCE_REPLY_OK
                                   : ICO_APF_RESOURCE_REPLY_NG,
                         ICO_APF_RESID_BASIC_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_reply_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_reply_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_src_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_get_int_screen_mode(const char* disp_dev,
                                     int scr_id, int int_scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_get_int_screen_mode: Enter(%s,%d,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id, int_scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_get_int_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 disp_dev, int_scr_id, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name,
                 int_scr_id, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_GET,
                         ICO_APF_RESID_INT_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_get_int_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_get_int_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen on basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_release_int_screen_mode(const char* disp_dev,
                                         int scr_id, int int_scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_release_int_screen_mode: Enter(%s,%d,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id, int_scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_release_int_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 disp_dev, int_scr_id, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name,
                 int_scr_id, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_RELEASE,
                         ICO_APF_RESID_INT_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_release_int_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_release_int_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt screen on basic screen
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  src_id      id of basic screen
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_reply_int_screen_mode(const char* disp_dev,
                                       int scr_id, int int_scr_id, const int ok)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_reply_int_screen_mode: Enter(%s,%d,%d,%d)",
               disp_dev ? disp_dev : "(NULL)", scr_id, int_scr_id, ok);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_reply_int_screen_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 disp_dev, int_scr_id, scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name,
                 int_scr_id, scr_id);
    }
    if (ico_apf_com_send(appsctl_handle,
                         (ok != 0) ? ICO_APF_RESOURCE_REPLY_OK
                                   : ICO_APF_RESOURCE_REPLY_NG,
                         ICO_APF_RESID_INT_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_reply_int_screen_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_reply_int_screen_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_get_int_screen_mode_disp(const char* disp_dev, int int_scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_get_int_screen_mode_disp: Enter(%s,%d)",
               disp_dev ? disp_dev : "(NULL)", int_scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_get_int_screen_mode_disp: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, int_scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, int_scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_GET,
                         ICO_APF_RESID_ON_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_get_int_screen_mode_disp: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_get_int_screen_mode_disp: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_release_int_screen_mode_disp(const char* disp_dev,
                                              int int_scr_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_release_int_screen_mode_disp: Enter(%s,%d)",
               disp_dev ? disp_dev : "(NULL)", int_scr_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_release_int_screen_mode_disp: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, int_scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, int_scr_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_RELEASE,
                         ICO_APF_RESID_ON_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_release_int_screen_mode_disp: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_release_int_screen_mode_disp: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt screen on display
 *
 *  @param[in]  disp_dev    display device uri
 *  @param[in]  int_scr_id  id of interrupt screen
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_reply_int_screen_mode_disp(const char* disp_dev,
                                            int int_scr_id, const int ok)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_reply_int_screen_mode_disp: Enter(%s,%d,%d)",
               disp_dev ? disp_dev : "(NULL)", int_scr_id, ok);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_reply_int_screen_mode_disp: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (disp_dev)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", disp_dev, int_scr_id);
    }
    else    {
        /* no display URI, default display  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->display[sysconf->misc.default_displayId].
                     zone[sysconf->misc.default_dispzoneId].name, int_scr_id);
    }
    if (ico_apf_com_send(appsctl_handle,
                         (ok != 0) ? ICO_APF_RESOURCE_REPLY_OK
                                   : ICO_APF_RESOURCE_REPLY_NG,
                         ICO_APF_RESID_ON_SCREEN, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_reply_int_screen_mode_disp: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_reply_int_screen_mode_disp: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @param[in]  adjust      adjust acction
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_get_sound_mode(const char* zone, int snd_id, int adjust)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_get_sound_mode: Enter(%s,%d,%d)",
               zone ? zone : "(NULL)", snd_id, adjust);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_get_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d", zone, snd_id, adjust);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name,
                 snd_id, adjust);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_GET,
                         ICO_APF_RESID_BASIC_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_get_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_get_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_release_sound_mode(const char* zone, int snd_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_release_sound_mode: Enter(%s,%d)",
               zone ? zone : "(NULL)", snd_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_release_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", zone, snd_id);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name, snd_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_RELEASE,
                         ICO_APF_RESID_BASIC_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_release_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_release_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of basic sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  snd_id      id of basic sound
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_reply_sound_mode(const char* zone, int snd_id, const int ok)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_reply_sound_mode: Enter(%s,%d,%d)",
               zone ? zone : "(NULL)", snd_id, ok);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_reply_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", zone, snd_id);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name, snd_id);
    }
    if (ico_apf_com_send(appsctl_handle,
                         (ok != 0) ? ICO_APF_RESOURCE_REPLY_OK
                                   : ICO_APF_RESOURCE_REPLY_NG,
                         ICO_APF_RESID_BASIC_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_reply_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_reply_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Get the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of basic sound
 *  @param[in]  adjust      adjust action
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_get_int_sound_mode(const char* zone,
                                    int int_snd_id, int adjust)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_get_int_sound_mode: Enter(%s,%d,%d)",
               zone ? zone : "(NULL)", int_snd_id, adjust);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_get_int_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d", zone, int_snd_id, adjust);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name,
                 int_snd_id, adjust);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_GET,
                         ICO_APF_RESID_INT_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_get_int_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_get_int_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Release the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of interrupt sound
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_release_int_sound_mode(const char* zone, int int_snd_id)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_release_int_sound_mode: Enter(%s,%d)",
               zone ? zone : "(NULL)", int_snd_id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_release_int_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", zone, int_snd_id);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name, int_snd_id);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_RELEASE,
                         ICO_APF_RESID_INT_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_release_int_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_release_int_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Reply the rights of interrupt sound on zone
 *
 *  @param[in]  zone        sound device uri
 *  @param[in]  int_snd_id  id of interrupt sound
 *  @param[in]  ok          OK(1) or NG(0)
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_reply_int_sound_mode(const char* zone, int int_snd_id, const int ok)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_reply_int_sound_mode: Enter(%s,%d,%d)",
               zone ? zone : "(NULL)", int_snd_id, ok);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_reply_int_sound_mode: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (zone)   {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", zone, int_snd_id);
    }
    else    {
        /* no zone, default zone    */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->sound[sysconf->misc.default_soundId].
                     zone[sysconf->misc.default_soundzoneId].name, int_snd_id);
    }
    if (ico_apf_com_send(appsctl_handle,
                         (ok != 0) ? ICO_APF_RESOURCE_REPLY_OK
                                   : ICO_APF_RESOURCE_REPLY_NG,
                         ICO_APF_RESID_INT_SOUND, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("ico_apf_resource_reply_int_sound_mode: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_reply_int_sound_mode: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Add the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_add_input_event(const char* input_dev, int events)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_add_input_event: Enter(%s,%d)",
               input_dev ? input_dev : "(NULL)", events);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_add_input_event: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (input_dev)  {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", input_dev, events);
    }
    else    {
        /* no input device, default device  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->inputdev[sysconf->misc.default_inputdevId].name, events);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_ADD,
                         ICO_APF_RESID_INPUT_DEV, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_add_input_event: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_add_input_event: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Change the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_change_input_event(const char* input_dev, int events)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_change_input_event: Enter(%s,%d)",
               input_dev ? input_dev : "(NULL)", events);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_change_input_event: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (input_dev)  {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", input_dev, events);
    }
    else    {
        /* no input device, default device  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->inputdev[sysconf->misc.default_inputdevId].name, events);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_CHANGE,
                         ICO_APF_RESID_INPUT_DEV, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_change_input_event: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_change_input_event: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Delete the input event notification from input device
 *
 *  @param[in]  input_dev   input device uri
 *  @param[in]  events      notify input events
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE               success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED    not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION      communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_delete_input_event(const char* input_dev, int events)
{
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_delete_input_event: Enter(%s,%d)",
               input_dev ? input_dev : "(NULL)", events);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_delete_input_event: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* set send message parameter */
    if (input_dev)  {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", input_dev, events);
    }
    else    {
        /* no input device, default device  */
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                 sysconf->inputdev[sysconf->misc.default_inputdevId].name, events);
    }
    if (ico_apf_com_send(appsctl_handle, ICO_APF_RESOURCE_COMMAND_DELETE,
                         ICO_APF_RESID_INPUT_DEV, NULL, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_delete_input_event: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_delete_input_event: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Send responce form server(AppsController) to client application
 *
 *  @param[in]  appid       client application id
 *  @param[in]  event       event
 *  @param[in]  resource    target resource
 *  @param[in]  device      display device / sound zone
 *  @param[in]  id          application defined object id
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE             success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED  not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION    communication error
 *  @retval     ICO_APF_RESOURCE_E_INVAL            illegal appid or event code
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_send_to_client(const char *appid, const int event,
                                const int resource, const char *device, const int id)
{
    remgr_com_handle_t  *p;
    char    msg[ICO_APF_RESOURCE_MSG_LEN];

    apfw_trace("ico_apf_resource_send_to_client: Enter(%s,%d,%d,%s,%d)",
               appid, event, resource, device ? device : "(NULL)", id);

    if ((! resmgr_initialized) || (! appsctl_handle))   {
        apfw_error("ico_apf_resource_send_to_client: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    /* search client application    */
    p = handles;
    while (p)   {
        if (strcmp(p->appid, appid) == 0)   break;
        p = p->next;
    }
    if (! p)    {
        apfw_error("ico_apf_resource_send_to_client: Leave(appid not exist)");
        return ICO_APF_RESOURCE_E_INVAL;
    }

    /* set send message parameter */
    if (device) {
        snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d", device, id);
    }
    else    {
        /* no display URI, default display  */
        if ((resource == ICO_APF_RESID_BASIC_SCREEN) ||
            (resource == ICO_APF_RESID_INT_SCREEN) ||
            (resource == ICO_APF_RESID_ON_SCREEN))  {
            snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                     sysconf->display[sysconf->misc.default_displayId].
                         zone[sysconf->misc.default_dispzoneId].name, id);
        }
        else if (resource == ICO_APF_RESID_INPUT_DEV)   {
            snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                     sysconf->inputdev[sysconf->misc.default_inputdevId].name, id);
        }
        else    {
            snprintf(msg, ICO_APF_RESOURCE_MSG_LEN-1, "%s %d",
                     sysconf->sound[sysconf->misc.default_soundId].
                         zone[sysconf->misc.default_soundzoneId].name, id);
        }
    }
    if (ico_apf_com_send(p->handle, event, resource, appid, msg)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_send_to_client: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }

    apfw_trace("ico_apf_resource_send_to_client: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

/*--------------------------------------------------------------------------*/
/**
 *  @brief  Send request to Multi Sound Manager
 *
 *  @param[in]  cmd         send command
 *  @param[in]  pid         target pid
 *  @return     result status
 *  @retval     ICO_APF_RESOURCE_E_NONE             success
 *  @retval     ICO_APF_RESOURCE_E_NOT_INITIALIZED  not initialized
 *  @retval     ICO_APF_RESOURCE_E_COMMUNICATION    communication error
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_apf_resource_send_to_soundctl(const ico_apf_sound_state_e cmd, const int pid)
{
    apfw_trace("ico_apf_resource_send_to_soundctl: Enter(%s[%d],%d)",
               cmd == ICO_APF_SOUND_COMMAND_MUTEON ? "Stop" :
                   (cmd == ICO_APF_SOUND_COMMAND_MUTEOFF ? "Start" : "\0"), (int)cmd, pid);

    if ((! resmgr_initialized) || (! soundmgr_handle))   {
        apfw_error("ico_apf_resource_send_to_soundctl: Leave(not initialized)");
        return ICO_APF_RESOURCE_E_NOT_INITIALIZED;
    }

    if (ico_apf_com_send(soundmgr_handle, cmd, pid, NULL, NULL)
            != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_apf_resource_send_to_soundctl: Leave(commnad send error)");
        return ICO_APF_RESOURCE_E_COMMUNICATION;
    }
    apfw_trace("ico_apf_resource_send_to_soundctl: Leave(OK)");
    return ICO_APF_RESOURCE_E_NONE;
}

