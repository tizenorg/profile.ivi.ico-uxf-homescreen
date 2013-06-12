/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Initialize/Terminate and Control for Apps Controller of System Controller
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <stdbool.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <sys/ioctl.h>
#include    <sys/time.h>
#include    <sys/resource.h>
#include    <fcntl.h>

#include    <wayland-client.h>
#include    <ico_window_mgr-client-protocol.h>
#include    <dbus/dbus.h>
#include    <Ecore.h>
#include    "ico_apf.h"
#include    "ico_uxf.h"
#include    "ico_apf_ecore.h"
#include    "ico_syc_apc.h"
#include    "ico_syc_apc_private.h"

/*==============================================================================*/
/* static tables                                                                */
/*==============================================================================*/
/* flag for initiailze                  */
static int  ico_apps_controller_init = 0;

/* zone management table                */
static ico_apc_dispzone_t       *dispzone = NULL;
static int                      ndispzone = 0;
static ico_apc_soundzone_t      *soundzone = NULL;
static int                      nsoundzone = 0;
static ico_apc_inputsw_t        *inputsw = NULL;
static int                      ninputsw = 0;

/* free request save table              */
static ico_apc_request_t        *free_request = NULL;

/* user(HomeScreen) control functions   */
static ico_apc_resource_control_t   displaycontrol = NULL;
static ico_apc_resource_control_t   soundcontrol = NULL;
static ico_apc_resource_control_t   inputcontrol = NULL;

/* Ecore timer                          */
static int  timer_count = 0;
static Ecore_Timer *ecore_timer = NULL;

/* configuration                        */
static Ico_Uxf_Sys_Config       *confsys = NULL;
static Ico_Uxf_App_Config       *confapp = NULL;

/*==============================================================================*/
/* define static function prototype                                             */
/*==============================================================================*/
static void resource_reqcb(ico_apf_resource_notify_info_t* info, void *user_data);
static void app_getdisplay(ico_apc_request_t *req, const int addprio);
static void app_freedisplay(ico_apc_request_t *req, const int send);
static void change_disprequest(ico_apc_request_t *req, const int active);
static void recalc_dispzone(const int idx);
static void app_getsound(ico_apc_request_t *req, const int addprio);
static void app_freesound(ico_apc_request_t *req, const int send);
static void change_soundrequest(ico_apc_request_t *req, const int active);
static void recalc_soundzone(const int idx);
static void app_getinput(ico_apc_request_t *req, const int addprio);
static void app_freeinput(ico_apc_request_t *req, const int send);
static void change_inputrequest(ico_apc_request_t *req, const int active);
static void recalc_inputsw(const int idx);
static void regulation_listener(const int appcategory,
                                const ico_apc_reguration_control_t control,
                                void *user_data);
static Eina_Bool request_timer(void *user_data);
static void app_display_hook(const char *appid, const int surface, const int object);
static ico_apc_request_t *get_freereq(void);
static ico_apc_request_t *search_disprequest(const Ico_Uxf_conf_application *conf,
                                             const int resid, const int id);
static ico_apc_request_t *search_soundrequest(const Ico_Uxf_conf_application *conf,
                                              const int resid, const int id,
                                              ico_apc_request_t **first_req);

/*==============================================================================*/
/* define fixed value                                                           */
/*==============================================================================*/
/* initiale allocate request blocks     */
#define INIT_REQCB  50

/* maximum number of request blocks     */
#if ICO_UXF_DISPLAY_ZONE_MAX > ICO_UXF_SOUND_ZONE_MAX
  #if ICO_UXF_INPUT_SW_MAX > ICO_UXF_DISPLAY_ZONE_MAX
    #define MAXREQ  ICO_UXF_INPUT_SW_MAX
  #else
    #define MAXREQ  ICO_UXF_DISPLAY_ZONE_MAX
  #endif
#else
  #if ICO_UXF_INPUT_SW_MAX > ICO_UXF_SOUND_ZONE_MAX
    #define MAXREQ  ICO_UXF_INPUT_SW_MAX
  #else
    #define MAXREQ  ICO_UXF_SOUND_ZONE_MAX
  #endif
#endif

/*--------------------------------------------------------------------------*/
/**
 * @brief   get_appconf: application configure(static function)
 *
 * @param       appid           application id
 * @return      result
 * @retval      != NULL         success(request block address)
 * @retval      == NULL         error(out of memory)
 */
/*--------------------------------------------------------------------------*/
static Ico_Uxf_conf_application *
get_appconf(const char *appid)
{
    Ico_Uxf_conf_application *appconf = NULL;

    appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByAppid(appid);
    if (! appconf)  {
        /* application id dose not exist, search application name   */
        appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByName(appid);
    }

    return appconf;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   get_freereq: get free request block(static function)
 *
 * @param       none
 * @return      result
 * @retval      != NULL         success(request block address)
 * @retval      == NULL         error(out of memory)
 */
/*--------------------------------------------------------------------------*/
static ico_apc_request_t *
get_freereq(void)
{
    ico_apc_request_t   *req;
    int     i;

    if (! free_request)   {
        free_request = malloc(sizeof(ico_apc_request_t)*ICO_UXF_MNG_BLOCK_ALLOCS);
        if (! free_request)  {
            apfw_error("get_freereq: No Memory");
            return NULL;
        }
        req = free_request;
        for (i = 0; i < ICO_UXF_MNG_BLOCK_ALLOCS; i++, req++)   {
            if (i < (ICO_UXF_MNG_BLOCK_ALLOCS-1))   {
                req->next = (req+1);
            }
            else    {
                req->next = NULL;
            }
        }
    }
    req = free_request;
    memset(req, 0, sizeof(ico_apc_request_t));
    free_request = free_request->next;

    return req;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   search_disprequest: search client display request(static function)
 *
 * @param[in]   conf            application configuration
 * @param[in]   resid           resource type
 * @param[in]   id              resource id requested from client
 * @return      result
 * @retval      != NULL         success(request block)
 * @retval      == NULL         error(request was not found)
 */
/*--------------------------------------------------------------------------*/
static ico_apc_request_t *
search_disprequest(const Ico_Uxf_conf_application *conf, const int resid, const int id)
{
    int                 i;
    ico_apc_request_t   *p;
    ico_apc_request_t   *sameres = NULL;

    for (i = 0; i < ndispzone; i++) {
        p = dispzone[i].req;
        while (p)   {
            if ((strcmp(p->appid, conf->appid) == 0) &&
                ((int)p->resid == resid))    {
                if ((id < 0) || (p->id == id))  {
                    return p;
                }
                if (! sameres)  sameres = p;
            }
            p = p->next;
        }
    }
    return sameres;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   search_soundrequest: search client sound request(static function)
 *
 * @param[in]   conf            application configuration
 * @param[in]   resid           resource type
 * @param[in]   id              resource id requested from client
 * @param[out]  first_req       request of top on same sound zone
 * @return      result
 * @retval      != NULL         success(request block)
 * @retval      == NULL         error(request was not found)
 */
/*--------------------------------------------------------------------------*/
static ico_apc_request_t *
search_soundrequest(const Ico_Uxf_conf_application *conf, const int resid,
                    const int id, ico_apc_request_t **first_req)
{
    int                 i;
    ico_apc_request_t   *p;
    ico_apc_request_t   *sameres = NULL;

    for (i = 0; i < nsoundzone; i++) {
        p = soundzone[i].req;
        while (p)   {
            if ((strcmp(p->appid, conf->appid) == 0) &&
                ((int)p->resid == resid))    {
                if ((id < 0) || (p->id == id))  {
                    if (first_req)  {
                        *first_req = soundzone[i].req;
                    }
                    return p;
                }
                if (! sameres)  sameres = p;
            }
            p = p->next;
        }
    }
    return sameres;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   resource_reqcb: resource request callback(static function)
 *
 * @param[in]   info            request information
 * @param[in]   user_data       user data(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
resource_reqcb(ico_apf_resource_notify_info_t* info, void *user_data)
{
    Ico_Uxf_conf_application    *appconf;
    ico_apc_request_t           *req;
    ico_apc_request_t           *p;
    ico_apc_request_t           *freq;
    int                         i;
    int                         count;
    ico_apc_request_t           *reqsave[MAXREQ];

    apfw_trace("resource_reqcb: Entry(cmd=%d res=%d id=%d bid=%d app=%s[%d])",
               info->state, info->resid, info->id, info->bid, info->appid, info->pid);

    if (info->state == ICO_APF_RESOURCE_STATE_CONNECTED)    {
        apfw_trace("resource_reqcb: Leave(application connected)");
        return;
    }

    if (info->state >= ICO_APF_SOUND_COMMAND_MIN)   {
        /* Multi Sound Manager, data check          */
        if ((int)info->state == (int)ICO_APF_SOUND_REPLY_LIST)  {
            apfw_trace("resource_reqcb: Leave(Sound Reply, no need)");
            return;
        }
        if (info->appid[0] == 0)    {
            apfw_warn("resource_reqcb: Leave(Sound Event, but no appid)");
            return;
        }
        if ((int)info->state == (int)ICO_APF_SOUND_EVENT_FREE)  {
            apfw_trace("resource_reqcb: Leave(Sound Free, no need)");
            return;
        }
    }

    /* check and search application id      */
    appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByAppid(info->appid);
    if (! appconf)  {
        /* application id dose not exist, search application name   */
        appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByName(info->appid);
        if (! appconf)  {
            apfw_error("resource_reqcb: Leave(appid[%s] dose not exist)",
                       info->appid);
            return;
        }
    }
    if ((info->state >= ICO_APF_SOUND_COMMAND_MIN) &&
        ((confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_ALMIGHTY) ||
         (confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_SYSTEM) ||
         (confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_SYSTEM_AUDIO)))  {
        /* System Program(ex. HomeScreen) no need resource control  */
        apfw_trace("resource_reqcb: Leave(appid[%s] is system program)",
                   info->appid);
        return;
    }

    if (info->state == ICO_APF_RESOURCE_STATE_DISCONNECTED) {
        apfw_trace("resource_reqcb: DISCONNECTED[%s]", info->appid);

        /* free all screen request from this application    */
        count = 0;
        for (i = 0; i < ndispzone; i++) {
            p = dispzone[i].req;
            while (p)   {
                if (strcmp(p->appid, appconf->appid) == 0) {
                    reqsave[count++] = p;
                }
                p = p->next;
            }
        }
        for (i = 0; i < count; i++) {
            app_freedisplay(reqsave[i], 0);
        }
        /* free all sound request from this application     */
        count = 0;
        for (i = 0; i < nsoundzone; i++)    {
            p = soundzone[i].req;
            while (p)   {
                if (strcmp(p->appid, appconf->appid) == 0) {
                    reqsave[count++] = p;
                }
                p = p->next;
            }
        }
        for (i = 0; i < count; i++) {
            app_freesound(reqsave[i], 0);
        }
        /* free all input switch request from this application*/
        count = 0;
        for (i = 0; i < ninputsw; i++)  {
            p = inputsw[i].req;
            while (p)   {
                if (strcmp(p->appid, appconf->appid) == 0) {
                    reqsave[count++] = p;
                }
                p = p->next;
            }
        }
        for (i = 0; i < count; i++) {
            app_freeinput(reqsave[i], 0);
        }
        apfw_trace("resource_reqcb: Leave");
        return;
    }

    req = get_freereq();
    if (! req)  {
        return;
    }
    strncpy(req->appid, appconf->appid, ICO_UXF_MAX_PROCESS_NAME);
    req->resid = info->resid;
    if (info->device[0])    {
        strcpy(req->device, info->device);
    }
    else    {
        strcpy(req->device, "UnKnown");
    }
    req->id = info->id;
    req->bid = info->bid;
    req->pid = info->pid;
    req->reqtype = ICO_APC_REQTYPE_REQUEST;

    switch (info->resid)    {
    case ICO_APF_RESID_BASIC_SCREEN:                /* basic screen             */
        switch (info->state)    {
        case ICO_APF_RESOURCE_COMMAND_GET:          /* get resource             */
            apfw_trace("resource_reqcb: app(%s) get BasicScreen(%d)",
                       info->appid, info->id);
            app_getdisplay(req, 0);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_COMMAND_RELEASE:      /* release resource         */
            apfw_trace("resource_reqcb: app(%s) release BasicScreen(%d)",
                       info->appid, info->id);
            app_freedisplay(req, 1);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_REPLY_OK:             /* ack reply                */
        case ICO_APF_RESOURCE_REPLY_NG:             /* nak reply                */
            apfw_trace("resource_reqcb: app(%s) BasicScreen(%d) reply(%s)", info->appid,
                       info->id, info->state == ICO_APF_RESOURCE_REPLY_OK ? "OK" : "NG");
            p = search_disprequest(appconf, ICO_APF_RESID_BASIC_SCREEN, info->id);
            if (p)  {
                if (p->state & ICO_APC_REQSTATE_REPLYACTIVE)    {
                    ico_uxf_window_control(appconf->appid, p->id,
                                           ICO_UXF_APPSCTL_INVISIBLE, 0);
                    if (displaycontrol) {
                        (*displaycontrol)(appconf, 1);
                    }
                }
                p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                p->timer = 0;
            }
            break;
        default:
            apfw_error("resource_reqcb: illegal command(%d)", info->state);
            break;
        }
        break;
    case ICO_APF_RESID_INT_SCREEN:                  /* interrupt screen         */
        switch (info->state)    {
        case ICO_APF_RESOURCE_COMMAND_GET:          /* get resource             */
            apfw_trace("resource_reqcb: app(%s) get IntScreen(%d) on basic screen(%d)",
                       info->appid, info->id, info->bid);
            app_getdisplay(req, ICO_UXF_PRIO_INTSCREEN);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_COMMAND_RELEASE:      /* release resource         */
            apfw_trace("resource_reqcb: app(%s) release IntScreen(%d)",
                       info->appid, info->id);
            app_freedisplay(req, 1);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_REPLY_OK:             /* ack reply                */
        case ICO_APF_RESOURCE_REPLY_NG:             /* nak reply                */
            apfw_trace("resource_reqcb: app(%s) IntScreen(%d) reply(%s)", info->appid,
                       info->id, info->state == ICO_APF_RESOURCE_REPLY_OK ? "OK" : "NG");
            p = search_disprequest(appconf, ICO_APF_RESID_INT_SCREEN, info->id);
            if (p)  {
                if (p->state & ICO_APC_REQSTATE_REPLYACTIVE)    {
                    ico_uxf_window_control(appconf->appid, p->id,
                                           ICO_UXF_APPSCTL_INVISIBLE, 0);
                    if (displaycontrol) {
                        (*displaycontrol)(appconf, 1);
                    }
                }
                p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                p->timer = 0;
            }
            break;
        default:
            apfw_error("resource_reqcb: illegal command(%d)", info->state);
            break;
        }
        break;
    case ICO_APF_RESID_ON_SCREEN:                   /* on screen                */
        switch (info->state)    {
        case ICO_APF_RESOURCE_COMMAND_GET:          /* get resource             */
            apfw_trace("resource_reqcb: app(%s) get OnScreen(%d)",
                       info->appid, info->id);
            app_getdisplay(req, ICO_UXF_PRIO_ONSCREEN);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_COMMAND_RELEASE:      /* release resource         */
            apfw_trace("resource_reqcb: app(%s) release OnScreen(%d)",
                       info->appid, info->id);
            app_freedisplay(req, 1);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_REPLY_OK:             /* ack reply                */
        case ICO_APF_RESOURCE_REPLY_NG:             /* nak reply                */
            apfw_trace("resource_reqcb: app(%s) OnScreen(%d) reply(%s)", info->appid,
                       info->id, info->state == ICO_APF_RESOURCE_REPLY_OK ? "OK" : "NG");
            p = search_disprequest(appconf, ICO_APF_RESID_ON_SCREEN, info->id);
            if (p)  {
                if (p->state & ICO_APC_REQSTATE_REPLYACTIVE)    {
                    ico_uxf_window_control(appconf->appid, p->id,
                                           ICO_UXF_APPSCTL_INVISIBLE, 0);
                    if (displaycontrol) {
                        (*displaycontrol)(appconf, 1);
                    }
                }
                p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                p->timer = 0;
            }
            break;
        default:
            apfw_error("resource_reqcb: illegal command(%d)", info->state);
            break;
        }
        break;
    case ICO_APF_RESID_BASIC_SOUND:                 /* basic sound              */
        switch ((int)info->state)    {
        case ICO_APF_RESOURCE_COMMAND_GET:          /* get resource             */
            apfw_trace("resource_reqcb: app(%s) get BasicSound(%d)",
                       info->appid, info->id);
            app_getsound(req, 0);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_COMMAND_RELEASE:      /* release resource         */
            apfw_trace("resource_reqcb: app(%s) release BasicSound(%d)",
                       info->appid, info->id);
            app_freesound(req, 1);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_REPLY_OK:             /* ack reply                */
        case ICO_APF_RESOURCE_REPLY_NG:             /* nak reply                */
            apfw_trace("resource_reqcb: app(%s) BasicSound(%d) reply(%s)", info->appid,
                       info->id, info->state == ICO_APF_RESOURCE_REPLY_OK ? "OK" : "NG");
            p = search_soundrequest(appconf, ICO_APF_RESID_BASIC_SOUND, info->id, NULL);
            if (p)  {
                if (p->state & ICO_APC_REQSTATE_REPLYACTIVE)    {
                    if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEOFF,
                                                          p->pid)
                            != ICO_APF_RESOURCE_E_NONE) {
                        apfw_warn("resource_reqcb: send MSM Error");
                    }
                    if (soundcontrol) {
                        (*soundcontrol)(get_appconf(req->appid), 1);
                    }
                }
                p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                p->timer = 0;
            }
            break;
        case ICO_APF_SOUND_EVENT_NEW:               /* create new sound stream  */
            apfw_trace("resource_reqcb: app(%s) create BasicSound", info->appid);
            p = search_soundrequest(appconf, ICO_APF_RESID_BASIC_SOUND, -1, &freq);
            if (p)  {
                apfw_trace("resource_reqcb: app(%s,state=%x,prio=%08x,pid=%d=>%d) "
                           "requested sound, Nop", p->appid, p->state, p->prio,
                           p->pid, info->pid);
                p->pid = info->pid;
                if (p->state & ICO_APC_REQSTATE_WAITREQ)    {
                    if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEON,
                                                          p->pid)
                            != ICO_APF_RESOURCE_E_NONE) {
                        apfw_warn("resource_reqcb: send MSM Error");
                    }
                    if (soundcontrol) {
                        (*soundcontrol)(get_appconf(p->appid), 0);
                    }
                    if ((freq->state & ICO_APC_REQSTATE_WAITREQ) == 0)  {
                        if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEOFF,
                                                              freq->pid)
                                    != ICO_APF_RESOURCE_E_NONE) {
                            apfw_warn("resource_reqcb: send MSM Error");
                        }
                        if (soundcontrol) {
                            (*soundcontrol)(get_appconf(freq->appid), 1);
                        }
                    }
                }
            }
            else    {
                apfw_trace("resource_reqcb: app(%s) not requested sound", info->appid);
                req->resid = ICO_APF_RESID_BASIC_SOUND;
                req->reqtype = ICO_APC_REQTYPE_CREATE;
                strcpy(req->device,
                       confsys->sound[confsys->misc.default_soundId].
                            zone[confsys->misc.default_soundId].name);
                app_getsound(req, 0);
                req = NULL;
            }
            break;
        default:
            apfw_error("resource_reqcb: illegal command(%d)", info->state);
            break;
        }
        break;
    case ICO_APF_RESID_INT_SOUND:                   /* interrupt sound          */
        switch (info->state)    {
        case ICO_APF_RESOURCE_COMMAND_GET:          /* get resource             */
            apfw_trace("resource_reqcb: app(%s) get IntSound(%d)",
                       info->appid, info->id);
            app_getsound(req, ICO_UXF_PRIO_ONSCREEN);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_COMMAND_RELEASE:      /* release resource         */
            apfw_trace("resource_reqcb: app(%s) release IntSound(%d)",
                       info->appid, info->id);
            app_freesound(req, 1);
            req = NULL;
            break;
        case ICO_APF_RESOURCE_REPLY_OK:             /* ack reply                */
        case ICO_APF_RESOURCE_REPLY_NG:             /* nak reply                */
            apfw_trace("resource_reqcb: app(%s) IntSound(%d) reply(%s)", info->appid,
                       info->id, info->state == ICO_APF_RESOURCE_REPLY_OK ? "OK" : "NG");
            p = search_soundrequest(appconf, ICO_APF_RESID_INT_SOUND, info->id, NULL);
            if (p)  {
                if (p->state & ICO_APC_REQSTATE_REPLYACTIVE)    {
                    if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEOFF,
                                                          p->pid)
                            != ICO_APF_RESOURCE_E_NONE) {
                        apfw_warn("resource_reqcb: send MSM Error");
                    }
                    if (soundcontrol) {
                        (*soundcontrol)(get_appconf(p->appid), 1);
                    }
                }
                p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                p->timer = 0;
            }
            break;
        default:
            apfw_error("resource_reqcb: illegal command(%d)", info->state);
            break;
        }
        break;
    case ICO_APF_RESID_INPUT_DEV:                   /* input switch             */
        if ((info->id < 0) || (info->id >= appconf->inputdevNum))   {
            apfw_error("resource_reqcb: app(%s) cmd=%d InputSW(%d) but no exist",
                       info->appid, info->state, info->id);
        }
        else    {
            switch (info->state)    {
            case ICO_APF_RESOURCE_COMMAND_ADD:      /* get switch               */
                apfw_trace("resource_reqcb: app(%s) get InputSW(%d)",
                           info->appid, info->id);
                app_getinput(req, 0);
                req = NULL;
                break;
            case ICO_APF_RESOURCE_COMMAND_CHANGE:   /* change switch            */
                apfw_trace("resource_reqcb: app(%s) change InputSW(%d)",
                           info->appid, info->id);
                app_getinput(req, 0);
                req = NULL;
                break;
            case ICO_APF_RESOURCE_COMMAND_DELETE:   /* delete switch            */
                apfw_trace("resource_reqcb: app(%s) delete InputSW(%d)",
                           info->appid, info->id);
                app_freeinput(req, 1);
                req = NULL;
                break;
            default:
                apfw_error("resource_reqcb: illegal command(%d)", info->state);
                break;
            }
        }
        break;
    default:
        apfw_error("resource_reqcb: illegal resource(%d)", info->resid);
        break;
    }
    if (req)    {
        req->next = free_request;
        free_request = req;
    }
    apfw_trace("resource_reqcb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_getdisplay: get display zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   addprio         a priority to add to the priority of the request
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_getdisplay(ico_apc_request_t *req, const int addprio)
{
    int     prio;
    int     i, j;
    Ico_Uxf_conf_application    *conf = (Ico_Uxf_conf_application *)get_appconf(req->appid);
    Ico_Uxf_conf_display_zone   *zone;
    ico_apc_dispzone_t          *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    /* priority     */
    prio = getpriority(PRIO_PROCESS, req->pid);
    if (prio > 19)          prio = 19;
    else if (prio < -20)    prio = -20;
    prio = confsys->category[conf->categoryId].priority * ICO_UXF_PRIO_CATEGORY + 19 - prio;
    prio += addprio;
    if (ico_syc_apc_regulation_app_visible(conf->categoryId))   {
        prio |= ICO_UXF_PRIO_REGULATION;
    }
    if (ico_uxf_process_is_active(conf->appid)) {
        for (i = 0; i < ndispzone; i++) {
            p = dispzone[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }
        }
        prio |= ICO_UXF_PRIO_ACTIVEAPP;
    }

    /* get display zone from device name        */
    for (i = 0; i < ndispzone; i++) {
        if (strcasecmp(dispzone[i].conf->name, req->device) == 0)  break;
    }
    if (i >= ndispzone) {
        i = confsys->misc.default_dispzoneId;
        apfw_trace("app_getdisplay: Entry(app=%s zone=%s(%s none) res=%d prio=%x(+%x))",
                   conf->appid, dispzone[i].conf->name, req->resid, req->device,
                   prio, addprio);
    }
    else    {
        apfw_trace("app_getdisplay: Entry(app=%s zone=%s res=%d prio=%x(+%x))",
                   conf->appid, dispzone[i].conf->name, req->resid, prio, addprio);
    }
    req->zoneidx = i;

    czone = &dispzone[i];
    zone = czone->conf;

    /* search same request          */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if ((strcmp(p->appid, req->appid) == 0) && (p->resid == req->resid) &&
            (p->zoneidx == req->zoneidx))   {
            break;
        }
        bp = p;
        p = p->next;
    }
    if (p)  {
        if (p->reqtype != ICO_APC_REQTYPE_REQUEST)  {
            apfw_trace("app_getdisplay: Leave(found same request)");
            return;
        }

        if (p->prio > prio) {
            prio = p->prio;
        }
        apfw_trace("app_getdisplay: found same request(app=%s zone=%s res=%d prio=%d)",
                   conf->appid, dispzone[i].conf->name, req->resid, prio);
        if (bp) {
            bp->next = p->next;
        }
        else    {
            czone->req = p->next;
        }
        p->next = free_request;
        free_request = p;
    }

    /* link request to zone table   */
    req->prio = prio;
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p->prio <= prio)    break;
        bp = p;
        p = p->next;
    }
    if (bp) {
        req->next = bp->next;
        bp->next = req;
        apfw_trace("app_getdisplay: app(%s) set after(%s) of zone(%s)",
                   conf->appid, bp->appid, req->device);
    }
    else    {
        req->next = czone->req;
        czone->req = req;
        apfw_trace("app_getdisplay: app(%s) set top of zone(%s) next %s",
                   conf->appid, req->device, req->next ? req->next->appid : "(NULL)");
    }

    /* check if maximum priority    */
    if (! bp)   {
        j = -1;
        for (i = 0; i < czone->noverlap; i++)    {
            if (((czone->overlap[i])->req != NULL) &&
                ((czone->overlap[i])->req->prio > prio))    {
                j = i;
                prio = czone->overlap[i]->req->prio;
            }
        }
    }
    else    {
        j = 9999;
    }
    if ((j >= 0) || ((req->prio & ICO_UXF_PRIO_REGULATION) == 0))   {
        /* lower priority, waitting this application.           */
        /* insert application to zone application list and      */
        /* change zone priority, if request application is top priority */
        req->state |= ICO_APC_REQSTATE_WAITREQ;
        if (j >= 0) {
            /* lower priority       */
            apfw_trace("app_getdisplay: priority low, waitting %s", conf->appid);
            ico_uxf_window_control(conf->appid, req->id, ICO_UXF_APPSCTL_INVISIBLE, 1);
            if (displaycontrol) {
                (*displaycontrol)(conf, 0);
            }
        }
        if ((req->prio & ICO_UXF_PRIO_REGULATION) == 0) {
            /* regulation control   */
            apfw_trace("app_getdisplay: regulation, waitting %s", conf->appid);
            ico_uxf_window_control(conf->appid, req->id, ICO_UXF_APPSCTL_REGULATION, 1);
            if ((j < 0) && (displaycontrol)) {
                (*displaycontrol)(conf, 0);
            }
        }
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                    conf->appid, ICO_APF_RESOURCE_STATE_WAITTING,
                    req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE)   {
                apfw_warn("app_getdisplay: Leave(send(%s) Error)", conf->appid);
            }
            else    {
                req->state &= ~ICO_APC_REQSTATE_REPLYACTIVE;
                req->state |= ICO_APC_REQSTATE_REPLYQUIET;
                req->timer = ICO_APC_REQREPLY_MAXTIME;
                timer_count ++;
            }
        }
    }
    else    {
        /* maximum priority, ok             */
        apfw_trace("app_getdisplay: priority heigh");
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                        conf->appid, ICO_APF_RESOURCE_STATE_ACQUIRED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
                apfw_warn("app_getdisplay: Leave(send(%s) Error)", conf->appid);
                ico_uxf_window_control(conf->appid, req->id, ICO_UXF_APPSCTL_INVISIBLE, 0);
                if (displaycontrol) {
                    (*displaycontrol)(conf, 1);
                }
            }
            else    {
                req->state &= ~ICO_APC_REQSTATE_REPLYQUIET;
                req->state |= ICO_APC_REQSTATE_REPLYACTIVE;
                req->timer = ICO_APC_REQREPLY_MAXTIME;
                timer_count ++;
            }
        }
        else    {
            ico_uxf_window_control(conf->appid, req->id, ICO_UXF_APPSCTL_INVISIBLE, 0);
            if (displaycontrol) {
                (*displaycontrol)(conf, 1);
            }
        }
        /* send change event to invisible application   */
        apfw_trace("app_getdisplay: next=%08x %s next_state=%x",
                   req->next, req->next ? req->next->appid : " ",
                   req->next ? req->next->state : 0x9999);
        if ((req->next) && ((req->next->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
            p = req->next;
        }
        else    {
            p = NULL;
        }
        /* send all waitting applications   */
        i = 0;
        while (1)   {
            if (p != NULL)  {
                if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0)   {
                    p->state |= ICO_APC_REQSTATE_WAITREQ;
                    apfw_trace("app_getdisplay: overlaped(%s), waitting", p->appid);
                    if (p->reqtype == ICO_APC_REQTYPE_REQUEST)  {
                        if (ico_apf_resource_send_to_client(
                                p->appid, ICO_APF_RESOURCE_STATE_DEPRIVED,
                                p->resid, p->device, p->id) != ICO_APF_RESOURCE_E_NONE) {
                            apfw_warn("app_getdisplay: send(%s) Error)", p->appid);
                        }
                        else    {
                            req->state &= ~ICO_APC_REQSTATE_REPLYACTIVE;
                            p->state |= ICO_APC_REQSTATE_REPLYQUIET;
                            p->timer = ICO_APC_REQREPLY_MAXTIME;
                            timer_count ++;
                        }
                    }
                    ico_uxf_window_control(p->appid, -1, ICO_UXF_APPSCTL_INVISIBLE, 1);
                    if (displaycontrol) {
                        (*displaycontrol)(get_appconf(p->appid), 0);
                    }
                }
            }
            if (i >= czone->noverlap)   break;
            p = czone->overlap[i]->req;
            i ++;
        }
    }
    apfw_trace("app_getdisplay: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_freedisplay: free display zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   send            send release event to client(1=send/0=no send)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_freedisplay(ico_apc_request_t *req, const int send)
{
    int     idx;
    Ico_Uxf_conf_application    *conf = get_appconf(req->appid);
    ico_apc_dispzone_t          *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    apfw_trace("app_freedisplay: Entry(app=%s)", req->appid);

    czone = &dispzone[req->zoneidx];
    idx = czone->conf->display->id;

    ico_uxf_window_control(conf->appid, req->id, ICO_UXF_APPSCTL_INVISIBLE, 0);
    if (displaycontrol) {
        (*displaycontrol)(conf, 0);
    }
    if ((send !=0) && (req->reqtype == ICO_APC_REQTYPE_REQUEST))   {
        (void) ico_apf_resource_send_to_client(
                                req->appid, ICO_APF_RESOURCE_STATE_RELEASED,
                                req->resid, req->device, req->id);
    }

    /* find request                     */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p == req)   break;
        bp = p;
        p = p->next;
    }
    if (! p)    {
        apfw_warn("app_freedisplay: Leave(request dose not exist)");
        return;
    }

    /* release request table from zone table    */
    if (bp) {
        bp->next = p->next;

        /* request is waitted, no need other control*/
        p->next = free_request;
        free_request = p;
        apfw_trace("app_freedisplay: Leave(request waited)");
        return;
    }
    czone->req = p->next;
    p->next = free_request;
    free_request = p;

    /* recalculate visible zone         */
    recalc_dispzone(idx);

    apfw_trace("app_freedisplay: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change_disprequest: change display zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   active          active(1) or inactive(0)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
change_disprequest(ico_apc_request_t *req, const int active)
{
    apfw_trace("change_disprequest: change to %s(%s)", active ? "active" : "inactive",
               req->appid);

    req->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);

    if (req->reqtype == ICO_APC_REQTYPE_REQUEST)   {
        if (ico_apf_resource_send_to_client(
                        req->appid,
                        active ? ICO_APF_RESOURCE_STATE_ACQUIRED :
                                 ICO_APF_RESOURCE_STATE_DEPRIVED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
            apfw_warn("change_disprequest: send(%s) Error", req->appid);
        }
        else    {
            req->state |= (active ? ICO_APC_REQSTATE_REPLYACTIVE :
                                    ICO_APC_REQSTATE_REPLYQUIET);
            req->timer = ICO_APC_REQREPLY_MAXTIME;
            timer_count ++;
        }
    }
    if ((req->state & ICO_APC_REQSTATE_REPLYACTIVE) == 0)   {
        ico_uxf_window_control(req->appid, req->id, ICO_UXF_APPSCTL_INVISIBLE,
                               active ? 0 : 1);
        if (displaycontrol) {
            (*displaycontrol)(get_appconf(req->appid), active);
        }
    }
    if (active)   {
        req->state &= ~ICO_APC_REQSTATE_WAITREQ;
    }
    else    {
        req->state |= ICO_APC_REQSTATE_WAITREQ;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   recalc_dispzone: calculate all display zone request priority(static function)
 *
 * @param[in]   idx         display index number
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
recalc_dispzone(const int idx)
{
    int     i;
    int     prio;
    ico_apc_dispzone_t  *czone;
    ico_apc_dispzone_t  *czone2;
    ico_apc_request_t   *p;
    Ico_Uxf_conf_display *disp = &confsys->display[idx];

    apfw_trace("recalc_dispzone: Enter(disp=%s)", disp->name);

    /* get top of priority of this display  */
    prio = -1;
    czone2 = NULL;
    for (i = 0; i < disp->zoneNum; i++)    {
        czone = &dispzone[disp->zone[i].zoneidx];
        p = czone->req;
        if (! p)    continue;
        p->state |= ICO_APC_REQSTATE_WAITPROC;
        if (p->prio > prio)    {
            czone2 = czone;
            prio = p->prio;
        }
    }
    if (czone2 == NULL) {
        /* no visible zone, end         */
        apfw_trace("recalc_dispzone: Leave(no request)");
        return;
    }
    if ((czone2->req->state & ICO_APC_REQSTATE_WAITREQ) == 0)   {
        /* not unvisible zone           */
        for (i = 0; i < disp->zoneNum; i++)    {
            czone = &dispzone[disp->zone[i].zoneidx];
            p = czone->req;
            if (! p)    continue;
            p->state &= ~ICO_APC_REQSTATE_WAITPROC;
        }
        apfw_trace("recalc_dispzone: Leave(%s no need visible control)",
                   czone2->req->appid);
        return;
    }

    /* change to show for top priority  */
    p = czone2->req;
    p->state &= ~ICO_APC_REQSTATE_WAITPROC;
    change_disprequest(p, 1);

    /* hide overlap zone                */
    for (i = 0; i < czone2->noverlap; i++)  {
        p = czone2->overlap[i]->req;
        if (p)    {
            if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                p->state &= ~ICO_APC_REQSTATE_WAITPROC;
                change_disprequest(p, 0);
            }
        }
    }

    /* show not overlap zone            */
    for (i = 0; i < disp->zoneNum; i++)    {
        czone = &dispzone[disp->zone[i].zoneidx];
        p = czone->req;
        if ((p == NULL) || ((p->state & ICO_APC_REQSTATE_WAITPROC) == 0))   continue;
        p->state &= ~ICO_APC_REQSTATE_WAITPROC;
        change_disprequest(p, 1);
    }
    apfw_trace("recalc_dispzone: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_getsound: get sound zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   addprio         a priority to add to the priority of the request
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_getsound(ico_apc_request_t *req, const int addprio)
{
    int     prio;
    int     i, j;
    Ico_Uxf_conf_application    *conf = get_appconf(req->appid);
    Ico_Uxf_conf_sound_zone     *zone;
    ico_apc_soundzone_t         *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    /* priority     */
    prio = getpriority(PRIO_PROCESS, req->pid);
    if (prio > 19)          prio = 19;
    else if (prio < -20)    prio = -20;
    prio = (confsys->category[conf->categoryId].priority * ICO_UXF_PRIO_CATEGORY) + 19 - prio;
    prio += addprio;
    if (ico_syc_apc_regulation_app_sound(conf->categoryId)) {
        prio |= ICO_UXF_PRIO_REGULATION;
    }
    if (ico_uxf_process_is_active(conf->appid)) {
        for (i = 0; i < nsoundzone; i++) {
            p = soundzone[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }
        }
        prio |= ICO_UXF_PRIO_ACTIVEAPP;
    }

    /* get sound zone from device name      */
    for (i = 0; i < nsoundzone; i++) {
        if (strcasecmp(soundzone[i].conf->name, req->device) == 0)  break;
    }
    if (i >= nsoundzone) {
        i = confsys->misc.default_soundzoneId;
        apfw_trace("app_getsound: Entry(app=%s zone=%s(%s none) prio=%x(+%x) pid=%d)",
                   conf->appid, soundzone[i].conf->name, req->device, prio, addprio,
                   req->pid);
    }
    else    {
        apfw_trace("app_getsound: Entry(app=%s zone=%s prio=%x(+%x) pid=%d)",
                   conf->appid, soundzone[i].conf->name, prio, addprio, req->pid);
    }
    req->zoneidx = i;

    czone = &soundzone[i];
    zone = czone->conf;

    /* search same request          */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if ((strcmp(p->appid, req->appid) == 0) && (p->resid == req->resid) &&
            (p->zoneidx == req->zoneidx))   {
            break;
        }
        bp = p;
        p = p->next;
    }
    if (p)  {
        if (p->reqtype != ICO_APC_REQTYPE_REQUEST)  {
            apfw_trace("app_getsound: Leave(found same request)");
            return;
        }

        if (p->prio > prio) {
            prio = p->prio;
        }
        apfw_trace("app_getsound: found same request(app=%s zone=%s res=%d prio=%d)",
                   conf->appid, soundzone[i].conf->name, req->resid, prio);
        if (bp) {
            bp->next = p->next;
        }
        else    {
            czone->req = p->next;
        }
        p->next = free_request;
        free_request = p;
    }

    /* link request to zone table   */
    req->prio = prio;
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p->prio <= prio)    break;
        bp = p;
        p = p->next;
    }
    if (bp) {
        req->next = bp->next;
        bp->next = req;
    }
    else    {
        req->next = czone->req;
        czone->req = req;
    }

    /* check if maximum priority    */
    if (! bp)   {
        j = -1;
        for (i =0; i < czone->noverlap; i++)    {
            if (((czone->overlap[i])->req != NULL) &&
                ((czone->overlap[i])->req->prio > prio))    {
                j = i;
                prio = czone->overlap[i]->req->prio;
            }
        }
    }
    else    {
        j = 9999;
    }
    if ((j >= 0) || ((req->prio & ICO_UXF_PRIO_REGULATION) == 0))   {
        /* lower priority, waitting this application.           */
        /* insert application to zone application list and      */
        /* change zone priority, if request application is top priority */
        req->state |= ICO_APC_REQSTATE_WAITREQ;
        if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEON, req->pid)
                != ICO_APF_RESOURCE_E_NONE) {
            apfw_warn("app_getsound: send MSM Error");
        }
        if (soundcontrol) {
            (*soundcontrol)(conf, 0);
        }
        apfw_trace("app_getsound: priority low, waitting %s", conf->appid);
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                    conf->appid, ICO_APF_RESOURCE_STATE_WAITTING,
                    req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE)   {
                apfw_warn("app_getsound: Leave(send(%s) Error)", conf->appid);
            }
            else    {
                req->state |= ICO_APC_REQSTATE_REPLYQUIET;
                req->timer = ICO_APC_REQREPLY_MAXTIME;
                timer_count ++;
            }
        }
    }
    else    {
        /* maximum priority, ok             */
        apfw_trace("app_getsound: priority heigh(%08x)", req->prio);
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                        conf->appid, ICO_APF_RESOURCE_STATE_ACQUIRED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
                apfw_warn("app_getsound: Leave(send(%s) Error)", conf->appid);
            }
        }
        /* send change event to mute application    */
        apfw_trace("app_getsound: next=%08x %s next_state=%x",
                   req->next, req->next ? req->next->appid : " ",
                   req->next ? req->next->state : 0x9999);
        if ((req->next) && ((req->next->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
            p = req->next;
        }
        else    {
            p = NULL;
        }
        /* send all waitting applications   */
        i = 0;
        while (1)   {
            if (p != NULL)  {
                if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0)   {
                    p->state |= ICO_APC_REQSTATE_WAITREQ;
                    apfw_trace("app_getsound: overlaped(%s), waitting", p->appid);
                    if (p->reqtype == ICO_APC_REQTYPE_REQUEST)  {
                        if (ico_apf_resource_send_to_client(
                                p->appid, ICO_APF_RESOURCE_STATE_DEPRIVED,
                                p->resid, p->device, p->id) != ICO_APF_RESOURCE_E_NONE) {
                            apfw_warn("app_getsound: send(%s) Error)", p->appid);
                        }
                        else    {
                            p->state |= ICO_APC_REQSTATE_REPLYQUIET;
                            p->timer = ICO_APC_REQREPLY_MAXTIME;
                            timer_count ++;
                        }
                    }
                }
                if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEON, p->pid)
                        != ICO_APF_RESOURCE_E_NONE) {
                    apfw_warn("app_getsound: send MSM Error");
                }
                if (soundcontrol) {
                    (*soundcontrol)(get_appconf(p->appid), 0);
                }
            }
            if (i >= czone->noverlap)   break;
            p = czone->overlap[i]->req;
            i ++;
        }
    }
    apfw_trace("app_getsound: Leave(req=%08x,state=%x,prio=%08x)",
               (int)req, req->state, req->prio);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_freesound: free sound zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   send            send release event to client(1=send/0=no send)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_freesound(ico_apc_request_t *req, const int send)
{
    int     idx;
    Ico_Uxf_conf_application    *conf = get_appconf(req->appid);
    ico_apc_soundzone_t         *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    apfw_trace("app_freesound: Entry(app=%s)", req->appid);

    czone = &soundzone[req->zoneidx];
    idx = czone->conf->sound->id;

    if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEON, req->pid)
                != ICO_APF_RESOURCE_E_NONE) {
        apfw_warn("app_freesound: send MSM Error");
    }
    if (soundcontrol) {
        (*soundcontrol)(conf, 0);
    }
    if ((send !=0) && (req->reqtype == ICO_APC_REQTYPE_REQUEST))   {
        (void) ico_apf_resource_send_to_client(
                                req->appid, ICO_APF_RESOURCE_STATE_RELEASED,
                                req->resid, req->device, req->id);
    }

    /* find request                     */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p == req)   break;
        bp = p;
        p = p->next;
    }
    if (! p)    {
        apfw_warn("app_freesound: Leave(request dose not exist)");
        return;
    }

    /* release request table from zone table    */
    if (bp) {
        bp->next = p->next;

        /* request is waitted, no need other control*/
        p->next = free_request;
        free_request = p;
        apfw_trace("app_freesound: Leave(request waited)");
        return;
    }
    czone->req = p->next;
    p->next = free_request;
    free_request = p;

    /* recalculate visible zone         */
    recalc_soundzone(idx);

    apfw_trace("app_freesound: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change_soundrequest: change sound zone resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   active          active(1) or quiet(0)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
change_soundrequest(ico_apc_request_t *req, const int active)
{
    apfw_trace("change_soundrequest: change to %s(%s)", active ? "active" : "quiet",
               req->appid);

    req->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);

    if (req->reqtype == ICO_APC_REQTYPE_REQUEST)   {
        if (ico_apf_resource_send_to_client(
                        req->appid,
                        active ? ICO_APF_RESOURCE_STATE_ACQUIRED :
                                 ICO_APF_RESOURCE_STATE_DEPRIVED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
            apfw_warn("change_soundrequest: send(%s) Error", req->appid);
        }
        else    {
            req->state |= (active ? ICO_APC_REQSTATE_REPLYACTIVE :
                                    ICO_APC_REQSTATE_REPLYQUIET);
            req->timer = ICO_APC_REQREPLY_MAXTIME;
            timer_count ++;
        }
    }
    if ((req->state & ICO_APC_REQSTATE_REPLYACTIVE) == 0)   {
        if (ico_apf_resource_send_to_soundctl(
                    active ? ICO_APF_SOUND_COMMAND_MUTEOFF : ICO_APF_SOUND_COMMAND_MUTEON,
                    req->pid) != ICO_APF_RESOURCE_E_NONE)   {
            apfw_warn("change_soundrequest: send MSM Error");
        }
        if (soundcontrol) {
            (*soundcontrol)(get_appconf(req->appid), active);
        }
    }
    if (active)   {
        req->state &= ~ICO_APC_REQSTATE_WAITREQ;
    }
    else    {
        req->state |= ICO_APC_REQSTATE_WAITREQ;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   recalc_soundzone: calculate all sound zone request priority(static function)
 *
 * @param[in]   idx         sound index number
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
recalc_soundzone(const int idx)
{
    int     i;
    int     prio;
    ico_apc_soundzone_t *czone;
    ico_apc_soundzone_t *czone2;
    ico_apc_request_t   *p;
    Ico_Uxf_conf_sound  *sound = &confsys->sound[idx];

    apfw_trace("recalc_soundzone: Enter(sound=%s)", sound->name);

    /* get top of priority of this sound  */
    prio = -1;
    czone2 = NULL;
    for (i = 0; i < sound->zoneNum; i++)    {
        czone = &soundzone[sound->zone[i].zoneidx];
        p = czone->req;
        if (! p)    continue;
        p->state |= ICO_APC_REQSTATE_WAITPROC;
        if (p->prio > prio)    {
            czone2 = czone;
            prio = p->prio;
        }
    }
    if (czone2 == NULL) {
        /* no active zone, end          */
        apfw_trace("recalc_soundzone: Leave(no request)");
        return;
    }
    if ((czone2->req->state & ICO_APC_REQSTATE_WAITREQ) == 0)   {
        /* not quiet zone               */
        for (i = 0; i < sound->zoneNum; i++)    {
            czone = &soundzone[sound->zone[i].zoneidx];
            p = czone->req;
            if (! p)    continue;
            p->state &= ~ICO_APC_REQSTATE_WAITPROC;
        }
        apfw_trace("recalc_soundzone: Leave(%s no need sound control)",
                   czone2->req->appid);
        return;
    }

    /* change to active for top priority*/
    p = czone2->req;
    p->state &= ~ICO_APC_REQSTATE_WAITPROC;
    if (p->prio & ICO_UXF_PRIO_REGULATION)  {
        apfw_trace("recalc_soundzone: Start %s(prio=%08x and no regulation)",
                   p->appid, p->prio);
        change_soundrequest(p, 1);
    }

    /* mute overlap zone                */
    for (i = 0; i < czone2->noverlap; i++)  {
        p = czone2->overlap[i]->req;
        if (p)    {
            if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                p->state &= ~ICO_APC_REQSTATE_WAITPROC;
                if (p->prio & ICO_UXF_PRIO_REGULATION)  {
                    apfw_trace("recalc_soundzone: Overlap Stop %s(top and no regulation)",
                               p->appid);
                    change_soundrequest(p, 0);
                }
                else    {
                    p->state |= ICO_APC_REQSTATE_WAITREQ;
                }
            }
        }
    }

    /* reset mute not overlap zone      */
    for (i = 0; i < sound->zoneNum; i++)    {
        czone = &soundzone[sound->zone[i].zoneidx];
        p = czone->req;
        if ((p == NULL) || ((p->state & ICO_APC_REQSTATE_WAITPROC) == 0))   continue;
        p->state &= ~ICO_APC_REQSTATE_WAITPROC;
        if (p->state & ICO_APC_REQSTATE_WAITREQ)    {
            if (p->prio & ICO_UXF_PRIO_REGULATION)  {
                apfw_trace("recalc_soundzone: Overlap Start %s(top and no regulation)",
                           p->appid);
                change_soundrequest(p, 1);
            }
        }
    }
    apfw_trace("recalc_soundzone: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_getinput: get input switch resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   addprio         a priority to add to the priority of the request
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_getinput(ico_apc_request_t *req, const int addprio)
{
    int     prio;
    int     i;
    Ico_Uxf_conf_application    *conf = get_appconf(req->appid);
    ico_apc_inputsw_t           *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    /* priority     */
    prio = getpriority(PRIO_PROCESS, req->pid);
    if (prio > 19)          prio = 19;
    else if (prio < -20)    prio = -20;
    prio = (confsys->category[conf->categoryId].priority * ICO_UXF_PRIO_CATEGORY) + 19 - prio;
    prio += addprio;
    if (ico_syc_apc_regulation_app_input(conf->categoryId)) {
        prio |= ICO_UXF_PRIO_REGULATION;
    }
    if (ico_uxf_process_is_active(conf->appid)) {
        for (i = 0; i < ninputsw; i++) {
            p = inputsw[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }
        }
        prio |= ICO_UXF_PRIO_ACTIVEAPP;
    }

    /* get input switch from device name    */
    for (i = 0; i < ninputsw; i++) {
        if (strcasecmp(inputsw[i].inputsw->swname, req->device) == 0)  break;
    }
    if (i >= ninputsw) {
        i = confsys->misc.default_inputswId;
        apfw_trace("app_getinput: Entry(app=%s inputsw=%s(%s none) prio=%x(+%x) pid=%d)",
                   conf->appid, inputsw[i].inputsw->swname, req->device, prio, addprio,
                   req->pid);
    }
    else    {
        apfw_trace("app_getinput: Entry(app=%s zone=%s prio=%x(+%x) pid=%d)",
                   conf->appid, inputsw[i].inputsw->swname, prio, addprio, req->pid);
    }
    req->zoneidx = i;

    czone = &inputsw[i];

    /* search same request          */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if ((strcmp(p->appid, req->appid) == 0) && (p->resid == req->resid) &&
            (p->zoneidx == req->zoneidx))   {
            break;
        }
        bp = p;
        p = p->next;
    }
    if (p)  {
        if (p->reqtype != ICO_APC_REQTYPE_REQUEST)  {
            apfw_trace("app_getinput: Leave(found same request)");
            return;
        }

        if (p->prio > prio) {
            prio = p->prio;
        }
        apfw_trace("app_getinput: found same request(app=%s sw=%s res=%d prio=%d)",
                   conf->appid, inputsw[i].inputsw->swname, req->resid, prio);
        if (bp) {
            bp->next = p->next;
        }
        else    {
            czone->req = p->next;
        }
        p->next = free_request;
        free_request = p;
    }

    /* link request to zone table   */
    req->prio = prio;
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p->prio <= prio)    break;
        bp = p;
        p = p->next;
    }
    if (bp) {
        req->next = bp->next;
        bp->next = req;
    }
    else    {
        req->next = czone->req;
        czone->req = req;
    }

    if ((req->prio & ICO_UXF_PRIO_REGULATION) == 0) {
        /* lower priority, waitting this application.           */
        /* insert application to zone application list and      */
        /* change zone priority, if request application is top priority */
        req->state |= ICO_APC_REQSTATE_WAITREQ;
        if (ico_uxf_input_control(0, req->appid, czone->inputdev->device,
                                  czone->inputsw->input) != ICO_UXF_EOK)    {
            apfw_warn("app_getinput: send MIM Error");
        }
        if (inputcontrol) {
            (*inputcontrol)(conf, 0);
        }
        apfw_trace("app_getinput: priority low, waitting %s", conf->appid);
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                    conf->appid, ICO_APF_RESOURCE_STATE_WAITTING,
                    req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE)   {
                apfw_warn("app_getinput: Leave(send(%s) Error)", conf->appid);
            }
            else    {
                req->state |= ICO_APC_REQSTATE_REPLYQUIET;
                req->timer = ICO_APC_REQREPLY_MAXTIME;
                timer_count ++;
            }
        }
    }
    else    {
        /* maximum priority, ok             */
        apfw_trace("app_getinput: priority heigh(%08x)", req->prio);
        if (req->reqtype == ICO_APC_REQTYPE_REQUEST)    {
            if (ico_apf_resource_send_to_client(
                        conf->appid, ICO_APF_RESOURCE_STATE_ACQUIRED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
                apfw_warn("app_getinput: Leave(send(%s) Error)", conf->appid);
            }
            else    {
                req->state |= ICO_APC_REQSTATE_REPLYACTIVE;
                req->timer = ICO_APC_REQREPLY_MAXTIME;
                timer_count ++;
            }
        }
        if (ico_uxf_input_control(1, req->appid, czone->inputdev->device,
                                  czone->inputsw->input) != ICO_UXF_EOK)    {
            apfw_warn("app_getinput: send MIM Error");
        }
        /* change lower priority stateus            */
        if ((req->next) && ((req->next->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
            p = req->next;
            p->state |= ICO_APC_REQSTATE_WAITREQ;
            apfw_trace("app_getinput: lower priority(%s), waitting", p->appid);
            if (p->reqtype == ICO_APC_REQTYPE_REQUEST)  {
                if (ico_apf_resource_send_to_client(
                            p->appid, ICO_APF_RESOURCE_STATE_DEPRIVED,
                            p->resid, p->device, p->id) != ICO_APF_RESOURCE_E_NONE) {
                    apfw_warn("app_getinput: send(%s) Error)", p->appid);
                }
                else    {
                    p->state |= ICO_APC_REQSTATE_REPLYQUIET;
                    p->timer = ICO_APC_REQREPLY_MAXTIME;
                    timer_count ++;
                }
            }
        }
    }
    apfw_trace("app_getinput: Leave(req=%08x,state=%x,prio=%08x)",
               (int)req, req->state, req->prio);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_freeinput: free input switch resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   send            send release event to client(1=send/0=no send)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_freeinput(ico_apc_request_t *req, const int send)
{
    Ico_Uxf_conf_application    *conf = get_appconf(req->appid);
    ico_apc_inputsw_t           *czone;
    ico_apc_request_t           *p;
    ico_apc_request_t           *bp;

    apfw_trace("app_freeinput: Entry(app=%s)", req->appid);

    czone = &inputsw[req->zoneidx];

    if (ico_uxf_input_control(0, conf->appid, czone->inputdev->device,
                              czone->inputsw->input) != ICO_UXF_EOK)    {
        apfw_warn("app_freeinput: send MIM Error");
    }
    if (inputcontrol) {
        (*inputcontrol)(conf, 0);
    }
    if ((send !=0) && (req->reqtype == ICO_APC_REQTYPE_REQUEST))   {
        (void) ico_apf_resource_send_to_client(
                                req->appid, ICO_APF_RESOURCE_STATE_RELEASED,
                                req->resid, req->device, req->id);
    }

    /* find request                     */
    p = czone->req;
    bp = NULL;
    while (p)   {
        if (p == req)   break;
        bp = p;
        p = p->next;
    }
    if (! p)    {
        apfw_warn("app_freeinput: Leave(request dose not exist)");
        return;
    }

    /* release request table from zone table    */
    if (bp) {
        bp->next = p->next;

        /* request is waitted, no need other control*/
        p->next = free_request;
        free_request = p;
        apfw_trace("app_freeinput: Leave(request waited)");
        return;
    }
    czone->req = p->next;
    p->next = free_request;
    free_request = p;

    /* recalculate visible zone         */
    recalc_inputsw(czone->inputsw->input);

    apfw_trace("app_freeinput: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   change_inputrequest: change input switch resource(static function)
 *
 * @param[in]   req             request block
 * @param[in]   active          use(1) or unuse(0)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
change_inputrequest(ico_apc_request_t *req, const int active)
{
    ico_apc_inputsw_t       *czone;

    apfw_trace("change_inputrequest: change to %s(%s)", active ? "use" : "unuse",
               req->appid);

    czone = &inputsw[req->zoneidx];

    if (req->reqtype == ICO_APC_REQTYPE_REQUEST)   {
        if (ico_apf_resource_send_to_client(
                        req->appid,
                        active ? ICO_APF_RESOURCE_STATE_ACQUIRED :
                                 ICO_APF_RESOURCE_STATE_DEPRIVED,
                        req->resid, req->device, req->id) != ICO_APF_RESOURCE_E_NONE) {
            apfw_warn("change_inputrequest: send(%s) Error", req->appid);
        }
        else    {
            req->state |= (active ? ICO_APC_REQSTATE_REPLYACTIVE :
                                    ICO_APC_REQSTATE_REPLYQUIET);
            req->timer = ICO_APC_REQREPLY_MAXTIME;
            timer_count ++;
        }
    }
    if (ico_uxf_input_control(active, req->appid, czone->inputdev->device,
                              czone->inputsw->input) != ICO_UXF_EOK)    {
        apfw_warn("app_getinput: send MIM Error");
    }
    if (inputcontrol) {
        (*inputcontrol)(get_appconf(req->appid), active);
    }
    if (active)   {
        req->state &= ~ICO_APC_REQSTATE_WAITREQ;
        req->state |= ICO_APC_REQSTATE_REPLYACTIVE;
    }
    else    {
        req->state |= (ICO_APC_REQSTATE_WAITREQ | ICO_APC_REQSTATE_REPLYQUIET);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   recalc_inputsw: calculate all input switch request priority(static function)
 *
 * @param[in]   idx         input switch index number
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
recalc_inputsw(const int idx)
{
    ico_apc_inputsw_t   *czone = &inputsw[idx];
    ico_apc_request_t   *p;

    apfw_trace("recalc_inputsw: Enter(input=%s)", czone->inputsw->swname);

    /* get top of priority of this input    */
    p = czone->req;
    if (p == NULL) {
        /* no active request, end           */
        apfw_trace("recalc_inputsw: Leave(no request)");
        return;
    }
    if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0)   {
        /* not wait request                 */
        apfw_trace("recalc_inputsw: Leave(%s no need input control)", p->appid);
        return;
    }

    /* change to active for top priority*/
    p->state &= ~ICO_APC_REQSTATE_WAITPROC;
    if (p->prio & ICO_UXF_PRIO_REGULATION)  {
        apfw_trace("recalc_inputsw: Start %s(prio=%08x and no regulation)",
                   p->appid, p->prio);
        change_inputrequest(p, 1);
    }

    apfw_trace("recalc_inputsw: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   regulation_listener: change regulation callback(static function)
 *
 * @param[in]   appcategory     category Id
 * @param[in]   control         control(display/sound/input active/inactive)
 * @param[in]   user_data       user data(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
regulation_listener(const int appcategory,
                    const ico_apc_reguration_control_t control, void *user_data)
{
    int     i, j, k;
    int     flag;
    int     disp, sound, input;
    ico_apc_request_t   *p;
    ico_apc_request_t   *bp;
    ico_apc_request_t   *p2;
    ico_apc_request_t   *bp2;
    unsigned short  reqdisp[MAXREQ];
    unsigned short  reqsound[MAXREQ];
    unsigned short  reqinput[MAXREQ];

    disp = 0;
    sound = 0;
    input = 0;

    confapp = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();

    if (control.display != ICO_SYC_APC_REGULATION_NOCHANGE) {
        /* display regulation control       */
        apfw_trace("regulation_listener: disp category=%d display=%d",
                   appcategory, control.display);
        for (i = 0; i < confapp->applicationNum; i++)   {
            if (confapp->application[i].categoryId != appcategory)  continue;
            for (k = 0; k < ndispzone; k++) {
                p = dispzone[k].req;
                if (!p) continue;
                bp = NULL;
                while (p)   {
                    if (strcmp(p->appid, confapp->application[i].appid) == 0)    {
                        flag = 0;
                        if (control.display == ICO_SYC_APC_REGULATION_REGULATION)   {
                            if (p->prio & ICO_UXF_PRIO_REGULATION)  {
                                p->prio &= ~ICO_UXF_PRIO_REGULATION;
                                flag ++;
                                ico_uxf_window_control(p->appid, p->id,
                                                       ICO_UXF_APPSCTL_REGULATION, 1);
                            }
                        }
                        else    {
                            if ((p->prio & ICO_UXF_PRIO_REGULATION) == 0)   {
                                p->prio |= ICO_UXF_PRIO_REGULATION;
                                flag ++;
                                ico_uxf_window_control(p->appid, p->id,
                                                       ICO_UXF_APPSCTL_REGULATION, 0);
                            }
                        }
                        if (flag)   {
                            for (j = 0; j < disp; j++)  {
                                if (dispzone[k].conf->display->id == reqdisp[j])    break;
                            }
                            if (j >= disp)  {
                                reqdisp[disp++] = dispzone[k].conf->display->id;
                            }
                            apfw_trace("regulation_listener: disp %s %08x",
                                       p->appid, p->prio);
                            if (bp) {
                                bp->next = p->next;
                            }
                            else    {
                                dispzone[k].req = p->next;
                            }
                            p2 = dispzone[k].req;
                            bp2 = NULL;
                            while (p2)  {
                                if (p2->prio < p->prio) {
                                    if (bp2)    {
                                        bp2->next = p;
                                        p->next = p2;
                                        if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                            change_disprequest(p, 0);
                                        }
                                    }
                                    else    {
                                        p->next = dispzone[k].req;
                                        dispzone[k].req = p;
                                        if ((p->next != NULL) &&
                                            ((p->next->state & ICO_APC_REQSTATE_WAITREQ)
                                                == 0)) {
                                            change_disprequest(p->next, 0);
                                        }
                                    }
                                    break;
                                }
                                bp2 = p2;
                                p2 = p2->next;
                            }
                            if (! p2)   {
                                if (bp2)    {
                                    bp2->next = p;
                                    if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                        change_disprequest(p, 0);
                                    }
                                }
                                else    {
                                    dispzone[k].req = p;
                                }
                                p->next = NULL;
                            }
                            break;
                        }
                    }
                    bp = p;
                    p = p->next;
                }
            }
        }
    }

    if (control.sound != ICO_SYC_APC_REGULATION_NOCHANGE) {
        /* sound regulation control         */
        apfw_trace("regulation_listener: sound category=%d sound=%d",
                   appcategory, control.sound);
        for (i = 0; i < confapp->applicationNum; i++)   {
            if (confapp->application[i].categoryId != appcategory)  continue;
            for (k = 0; k < nsoundzone; k++)    {
                p = soundzone[k].req;
                if (!p) continue;
                bp = NULL;
                while (p)   {
                    if (strcmp(p->appid, confapp->application[i].appid) == 0)    {
                        flag = 0;
                        if (control.sound == ICO_SYC_APC_REGULATION_REGULATION) {
                            if (p->prio & ICO_UXF_PRIO_REGULATION)  {
                                p->prio &= ~ICO_UXF_PRIO_REGULATION;
                                flag ++;
                                if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                    change_soundrequest(p, 0);
                                }
                            }
                        }
                        else    {
                            if ((p->prio & ICO_UXF_PRIO_REGULATION) == 0)   {
                                p->prio |= ICO_UXF_PRIO_REGULATION;
                                flag ++;
                            }
                        }
                        if (flag)   {
                            apfw_trace("regulation_listener: sound change category=%d "
                                       "app(%s) sound=%d prio=%08x",
                                       appcategory, p->appid, control.sound, p->prio);
                            for (j = 0; j < sound; j++)  {
                                if (soundzone[k].conf->sound->id == reqsound[j])    break;
                            }
                            if (j >= sound)  {
                                reqsound[sound++] = soundzone[k].conf->sound->id;
                            }
                            apfw_trace("regulation_listener: sound %s %08x %08x",
                                       p->appid, p->prio, (int)bp);
                            if (bp) {
                                bp->next = p->next;
                            }
                            else    {
                                soundzone[k].req = p->next;
                            }
                            p2 = soundzone[k].req;
                            bp2 = NULL;
                            while (p2)  {
                                if (p2->prio < p->prio) {
                                    if (bp2)    {
                                        bp2->next = p;
                                        p->next = p2;
                                        if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                            change_soundrequest(p, 0);
                                        }
                                    }
                                    else    {
                                        p->next = soundzone[k].req;
                                        soundzone[k].req = p;
                                        if ((p->next != NULL) &&
                                            ((p->next->state & ICO_APC_REQSTATE_WAITREQ)
                                                == 0)) {
                                            change_soundrequest(p->next, 0);
                                        }
                                    }
                                    break;
                                }
                                bp2 = p2;
                                p2 = p2->next;
                            }
                            if (! p2)   {
                                if (bp2)    {
                                    bp2->next = p;
                                    if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                        change_soundrequest(p, 0);
                                    }
                                }
                                else    {
                                    soundzone[k].req = p;
                                }
                                p->next = NULL;
                            }
                            break;
                        }
                    }
                    bp = p;
                    p = p->next;
                }
            }
        }
    }

    if (control.input != ICO_SYC_APC_REGULATION_NOCHANGE)   {
        /* input reguration control     */
        apfw_trace("regulation_listener: input category=%d input=%d",
                   appcategory, control.input);
        for (i = 0; i < confapp->applicationNum; i++)   {
            if (confapp->application[i].categoryId != appcategory)  continue;
            for (k = 0; k < ninputsw; k++)    {
                p = inputsw[k].req;
                if (!p) continue;
                bp = NULL;
                while (p)   {
                    if (strcmp(p->appid, confapp->application[i].appid) == 0)    {
                        flag = 0;
                        if (control.input == ICO_SYC_APC_REGULATION_REGULATION) {
                            if (p->prio & ICO_UXF_PRIO_REGULATION)  {
                                p->prio &= ~ICO_UXF_PRIO_REGULATION;
                                flag ++;
                                if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                    change_inputrequest(p, 0);
                                }
                            }
                        }
                        else    {
                            if ((p->prio & ICO_UXF_PRIO_REGULATION) == 0)   {
                                p->prio |= ICO_UXF_PRIO_REGULATION;
                                flag ++;
                            }
                        }
                        if (flag)   {
                            apfw_trace("regulation_listener: input change category=%d "
                                       "app(%s) input=%d prio=%08x",
                                       appcategory, p->appid, control.input, p->prio);
                            for (j = 0; j < sound; j++)  {
                                if (inputsw[k].inputsw->input == reqinput[j])    break;
                            }
                            if (j >= input)  {
                                reqinput[input++] = inputsw[k].inputsw->input;
                            }
                            apfw_trace("regulation_listener: input %s %08x %08x",
                                       p->appid, p->prio, (int)bp);
                            if (bp) {
                                bp->next = p->next;
                            }
                            else    {
                                inputsw[k].req = p->next;
                            }
                            p2 = inputsw[k].req;
                            bp2 = NULL;
                            while (p2)  {
                                if (p2->prio < p->prio) {
                                    if (bp2)    {
                                        bp2->next = p;
                                        p->next = p2;
                                        if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                            change_inputrequest(p, 0);
                                        }
                                    }
                                    else    {
                                        p->next = inputsw[k].req;
                                        inputsw[k].req = p;
                                    }
                                    break;
                                }
                                bp2 = p2;
                                p2 = p2->next;
                            }
                            if (! p2)   {
                                if (bp2)    {
                                    bp2->next = p;
                                    if ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0) {
                                        change_inputrequest(p, 0);
                                    }
                                }
                                else    {
                                    inputsw[k].req = p;
                                }
                                p->next = NULL;
                            }
                            break;
                        }
                    }
                    bp = p;
                    p = p->next;
                }
            }
        }
    }

    /* re-calculate display zone, sound zone and inout switch priority  */
    for (i = 0; i < disp; i++)  {
        recalc_dispzone(reqdisp[i]);
    }
    for (i = 0; i < sound; i++) {
        recalc_soundzone(reqsound[i]);
    }
    for (i = 0; i < input; i++) {
        recalc_inputsw(reqinput[i]);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   request_timer: request timedout timer(static function)
 *
 * @param[in]   user_data       user data(unused)
 * @return      always ECORE_CALLBACK_RENEW(periodic timer)
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
request_timer(void *user_data)
{
    int                 i;
    ico_apc_request_t   *p;

    if (timer_count <= 0)   {
        /* no need timedout check   */
        return ECORE_CALLBACK_RENEW;
    }
    apfw_trace("request_timer: start(%d)", timer_count);
    timer_count = 0;

    /* check display request timedout   */
    for (i = 0; i < ndispzone; i++) {
        p = dispzone[i].req;
        while (p)   {
            if (p->timer > 0)   {
                if (p->timer >= ICO_APC_REQREPLY_INTERVAL)
                    p->timer -= ICO_APC_REQREPLY_INTERVAL;
                else
                    p->timer = 0;
                apfw_trace("request_timer: dispzone[%d] timer(%d)", i, p->timer);
                if (p->timer == 0)  {
                    apfw_trace("request_timer: display timedout(%s %d %d prio=%08x)",
                               p->appid, p->resid, p->id, p->prio);
                    if ((p->state & ICO_APC_REQSTATE_REPLYACTIVE) &&
                        ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
                        ico_uxf_window_control(p->appid, p->id,
                                               ICO_UXF_APPSCTL_INVISIBLE, 0);
                        if (displaycontrol) {
                            (*displaycontrol)(get_appconf(p->appid), 1);
                        }
                    }
                    p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                }
                else    {
                    timer_count ++;
                }
            }
            p = p->next;
        }
    }

    /* check sound request timedout */
    for (i = 0; i < nsoundzone; i++) {
        p = soundzone[i].req;
        while (p)   {
            if (p->timer > 0)   {
                if (p->timer >= ICO_APC_REQREPLY_INTERVAL)
                    p->timer -= ICO_APC_REQREPLY_INTERVAL;
                else
                    p->timer = 0;
                apfw_trace("request_timer: soundzone[%d] timer(%d)", i, p->timer);
                if (p->timer == 0)  {
                    apfw_trace("request_timer: sound timedout(%s %d %d prio=%08x)",
                               p->appid, p->resid, p->id, p->prio);
                    if ((p->state & ICO_APC_REQSTATE_REPLYACTIVE) &&
                        ((p->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
                        if (ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_MUTEOFF,
                                                              p->pid)
                                != ICO_APF_RESOURCE_E_NONE) {
                            apfw_warn("request_timer: send MSM Error");
                        }
                        if (soundcontrol) {
                            (*soundcontrol)(get_appconf(p->appid), 1);
                        }
                    }
                    p->state &= ~(ICO_APC_REQSTATE_REPLYACTIVE|ICO_APC_REQSTATE_REPLYQUIET);
                }
                else    {
                    timer_count ++;
                }
            }
            p = p->next;
        }
    }
    apfw_trace("request_timer: end(%d)", timer_count);
    return ECORE_CALLBACK_RENEW;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_is_waitshow: check if application is waiting show
 *
 * @param[in]   appid           application Id
 * @return      answer
 * @retval      =1              waiting show
 * @retval      =0              not wait
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_is_waitshow(const char *appid)
{
    int                 i;
    ico_apc_request_t   *p;

    /* check display request list   */
    for (i = 0; i < ndispzone; i++) {
        p = dispzone[i].req;
        while (p)   {
            if (p->timer > 0)   {
                /* waiting show         */
                if (strcmp(p->appid, appid) == 0)   {
                    /* found application    */
                    uifw_trace("ico_syc_apc_is_waitshow: %s is waiting", appid);
                    return 1;
                }
            }
            p = p->next;
        }
    }
    uifw_trace("ico_syc_apc_is_waitshow: %s is not wait", appid);
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_active: application change to active
 *
 * @param[in]   appid           application Id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_apc_active(const char *appid)
{
    int     i, j;
    int     disp, sound, input;
    int     flag;
    Ico_Uxf_conf_application    *appconf;
    char    *child_appid;
    ico_apc_request_t   *p;
    ico_apc_request_t   *bp;
    ico_apc_request_t   *p2;
    ico_apc_request_t   *bp2;
    unsigned short  reqdisp[MAXREQ];
    unsigned short  reqsound[MAXREQ];
    unsigned short  reqinput[MAXREQ];

    apfw_trace("ico_syc_apc_active: Enter(%s)", appid ? appid : "(NULL)");

    if ((appid != NULL) && (*appid != 0))   {
        /* get active application           */
        appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByAppid(appid);
        if (! appconf)  {
            apfw_trace("ico_syc_apc_active: appid(%s) dose not exist", appid);
        }
    }
    else    {
        /* chane to no active application   */
        appconf = NULL;
    }

    if (appconf)    {
        /* save child process if exist          */
        child_appid = ico_uxf_getchild_appid(appconf->appid);
        /* set last active process for child process    */
        ico_uxf_set_lastapp(appconf->appid);
    }
    else    {
        /* reset last active process for child process  */
        ico_uxf_set_lastapp(NULL);
        child_appid = NULL;
    }

    /* change all screen request from this application  */
    disp = 0;
    for (i = 0; i < ndispzone; i++) {
        p = dispzone[i].req;
        flag = 0;
        bp = NULL;
        while (p)   {
            if (appconf && (strcmp(p->appid, appconf->appid) == 0)) {
                apfw_trace("ico_syc_apc_active: disp %s prio=%08x is %s",
                           p->appid, p->prio, bp ? "not top" : "top");
                if ((p->prio & ICO_UXF_PRIO_ACTIVEAPP) != ICO_UXF_PRIO_ACTIVEAPP)   {
                    p->prio |= ICO_UXF_PRIO_ACTIVEAPP;
                    apfw_trace("ico_syc_apc_active: cgange active %s prio to %08x",
                               p->appid, p->prio);
                    flag ++;

                    if (bp) {
                        bp->next = p->next;
                        p2 = dispzone[i].req;
                        bp2 = NULL;
                        while (p2)  {
                            if (p2->prio <= p->prio)    {
                                p->next = p2;
                                if (bp2) {
                                    apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                               p->appid, dispzone[i].req->appid,
                                               dispzone[i].req->prio);
                                    bp2->next = p;
                                }
                                else    {
                                    if (appconf && (strcmp(p2->appid, appconf->appid) != 0) &&
                                        ((p2->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
                                        change_disprequest(p2, 0);
                                    }
                                    dispzone[i].req = p;
                                }
                                break;
                            }
                            bp2 = p2;
                            p2 = p2->next;
                        }
                        if (! p2)   {
                            apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                       p->appid, dispzone[i].req->appid,
                                       dispzone[i].req->prio);
                            if (bp2)    {
                                bp2->next = p;
                            }
                            else    {
                                dispzone[i].req = p;
                            }
                            p->next = NULL;
                        }
                    }
                    else    {
                        apfw_trace("ico_syc_apc_active: app %s is top", p->appid);
                    }
                }
            }
            bp = p;
            p = p->next;
        }
        if (flag)   {
            p = dispzone[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }

            for (j = 0; j < disp; j++)  {
                if (dispzone[i].conf->display->id == reqdisp[j])    break;
            }
            if (j >= disp)  {
                reqdisp[disp++] = dispzone[i].conf->display->id;
            }
        }
    }

    /* change all sound request from this application   */
    sound = 0;
    for (i = 0; i < nsoundzone; i++) {
        p = soundzone[i].req;
        flag = 0;
        bp = NULL;
        while (p)   {
            if (appconf && (strcmp(p->appid, appconf->appid) == 0)) {
                apfw_trace("ico_syc_apc_active: sound %s prio=%08x is %s",
                           p->appid, p->prio, bp ? "not top" : "top");
                if ((p->prio & ICO_UXF_PRIO_ACTIVEAPP) != ICO_UXF_PRIO_ACTIVEAPP)   {
                    p->prio |= ICO_UXF_PRIO_ACTIVEAPP;
                    apfw_trace("ico_syc_apc_active: cgange active %s prio to %08x",
                               p->appid, p->prio);
                    flag ++;

                    if (bp) {
                        bp->next = p->next;
                        p2 = soundzone[i].req;
                        bp2 = NULL;
                        while (p2)  {
                            if (p2->prio <= p->prio)    {
                                p->next = p2;
                                if (bp2) {
                                    apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                               p->appid, soundzone[i].req->appid,
                                               soundzone[i].req->prio);
                                    bp2->next = p;
                                }
                                else    {
                                    if (appconf && (strcmp(p2->appid, appconf->appid) != 0) &&
                                        ((p2->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
                                        change_soundrequest(p2, 0);
                                    }
                                    soundzone[i].req = p;
                                }
                                break;
                            }
                            bp2 = p2;
                            p2 = p2->next;
                        }
                        if (! p2)   {
                            apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                       p->appid, soundzone[i].req->appid,
                                       soundzone[i].req->prio);
                            if (bp2)    {
                                bp2->next = p;
                            }
                            else    {
                                soundzone[i].req = p;
                            }
                            p->next = NULL;
                        }
                    }
                    else    {
                        apfw_trace("ico_syc_apc_active: app %s is top", p->appid);
                    }
                }
            }
            bp = p;
            p = p->next;
        }
        if (flag)   {
            p = soundzone[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }

            for (j = 0; j < sound; j++) {
                if (soundzone[i].conf->sound->id == reqsound[j])    break;
            }
            if (j >= sound) {
                reqsound[sound++] = soundzone[i].conf->sound->id;
            }
        }
    }

    /* change all inputsw request from this application */
    input = 0;
    for (i = 0; i < ninputsw; i++) {
        p = inputsw[i].req;
        flag = 0;
        bp = NULL;
        while (p)   {
            if (appconf && (strcmp(p->appid, appconf->appid) == 0)) {
                if ((p->prio & ICO_UXF_PRIO_ACTIVEAPP) != ICO_UXF_PRIO_ACTIVEAPP)   {
                    p->prio |= ICO_UXF_PRIO_ACTIVEAPP;
                    apfw_trace("ico_syc_apc_active: cgange active %s prio to %08x",
                               p->appid, p->prio);
                    flag ++;

                    if (bp) {
                        bp->next = p->next;
                        p2 = inputsw[i].req;
                        bp2 = NULL;
                        while (p2)  {
                            if (p2->prio <= p->prio)    {
                                p->next = p2;
                                if (bp2) {
                                    apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                               p->appid, inputsw[i].req->appid,
                                               inputsw[i].req->prio);
                                    bp2->next = p;
                                }
                                else    {
                                    if (appconf && (strcmp(p2->appid, appconf->appid) != 0) &&
                                        ((p2->state & ICO_APC_REQSTATE_WAITREQ) == 0))  {
                                        change_inputrequest(p2, 0);
                                    }
                                    inputsw[i].req = p;
                                }
                                break;
                            }
                            bp2 = p2;
                            p2 = p2->next;
                        }
                        if (! p2)   {
                            apfw_trace("ico_syc_apc_active: %s is not top=%s(%08x)",
                                       p->appid, inputsw[i].req->appid,
                                       inputsw[i].req->prio);
                            if (bp2)    {
                                bp2->next = p;
                            }
                            else    {
                                inputsw[i].req = p;
                            }
                            p->next = NULL;
                        }
                    }
                    else    {
                        apfw_trace("ico_syc_apc_active: app %s is top", p->appid);
                    }
                }
            }
            bp = p;
            p = p->next;
        }
        if (flag)   {
            p = inputsw[i].req;
            while (p)   {
                if (p->prio & ICO_UXF_PRIO_ACTIVEAPP)  {
                    p->prio -= ICO_UXF_PRIO_ACTIVECOUNT;
                }
                p = p->next;
            }

            for (j = 0; j < input; j++) {
                if (inputsw[i].inputsw->input == reqinput[j])   break;
            }
            if (j >= input) {
                reqinput[input++] = inputsw[i].inputsw->input;
            }
        }
    }

    /* re-calculate display zone, sound zone and inout switch priority  */
    for (i = 0; i < disp; i++)  {
        recalc_dispzone(reqdisp[i]);
    }
    for (i = 0; i < sound; i++) {
        recalc_soundzone(reqsound[i]);
    }
    for (i = 0; i < input; i++) {
        recalc_inputsw(reqinput[i]);
    }
    if (child_appid)    {
        apfw_trace("ico_syc_apc_active: active child app(%s)", child_appid);
        ico_syc_apc_active(child_appid);
    }
    apfw_trace("ico_syc_apc_active: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   app_display_hook: hook function for surface create/destroy(static function)
 *
 * @param[in]   appid           application Id
 * @param[in]   surface         surface Id
 * @param[in]   object          target surface type and operation
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
app_display_hook(const char *appid, const int surface, const int object)
{
    int     i;
    int     count;
    ico_apc_request_t           *reqsave[MAXREQ];
    Ico_Uxf_conf_application    *appconf;
    ico_apc_request_t           *req;

    apfw_trace("app_display_hook: Enter(%s,%08x,%x)", appid, surface, object);

    appconf = (Ico_Uxf_conf_application *)ico_uxf_getAppByAppid(appid);
    if (! appconf)  {
        apfw_warn("app_display_hook: Leave(appid[%s] dose not exist)", appid);
        return;
    }
    if ((confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_ALMIGHTY) ||
        (confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_SYSTEM) ||
        (confsys->kind[appconf->kindId].priv == ICO_UXF_PRIVILEGE_SYSTEM_VISIBLE))  {
        /* System Program(ex. HomeScreen) no need resource control  */
        apfw_trace("app_display_hook: Leave(appid[%s] is system program)", appid);
        return;
    }

    switch (object) {
    case ICO_UXF_HOOK_WINDOW_CREATE_MAIN:   /* created application main window  */
        /* search display request   */
        for (i = 0; i < ndispzone; i++) {
            req = dispzone[i].req;
            while (req)   {
                if ((strcmp(req->appid, appconf->appid) == 0) &&
                    ((req->resid == ICO_APF_RESID_BASIC_SCREEN) ||
                     (req->resid == ICO_APF_RESID_ON_SCREEN)))  break;
                req = req->next;
            }
            if (req)    break;
        }
        if (i < ndispzone)  {
            apfw_trace("app_display_hook: app(%s) requested display, Nop", appid);
        }
        else    {
            apfw_trace("app_display_hook: app(%s) not requested display, set default[%s]",
                       appid,
                       confsys->display[appconf->display[0].displayId].
                           zone[appconf->display[0].zoneId].name);
            req = get_freereq();
            if (req)    {
                strncpy(req->appid, appconf->appid, ICO_UXF_MAX_PROCESS_NAME);
                req->resid = ICO_APF_RESID_BASIC_SCREEN;
                req->reqtype = ICO_APC_REQTYPE_CREATE;
                strcpy(req->device,
                       confsys->display[appconf->display[0].displayId].
                           zone[appconf->display[0].zoneId].name);
                app_getdisplay(req, 0);
            }
        }
        break;
    case ICO_UXF_HOOK_WINDOW_DESTORY_MAIN:  /* destoryed application main window*/
        /* delete all request from this application */
        count = 0;
        for (i = 0; i < ndispzone; i++) {
            req = dispzone[i].req;
            while (req)   {
                if (strcmp(req->appid, appconf->appid) == 0) {
                    reqsave[count++] = req;
                }
                req = req->next;
            }
        }
        if (count > 0)  {
            apfw_trace("app_display_hook: free app(%s) all display request", appid);
            for (i = 0; i < count; i++) {
                app_freedisplay(reqsave[i], 0);
            }
        }
        break;
    default:                                /* other, Nop   */
        break;
    }

    apfw_trace("app_display_hook: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_init: initialize application controller
 *
 * @param[in]   display     callback function for display control
 * @param[in]   sound       callback function for sound control
 * @param[in]   input       callback function for input control
 * @return      result
 * @retval      ICO_SYC_EOK     success
 * @retval      ICO_SYC_ENOSYS  error(system error)
 * @retval      ICO_SYC_ENOMEM  error(out of memory)
 */
/*--------------------------------------------------------------------------*/
int
ico_syc_apc_init(ico_apc_resource_control_t display, ico_apc_resource_control_t sound,
                 ico_apc_resource_control_t input)
{
    int     count;
    int     base_count;
    int     i, j, k;
    int     ret;
    Ico_Uxf_InputDev    *pdev;
    Ico_Uxf_InputSw     *psw;

    apfw_trace("ico_syc_apc_init: Enter");

    if (ico_apps_controller_init)   {
        apfw_trace("ico_syc_apc_init: Leave(OK, initialized)");
        return ICO_SYC_EOK;
    }

    /* set callback functions for HomeScreen        */
    displaycontrol = display;
    soundcontrol = sound;
    inputcontrol = input;

    ico_apps_controller_init = 1;

    /* get configurations                           */
    confsys = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    confapp = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();

    if ((! confsys) || (! confapp)) {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(can not read configuration)");
        return ICO_SYC_ENOSYS;
    }

    /* initialize internal tables                   */
    /* display zone table                           */
    ndispzone = 0;
    for (i = 0; i < confsys->displayNum; i++)   {
        ndispzone += confsys->display[i].zoneNum;
    }
    dispzone = malloc(sizeof(ico_apc_dispzone_t) * ndispzone);
    if (! dispzone) {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(No Memory)");
        return ICO_SYC_ENOMEM;
    }
    memset(dispzone, 0, sizeof(ico_apc_dispzone_t) * ndispzone);
    count = 0;
    for (i = 0; i < confsys->displayNum; i++)   {
        base_count = count;
        for (j = 0; j < confsys->display[i].zoneNum; j++)   {
            dispzone[count].conf = &confsys->display[i].zone[j];
            dispzone[count].noverlap = confsys->display[i].zone[j].overlapNum;
            for (k = 0; k < dispzone[count].noverlap; k++)  {
                dispzone[count].overlap[k] =
                    &dispzone[base_count + confsys->display[i].zone[j].overlap[k]];
            }
            count ++;
        }
    }

    /* sound zone table                         */
    nsoundzone = 0;
    for (i = 0; i < confsys->soundNum; i++) {
        nsoundzone += confsys->sound[i].zoneNum;
    }
    soundzone = malloc(sizeof(ico_apc_soundzone_t) * nsoundzone);
    if (! soundzone)    {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(No Memory)");
        return ICO_SYC_ENOMEM;
    }
    memset(soundzone, 0, sizeof(ico_apc_soundzone_t) * nsoundzone);
    count = 0;
    for (i = 0; i < confsys->soundNum; i++) {
        base_count = count;
        for (j = 0; j < confsys->sound[i].zoneNum; j++) {
            soundzone[count].conf = &confsys->sound[i].zone[j];
            soundzone[count].noverlap = confsys->sound[i].zone[j].overlapNum;
            for (k = 0; k < soundzone[count].noverlap; j++) {
                soundzone[count].overlap[k] =
                    &soundzone[base_count + confsys->sound[i].zone[j].overlap[k]];
            }
            count ++;
        }
    }
    /* input sw table                           */
    ninputsw = 0;
    for (i = 0; ; i++)  {
        pdev = ico_uxf_inputdev_attribute_get(i);
        if (pdev == NULL)   break;
        apfw_trace("ico_syc_apc_init: input device.%d %s has %d switchs",
                   i, pdev->device, pdev->numInputSw);
        for (j = 0; j < pdev->numInputSw; j++)  {
            psw = ico_uxf_inputsw_attribute_get(pdev, j);
            if (psw == NULL)    break;
            if (psw->fix == 0)  {
                ninputsw ++;
            }
        }
    }
    if (ninputsw > 0)   {
        inputsw = malloc(sizeof(ico_apc_inputsw_t) * ninputsw);
        if (! inputsw)  {
            ico_apps_controller_init = 0;
            apfw_error("ico_syc_apc_init: Leave(No Memory)");
            return ICO_SYC_ENOMEM;
        }
        memset(inputsw, 0, sizeof(ico_apc_inputsw_t) * ninputsw);
        count = 0;
        for (i = 0; ; i++)  {
            pdev = ico_uxf_inputdev_attribute_get(i);
            if (pdev == NULL)   break;
            for (j = 0; j < pdev->numInputSw; j++)  {
                psw = ico_uxf_inputsw_attribute_get(pdev, j);
                if (psw == NULL)    break;
                if (psw->fix)       continue;
                inputsw[count].inputdev = pdev;
                inputsw[count].inputsw = psw;
                count ++;
            }
        }
    }

    /* initialize request table                     */
    ico_apc_request_t *req = malloc(sizeof(ico_apc_request_t) * INIT_REQCB);
    ico_apc_request_t *breq;
    if (! req)  {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(No Memory)");
        return ICO_SYC_ENOMEM;
    }
    memset(req, 0, sizeof(ico_apc_request_t) * INIT_REQCB);
    free_request = req;
    for (count = 1; count < INIT_REQCB; count++)    {
        breq = req;
        req ++;
        breq->next = req;
    }

    /* initialzie application framework library     */
    ret = ico_apf_ecore_init_server(NULL);
    if (ret != ICO_APF_E_NONE)  {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(ico_apf_ecore_init_server Error<%d>)", ret);
        return ICO_SYC_ENOSYS;
    }

    /* regist callback for application resource request */
    ret = ico_apf_resource_set_event_cb(resource_reqcb, NULL);
    if (ret != ICO_APF_RESOURCE_E_NONE) {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(ico_apf_resource_set_event_cb Error<%d>)",
                   ret);
        ico_apf_resource_term_server();
        return ICO_SYC_ENOSYS;
    }

    /* initialize regulation controller    */
    ret = ico_syc_apc_regulation_init();
    if (ret != ICO_SYC_EOK) {
        ico_apps_controller_init = 0;
        apfw_error("ico_syc_apc_init: Leave(ico_syc_apc_regulation_init Error<%d>)",
                   ret);
        ico_apf_resource_term_server();
        return ICO_SYC_ENOSYS;
    }

    ico_syc_apc_regulation_listener(regulation_listener, NULL);

    /* set hook for window create/destory   */
    (void) ico_uxf_window_hook(app_display_hook);

    /* create timer     */
    ecore_timer = ecore_timer_add(((double)ICO_APC_REQREPLY_INTERVAL)/1000.0,
                                  request_timer, NULL);
    timer_count = 1;

    /* send sound stream list request to Multi Sound manager    */
    ret = ico_apf_resource_send_to_soundctl(ICO_APF_SOUND_COMMAND_GETLIST, 0);
    if (ret != ICO_APF_RESOURCE_E_NONE) {
        apfw_error("ico_syc_apc_init: Leave(ico_apf_resource_send_to_soundctl Error<%d>)",
                   ret);
        return ICO_SYC_ENOSYS;
    }

    apfw_trace("ico_syc_apc_init: Leave(EOK)");
    return ICO_SYC_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_apc_term: terminate application controller
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_syc_apc_term(void)
{
    apfw_trace("ico_syc_apc_term: Enter");
    if (ico_apps_controller_init == 0)  {
        apfw_trace("ico_syc_apc_term: Leave(not initialized)");
        return;
    }
    if (ecore_timer)    {
        ecore_timer_del(ecore_timer);
        ecore_timer = NULL;
    }

    ico_syc_apc_regulation_term();
    ico_apf_resource_term_server();
    ico_apps_controller_init = 0;

    apfw_trace("ico_syc_apc_term: Leave");
}

