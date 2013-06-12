/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library(initialize/terminate/event)
 *
 * @date    Feb-28-2013
 */

#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <string.h>
#include    <errno.h>
#include    <pthread.h>
#include    <sys/ioctl.h>
#include    <sys/time.h>
#include    <fcntl.h>

#include    "wayland-client.h"
#include    "ico_window_mgr-client-protocol.h"
#include    "ico_uxf.h"
#include    "ico_uxf_conf.h"
#include    "ico_uxf_private.h"

/* static functions             */
/* wayland standard callback functions      */
static void ico_uxf_wayland_globalcb(void *data, struct wl_registry *registry,
                                     uint32_t name, const char *interface,
                                     uint32_t version);
static void ico_uxf_output_geometrycb(void *data, struct wl_output *wl_output,
                                      int32_t x, int32_t y, int32_t physical_width,
                                      int32_t physical_height, int32_t subpixel,
                                      const char *make, const char *model,
                                      int32_t transform);
static void ico_uxf_output_modecb(void *data, struct wl_output *wl_output,
                                  uint32_t flags, int32_t width, int32_t height,
                                  int32_t refresh);

/* ico_window_mgr(Multi Window Manager) callback functions  */
struct ico_window;
static void ico_uxf_window_createdcb(void *data, struct ico_window_mgr *ico_window_mgr,
                                     uint32_t surfaceid, int32_t pid, const char *appid);
static void ico_uxf_window_destroyedcb(void *data, struct ico_window_mgr *ico_window_mgr,
                                       uint32_t surfaceid);
static void ico_uxf_window_visiblecb(void *data, struct ico_window_mgr *ico_window_mgr,
                                     uint32_t surfaceid, int32_t visible, int32_t raise,
                                     int32_t hint);
static void ico_uxf_window_configurecb(void *data, struct ico_window_mgr *ico_window_mgr,
                                       uint32_t surfaceid, const char *appid,
                                       int32_t layer, int32_t x, int32_t y,
                                       int32_t width, int32_t height, int32_t hint);
static void ico_uxf_window_activecb(void *data,
                                    struct ico_window_mgr *ico_window_mgr,
                                    uint32_t surfaceid,
                                    uint32_t active);

/* ico_input_mgr(Multi Input Manager) callback functions    */
static void ico_uxf_input_capabilitiescb(void *data, struct ico_exinput *ico_exinput,
                                         const char *device, int32_t type,
                                         const char *swname, int32_t input,
                                         const char *codename, int32_t code);
static void ico_uxf_input_codecb(void *data, struct ico_exinput *ico_exinput,
                                 const char *device, int32_t input, const char *codename,
                                 int32_t code);
static void ico_uxf_input_inputcb(void *data, struct ico_exinput *ico_exinput,
                                  uint32_t time, const char *device, int32_t input,
                                  int32_t code, int32_t state);

/* AppCore(AUL) callback function   */
static int ico_uxf_aul_aulcb(int pid, void *data);

/* Variables & Tables               */
Ico_Uxf_Api_Mng         gIco_Uxf_Api_Mng = { 0 };

static pthread_mutex_t  sMutex;

/* Wayland Registry Listener */
static const struct wl_registry_listener ico_uxf_registry_listener = {
    ico_uxf_wayland_globalcb
};

/* Window Manger Interface */
static const struct ico_window_mgr_listener     windowlistener = {
    ico_uxf_window_createdcb,
    ico_uxf_window_destroyedcb,
    ico_uxf_window_visiblecb,
    ico_uxf_window_configurecb,
    ico_uxf_window_activecb
};

/* Input Manger Interface */
static const struct ico_exinput_listener      exinputlistener = {
    ico_uxf_input_capabilitiescb,
    ico_uxf_input_codecb,
    ico_uxf_input_inputcb
};

/* Wayland Output interface */
static const struct wl_output_listener outputlistener = {
    ico_uxf_output_geometrycb,
    ico_uxf_output_modecb
};


/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_init: initialize user interface library hor HomeScreen
 *
 * @param[in]   name            application id
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_EBUSY   error(cullentry terminating)
 * @retval      ICO_UXF_ESRCH   error(configuration error)
 * @retval      ICO_UXF_ENOSYS  error(system error)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_init(const char *name)
{
    Ico_Uxf_Mng_Display *dsp;
    Ico_Uxf_Mng_Layer   *lay;
    Ico_Uxf_Mng_Process *prc;
    pthread_mutexattr_t sMutexAttr;
    Ico_Uxf_Sys_Config *sysconf;
    Ico_Uxf_App_Config *appconf;
    Ico_Uxf_conf_appdisplay *appdsp;
    Ico_Uxf_Mng_Callback    *freecb;
    Ico_Uxf_Mng_EventQue    *freeeq;
    int         dn, tn;
    int         ret;

    ico_uxf_log_open(name);

    if (gIco_Uxf_Api_Mng.Initialized)  {
        if (gIco_Uxf_Api_Mng.Initialized < 0)  {
            uifw_trace("ico_uxf_init: Enter");
            uifw_warn("ico_uxf_init: Leave(EBUSY)");
            return ICO_UXF_EBUSY;
        }
        uifw_trace("ico_uxf_init: Enter");
        uifw_trace("ico_uxf_init: Leave(EOK)");
        return ICO_UXF_EOK;
    }
    uifw_trace("ico_uxf_init: Enter");

    gIco_Uxf_Api_Mng.Initialized = -1;
    memset(gIco_Uxf_Api_Mng.MyProcess, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
    strncpy(gIco_Uxf_Api_Mng.MyProcess, name, ICO_UXF_MAX_PROCESS_NAME);
    gIco_Uxf_Api_Mng.EventMask = 0;
    gIco_Uxf_Api_Mng.WaylandFd = -1;
    gIco_Uxf_Api_Mng.Wayland_Display = NULL;
    gIco_Uxf_Api_Mng.InitTimer = ICO_UXF_SUSP_INITTIME;
    gIco_Uxf_Api_Mng.Win_Show_Anima = 1;
    gIco_Uxf_Api_Mng.Win_Hide_Anima = 1;

    /* read configurations */
    sysconf = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    appconf = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();
    if ((sysconf == NULL) || (appconf == NULL)) {
        uifw_error("ico_uxf_init: Leave(ESRCH), Configuration Read Error");
        return ICO_UXF_ESRCH;
    }

    uifw_trace("ico_uxf_init: set display configurations");
    /* set display configurations */
    for (dn = 0; dn < sysconf->displayNum; dn++) {
        dsp = ico_uxf_mng_display(sysconf->display[dn].id, 1);
        dsp->attr.type = sysconf->display[dn].type;
        dsp->attr.num_layers = sysconf->display[dn].layerNum;
        dsp->attr.hostId = sysconf->display[dn].hostId;
        dsp->attr.displayNo = sysconf->display[dn].displayno;
        dsp->attr.w = sysconf->display[dn].width;
        dsp->attr.h = sysconf->display[dn].height;
        dsp->attr.pWidth = -1;
        dsp->attr.pHeight = -1;
        dsp->attr.orientation = ICO_UXF_ORIENTATION_HORIZONTAL;
        dsp->attr.inch = sysconf->display[dn].inch;
        strncpy(dsp->attr.name, sysconf->display[dn].name, ICO_UXF_MAX_WIN_NAME);
        dsp->attr.name[ICO_UXF_MAX_WIN_NAME] = 0;

        for (tn = 0; tn < sysconf->display[dn].layerNum; tn++) {
            lay = ico_uxf_mng_layer(sysconf->display[dn].id,
                                    sysconf->display[dn].layer[tn].id, 1);
            lay->attr.display = sysconf->display[dn].id;
            lay->attr.w = sysconf->display[dn].width;
            lay->attr.h = sysconf->display[dn].height;
            lay->attr.menuoverlap = sysconf->display[dn].layer[tn].menuoverlap;
            lay->mng_display = dsp;
        }
    }

    uifw_trace("ico_uxf_init: set application configurations(num=%d)",
               appconf->applicationNum);
    /* set application configurations */
    for(dn = 0; dn < appconf->applicationNum; dn++)  {
        prc = ico_uxf_mng_process(appconf->application[dn].appid, 1);
        prc->attr.internalid = 0;
        prc->appconf = (void *)&appconf->application[dn];
        prc->attr.status = ICO_UXF_PROCSTATUS_STOP;
        prc->attr.type = appconf->application[dn].categoryId;
        prc->attr.hostId = appconf->application[dn].hostId;
        prc->attr.myHost = (prc->attr.hostId == sysconf->misc.myhostId) ? 1 : 0;
        prc->attr.noicon = appconf->application[dn].noicon;
        prc->attr.autostart = appconf->application[dn].autostart;
        prc->attr.invisiblecpu = appconf->application[dn].invisiblecpu;

        appdsp = &appconf->application[dn].display[0];
        prc->attr.mainwin.window = 0;
        prc->attr.mainwin.windowtype = appconf->application[dn].categoryId;
        prc->attr.mainwin.display = appdsp->displayId;
        prc->attr.mainwin.layer = appdsp->layerId;
        prc->attr.mainwin.x =
                sysconf->display[appdsp->displayId].zone[appdsp->zoneId].x;
        prc->attr.mainwin.y =
                sysconf->display[appdsp->displayId].zone[appdsp->zoneId].y;
        prc->attr.mainwin.w =
                sysconf->display[appdsp->displayId].zone[appdsp->zoneId].width;
        prc->attr.mainwin.h =
                sysconf->display[appdsp->displayId].zone[appdsp->zoneId].height;
        prc->attr.mainwin.name[ICO_UXF_MAX_WIN_NAME] = 0;
        prc->attr.numwindows = appconf->application[dn].displayzoneNum;
        /* get sub windows                          */
        if (prc->attr.numwindows > 1)   {
            prc->attr.subwin = malloc(sizeof(Ico_Uxf_ProcessWin) *
                                      (prc->attr.numwindows - 1));
            if (! prc->attr.subwin) {
                uifw_error("ico_uxf_init: No Memory");
                appconf->application[dn].displayzoneNum = 1;
                prc->attr.numwindows = 1;
            }
            else    {
                memset(prc->attr.subwin, 0, (prc->attr.numwindows - 1));
                for (tn = 0; tn < (prc->attr.numwindows - 1); tn++) {
                    appdsp ++;
                    prc->attr.subwin[tn].windowtype = prc->attr.mainwin.windowtype;
                    prc->attr.subwin[tn].display = appdsp->displayId;
                    prc->attr.subwin[tn].layer = appdsp->layerId;
                    prc->attr.subwin[tn].x =
                        sysconf->display[appdsp->displayId].zone[appdsp->zoneId].x;
                    prc->attr.subwin[tn].y =
                        sysconf->display[appdsp->displayId].zone[appdsp->zoneId].y;
                    prc->attr.subwin[tn].w =
                        sysconf->display[appdsp->displayId].zone[appdsp->zoneId].width;
                    prc->attr.subwin[tn].h =
                        sysconf->display[appdsp->displayId].zone[appdsp->zoneId].height;
                }
            }
        }
    }

    gIco_Uxf_Api_Mng.Mng_MyProcess
        = ico_uxf_mng_process(gIco_Uxf_Api_Mng.MyProcess, 0);
    if (!gIco_Uxf_Api_Mng.Mng_MyProcess) {
        uifw_error("ico_uxf_init: Leave(ESRCH), Unknown Apprication(%s)",
                   gIco_Uxf_Api_Mng.MyProcess);
        return ICO_UXF_ESRCH;
    }
    gIco_Uxf_Api_Mng.Mng_MyProcess->attr.internalid = getpid();
    gIco_Uxf_Api_Mng.Mng_MyProcess->attr.status = ICO_UXF_PROCSTATUS_RUN;

    dsp = ico_uxf_mng_display(gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display, 0);

    uifw_trace("ico_uxf_init: App.%d MainDisplay.%d %08x",
               gIco_Uxf_Api_Mng.Mng_MyProcess->attr.process,
               gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display, dsp);

    for (ret = 0; ret < (5000/50); ret++)  {
        gIco_Uxf_Api_Mng.Wayland_Display = wl_display_connect(NULL);
        if (gIco_Uxf_Api_Mng.Wayland_Display)  break;
        usleep(50*1000);
    }

    gIco_Uxf_Api_Mng.Wayland_Registry
        = wl_display_get_registry(gIco_Uxf_Api_Mng.Wayland_Display);
    wl_registry_add_listener(gIco_Uxf_Api_Mng.Wayland_Registry,
                             &ico_uxf_registry_listener, (void *)0);

    for (ret = 0; ret < (500/20); ret++) {
        if ((gIco_Uxf_Api_Mng.Wayland_WindowMgr != NULL) &&
            (gIco_Uxf_Api_Mng.Wayland_exInput != NULL) &&
            (gIco_Uxf_Api_Mng.Wayland_InputMgr != NULL))    break;
        uifw_trace("ico_uxf_init: call wl_display_dispatch(%08x)",
                   (int)gIco_Uxf_Api_Mng.Wayland_Display);
        wl_display_dispatch(gIco_Uxf_Api_Mng.Wayland_Display);
        usleep(20*1000);
    }
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
    uifw_trace("ico_uxf_init: Wayland/Weston connect OK");

    /* set client attribute if need     */
    if (gIco_Uxf_Api_Mng.Wayland_WindowMgr) {
        for(dn = 0; dn < appconf->applicationNum; dn++)  {
            if (appconf->application[dn].noconfigure)   {
                uifw_trace("ico_uxf_init: %s no need configure event",
                           appconf->application[dn].appid);
                ico_window_mgr_set_client_attr(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                               appconf->application[dn].appid,
                                               ICO_WINDOW_MGR_CLIENT_ATTR_NOCONFIGURE, 1);
                wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
            }
        }
    }

    gIco_Uxf_Api_Mng.WaylandFd
        = wl_display_get_fd(gIco_Uxf_Api_Mng.Wayland_Display);

    /* initialize mutex */
    (void) pthread_mutexattr_init(&sMutexAttr);
    if (pthread_mutex_init(&sMutex, &sMutexAttr) != 0) {
        uifw_error("ico_uxf_init: Leave(ENOSYS), Mutex Create Error %d", errno);
        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
        gIco_Uxf_Api_Mng.Wayland_Display = NULL;
        gIco_Uxf_Api_Mng.Initialized = 0;
        return ICO_UXF_ENOSYS;
    }

    /* allocate free control blocks         */
    freecb = ico_uxf_alloc_callback();
    ico_uxf_free_callback(freecb);
    freeeq = ico_uxf_alloc_eventque();
    ico_uxf_free_eventque(freeeq);

    /* flush wayland connection             */
    ico_window_mgr_set_eventcb(gIco_Uxf_Api_Mng.Wayland_WindowMgr, 1);
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    /* application launch/dead callback from AppCore(aul)   */
    aul_listen_app_launch_signal(ico_uxf_aul_aulcb, (void *)0);
    aul_listen_app_dead_signal(ico_uxf_aul_aulcb, (void *)1);

    gIco_Uxf_Api_Mng.Initialized = 1;

    uifw_trace("ico_uxf_init: Leave(EOK)");
    uifw_logflush();

    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_wl_display_fd: get wayland file discriptor
 *
 * @param       none
 * @return      wayland connect socket file descriptor
 * @retval      >=0         success(file descriptor)
 * @retval      < 0         error(wayland not connect)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_wl_display_fd(void)
{
    uifw_trace("ico_uxf_wl_display_fd: fd=%d", gIco_Uxf_Api_Mng.WaylandFd);

    return gIco_Uxf_Api_Mng.WaylandFd;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_shutdown: terminate user interface library hor HomeScreen
 *
 * @param       none
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_EBUSY   error(cullentry terminating)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_shutdown(void)
{
    uifw_trace("ico_uxf_shutdown: Enter");

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        if (gIco_Uxf_Api_Mng.Initialized < 0)  {
            uifw_warn("ico_uxf_shutdown: Leave(EBUSY)");
            return ICO_UXF_EBUSY;
        }
        uifw_trace("ico_uxf_shutdown: Leave(EOK)");
        return ICO_UXF_EOK;
    }
    gIco_Uxf_Api_Mng.Initialized = -1;
    gIco_Uxf_Api_Mng.LastEvent = 0;

    (void) pthread_mutex_destroy(&sMutex);

    if (gIco_Uxf_Api_Mng.Wayland_Seat) {
        wl_seat_destroy(gIco_Uxf_Api_Mng.Wayland_Seat);
        gIco_Uxf_Api_Mng.Wayland_Seat = NULL;
    }
    if (gIco_Uxf_Api_Mng.Wayland_exInput) {
        ico_exinput_destroy(gIco_Uxf_Api_Mng.Wayland_exInput);
        gIco_Uxf_Api_Mng.Wayland_exInput = NULL;
    }
    if (gIco_Uxf_Api_Mng.Wayland_InputMgr) {
        ico_input_mgr_control_destroy(gIco_Uxf_Api_Mng.Wayland_InputMgr);
        gIco_Uxf_Api_Mng.Wayland_InputMgr = NULL;
    }
    if (gIco_Uxf_Api_Mng.Wayland_Compositor)   {
        wl_compositor_destroy(gIco_Uxf_Api_Mng.Wayland_Compositor);
        gIco_Uxf_Api_Mng.Wayland_Compositor = NULL;
    }
    if (gIco_Uxf_Api_Mng.Wayland_Display)  {
        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
        wl_display_disconnect(gIco_Uxf_Api_Mng.Wayland_Display);
        gIco_Uxf_Api_Mng.Wayland_Display = NULL;
    }

    gIco_Uxf_Api_Mng.Initialized = 0;

    uifw_trace("ico_uxf_shutdown: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_main_loop_iterate: process user interface library events
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_main_loop_iterate(void)
{
    int                     arg;
    Ico_Uxf_Mng_EventQue    *wkque;
    Ico_Uxf_Mng_Callback    *callback;
    Ico_Uxf_Event_Cb        func;

    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);

    arg = 0;
    if (ioctl(gIco_Uxf_Api_Mng.WaylandFd, FIONREAD, &arg) < 0)   {
        uifw_warn("ico_uxf_main_loop_iterate: ioclt(FIONREAD,) Error %d", errno);
        arg = 0;
    }

    if (arg > 0)   {
        wl_display_dispatch(gIco_Uxf_Api_Mng.Wayland_Display);
    }

    ico_uxf_enter_critical();

    /* notify event */
    while (gIco_Uxf_Api_Mng.EventQue) {

        wkque = gIco_Uxf_Api_Mng.EventQue;
        gIco_Uxf_Api_Mng.EventQue = gIco_Uxf_Api_Mng.EventQue->next;

        gIco_Uxf_Api_Mng.LastEvent = wkque->detail.event;
        callback = gIco_Uxf_Api_Mng.Callback;
        while (callback) {
            if (callback->eventmask & gIco_Uxf_Api_Mng.LastEvent) {
                func = callback->func;
                arg = callback->arg;
                ico_uxf_leave_critical();

                uifw_trace("ico_uxf_main_loop: Call Event Callback(%08x,,%08x)",
                           gIco_Uxf_Api_Mng.LastEvent, arg);
                (*func)(gIco_Uxf_Api_Mng.LastEvent, wkque->detail, arg);

                ico_uxf_enter_critical();
            }
            callback = callback->next;
        }
        ico_uxf_free_eventque(wkque);
    }

    ico_uxf_leave_critical();

    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_callback_set: set event calback function
 *
 * @param[in]   mask            event mask
 * @param[in]   func            callback function
 * @param[in]   arg             user argument
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(user interface library not initialized)
 * @retval      ICO_UXF_EINVAL  error(illegal event mask)
 * @retval      ICO_UXF_EBUSY   error(already set same event)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_callback_set(const unsigned int mask, Ico_Uxf_Event_Cb func, const int arg)
{
    Ico_Uxf_Mng_Callback    *callback;
    unsigned int            wkmask;

    uifw_trace("ico_uxf_callback_set: Enter(%08x,%08x,%08x)", mask, (int)func, arg);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_callback_set: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (mask)  {
        wkmask = mask;
    }
    else    {
        wkmask = ICO_UXF_EVENT_ALL;
    }
    if ((wkmask & ICO_UXF_EVENT_VALIDALL) == 0) {
        uifw_warn("ico_uxf_callback_set: Leave(EINVAL)");
        return ICO_UXF_EINVAL;
    }

    ico_uxf_enter_critical();

    callback = gIco_Uxf_Api_Mng.Callback;
    while (callback)   {
        if ((callback->eventmask == wkmask) && (callback->func == func) &&
            (callback->arg == arg))    {

            ico_uxf_leave_critical();
            uifw_warn("ico_uxf_callback_set: Leave(EBUSY)");
            return ICO_UXF_EBUSY;
        }
    }

    callback = ico_uxf_alloc_callback();

    callback->eventmask = wkmask;
    callback->func = func;
    callback->arg = arg;

    ico_uxf_regist_callback(callback);

    gIco_Uxf_Api_Mng.EventMask |= wkmask;

    ico_uxf_leave_critical();

    uifw_trace("ico_uxf_callback_set: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_callback_remove: remove event calback function
 *
 * @param[in]   mask            event mask
 * @param[in]   func            callback function
 * @param[in]   arg             user argument
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ESRCH   error(user interface library not initialized)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_callback_remove(const unsigned int mask, Ico_Uxf_Event_Cb func, const int arg)
{
    Ico_Uxf_Mng_Callback    *callback;
    unsigned int            wkmask;

    uifw_trace("ico_uxf_callback_remove: Enter(%08x,%08x,%08x)", mask, (int)func, arg);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_warn("ico_uxf_callback_remove: Leave(ESRCH)");
        return ICO_UXF_ESRCH;
    }

    if (mask)  {
        wkmask = mask;
    }
    else    {
        wkmask = ICO_UXF_EVENT_ALL;
    }

    ico_uxf_enter_critical();

    gIco_Uxf_Api_Mng.EventMask = 0;

    callback = gIco_Uxf_Api_Mng.Callback;

    while (callback)   {
        if ((callback->eventmask == wkmask) && (callback->func == func) &&
            (callback->arg == arg))    {
            ico_uxf_remove_callback(callback);
        }
        else    {
            gIco_Uxf_Api_Mng.EventMask |= callback->eventmask;
        }
        callback = callback->next;
    }

    ico_uxf_leave_critical();

    uifw_trace("ico_uxf_callback_remove: Leave(EOK)");
    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_free_procwin: free active window management table(internal function)
 *
 * @param[in]   prc             application management table address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_free_procwin(Ico_Uxf_Mng_Process *prc)
{
    Ico_Uxf_Mng_Window  *p;
    Ico_Uxf_Mng_Window  *pp;
    Ico_Uxf_Mng_Window  *freep;
    int                 hash;

    uifw_trace("ico_uxf_free_procwin: Enter(%d)", prc->attr.process);

    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)   {
        p = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
        pp = (Ico_Uxf_Mng_Window *)0;
        while (p)  {
            if (p->attr.process == prc->attr.process)  {
                freep = p;
                p = p->nextidhash;
                if (! pp)  {
                    gIco_Uxf_Api_Mng.Hash_WindowId[hash] = p;
                }
                else    {
                    pp->nextidhash = p;
                }
                uifw_trace("ico_uxf_free_procwin: Free Window(%08x)", freep->attr.window);
                free(freep);
            }
            else    {
                pp = p;
                p = p->nextidhash;
            }
        }
    }
    uifw_trace("ico_uxf_free_procwin: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_wayland_globalcb: wayland global callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   registry        wayland registry
 * @param[in]   name            wayland display Id(unused)
 * @param[in]   interface       wayland interface name
 * @param[in]   version         wayland interface version number(unused)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_wayland_globalcb(void *data, struct wl_registry *registry,
                         uint32_t name, const char *interface, uint32_t version)
{
    uifw_trace("ico_uxf_wayland_globalcb: Enter(Event=%s DispId=%08x)", interface, name);

    if (strcmp(interface, "ico_window_mgr") == 0)    {
        gIco_Uxf_Api_Mng.Wayland_WindowMgr = (struct ico_window_mgr *)
                wl_registry_bind(registry, name, &ico_window_mgr_interface, 1);
        ico_window_mgr_add_listener(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                    &windowlistener, NULL);
        ico_window_mgr_set_user(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                getpid(), gIco_Uxf_Api_Mng.MyProcess);
    }
    else if (strcmp(interface, "ico_exinput") == 0) {
        /* regist exinput */
        gIco_Uxf_Api_Mng.Wayland_exInput = (struct ico_exinput *)
                wl_registry_bind(registry, name, &ico_exinput_interface, 1);
        ico_exinput_add_listener(gIco_Uxf_Api_Mng.Wayland_exInput, &exinputlistener, NULL);
    }
    else if (strcmp(interface, "ico_input_mgr_control") == 0) {
        /* regist Multi Input Manager interface */
        gIco_Uxf_Api_Mng.Wayland_InputMgr = (struct ico_input_mgr_control *)
                wl_registry_bind(registry, name, &ico_input_mgr_control_interface, 1);
    }
    else if (strcmp(interface, "wl_output") == 0) {
        /* regist wl_output listener to get display info */
        gIco_Uxf_Api_Mng.Wayland_Output = (struct wl_output *)
                wl_registry_bind(registry, name, &wl_output_interface, 1);
        wl_output_add_listener(gIco_Uxf_Api_Mng.Wayland_Output, &outputlistener, NULL);
    }
    wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
    uifw_trace("ico_uxf_wayland_globalcb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_output_geometrycb: wayland display attribute callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   wl_output       wayland wl_output interface
 * @param[in]   x               display upper-left X coodinate
 * @param[in]   y               display upper-left Y coodinate
 * @param[in]   physical_width  display physical width
 * @param[in]   physical_height display physical height
 * @param[in]   subpixel        display sub pixcel
 * @param[in]   make            display maker
 * @param[in]   model           diaplay model
 * @param[in]   transform       transform
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_output_geometrycb(void *data, struct wl_output *wl_output, int32_t x, int32_t y,
                          int32_t physical_width, int32_t physical_height, int32_t subpixel,
                          const char *make, const char *model, int32_t transform)
{
    Ico_Uxf_Mng_Display *dsp;

    uifw_trace("ico_uxf_output_geometrycb: Enter(x/y=%d/%d, pwidth/height=%d/%d, trans=%d)",
               x, y, physical_width, physical_height, transform);

    dsp = ico_uxf_mng_display(gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display, 0);

    if (dsp) {
        if ((transform == WL_OUTPUT_TRANSFORM_90)
            || (transform == WL_OUTPUT_TRANSFORM_270)
            || (transform == WL_OUTPUT_TRANSFORM_FLIPPED_90)
            || (transform == WL_OUTPUT_TRANSFORM_FLIPPED_270))  {
            dsp->attr.pWidth = physical_height;
            dsp->attr.pHeight = physical_width;
            dsp->attr.orientation = ICO_UXF_ORIENTATION_VERTICAL;
        }
        else {
            dsp->attr.pWidth = physical_width;
            dsp->attr.pHeight = physical_height;
            dsp->attr.orientation = ICO_UXF_ORIENTATION_HORIZONTAL;
        }
    }
    uifw_trace("ico_uxf_output_geometrycb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_output_modecb: wayland display mode callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   wl_output       wayland wl_output interface
 * @param[in]   flags           flags
 * @param[in]   width           display width
 * @param[in]   height          display height
 * @param[in]   refresh         display refresh rate
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_output_modecb(void *data, struct wl_output *wl_output, uint32_t flags,
                      int32_t width, int32_t height, int32_t refresh)
{
    Ico_Uxf_Mng_Display *dsp;

    uifw_trace("ico_uxf_output_modecb: Enter(flg=%d, width=%d, height=%d, ref=%d)",
               flags, width, height, refresh);

    dsp = ico_uxf_mng_display(gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display, 0);

    if (dsp && (flags & WL_OUTPUT_MODE_CURRENT)) {
        if (dsp->attr.orientation == ICO_UXF_ORIENTATION_VERTICAL) {
            dsp->attr.pWidth = height;
            dsp->attr.pHeight = width;
        }
        else {
            dsp->attr.pWidth = width;
            dsp->attr.pHeight = height;
        }
    }
    uifw_trace("ico_uxf_output_modecb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_createdcb: wayland surface create callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param[in]   surfaceid       ico_window_mgr surface Id
 * @param[in]   pid             wayland client process Id
 * @param[in]   appid           wayland client application Id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_window_createdcb(void *data, struct ico_window_mgr *ico_window_mgr,
                         uint32_t surfaceid, int32_t pid, const char *appid)
{
    Ico_Uxf_Mng_Process *prc;
    Ico_Uxf_Mng_ProcWin *ppwin;
    Ico_Uxf_Mng_ProcWin *ppw;

    uifw_trace("ico_uxf_window_createdcb: Enter(surf=%08x pid=%d appid=%s myapp=%s)",
               (int)surfaceid, pid, appid, gIco_Uxf_Api_Mng.MyProcess);

    prc = ico_uxf_mng_process(appid, 0);
    if (prc)    {
        /* set window animation     */
        if (prc->appconf != NULL)   {
            if (((Ico_Uxf_conf_application *)prc->appconf)->animation)  {
                ico_window_mgr_set_animation(
                    gIco_Uxf_Api_Mng.Wayland_WindowMgr, surfaceid,
                    ((Ico_Uxf_conf_application *)prc->appconf)->animation,
                    ((Ico_Uxf_conf_application *)prc->appconf)->animation_time);
            }
            else if (((Ico_Uxf_conf_application *)prc->appconf)->animation_time > 0)    {
                ico_window_mgr_set_animation(
                    gIco_Uxf_Api_Mng.Wayland_WindowMgr, surfaceid, " ",
                    ((Ico_Uxf_conf_application *)prc->appconf)->animation_time);
            }
        }
        if (prc->attr.mainwin.window <= 0)  {
            uifw_trace("ico_uxf_window_createdcb: Set Main Window, Config Data");
            prc->attr.mainwin.window = surfaceid;
            ico_uxf_window_configurecb(data, ico_window_mgr, surfaceid, appid,
                                       prc->attr.mainwin.layer, prc->attr.mainwin.x,
                                       prc->attr.mainwin.y, prc->attr.mainwin.w,
                                       prc->attr.mainwin.h, 0);
            if (gIco_Uxf_Api_Mng.Hook_Window)   {
                (*gIco_Uxf_Api_Mng.Hook_Window)(prc->attr.process, surfaceid,
                                                ICO_UXF_HOOK_WINDOW_CREATE_MAIN);
            }
        }
        else    {
            uifw_trace("ico_uxf_window_createdcb: Sub Window, Dummy Data");
            ico_uxf_window_configurecb(data, ico_window_mgr, surfaceid, appid,
                                       prc->attr.mainwin.layer, 16384, 16384, 1, 1, 0);
            ppwin = (Ico_Uxf_Mng_ProcWin *)malloc(sizeof(Ico_Uxf_Mng_ProcWin));
            if (ppwin) {
                memset(ppwin, 0, sizeof(Ico_Uxf_Mng_ProcWin));
                ppwin->attr.window = surfaceid;
                ppwin->attr.display = prc->attr.mainwin.display;
                if (!prc->procwin) {
                    prc->procwin = ppwin;
                }
                else {
                    ppw = prc->procwin;
                    while (ppw->next) {
                        ppw = ppw->next;
                    }
                    ppw->next = ppwin;
                }
            }
            if (gIco_Uxf_Api_Mng.Hook_Window)   {
                (*gIco_Uxf_Api_Mng.Hook_Window)(prc->attr.process, surfaceid,
                                                ICO_UXF_HOOK_WINDOW_CREATE_SUB);
            }
        }
    }
    else    {
        uifw_warn("ico_uxf_window_createdcb: Application.%s dose not exist", appid);
    }
    uifw_trace("ico_uxf_window_createdcb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_destroyedcb: wayland surface destroy callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param[in]   surfaceid       ico_window_mgr surface Id
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_window_destroyedcb(void *data, struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid)
{
    Ico_Uxf_Mng_Window  *p;
    Ico_Uxf_Mng_Window  *pp;
    Ico_Uxf_Mng_Window  *freep;
    Ico_Uxf_Mng_Process *prc;
    Ico_Uxf_Mng_EventQue *que;
    int                 hash;
    char                wkappid[ICO_UXF_MAX_PROCESS_NAME+1];
    int                 wksubwindow;

    uifw_trace("ico_uxf_window_destroyedcb: Enter(surf=%08x)", (int)surfaceid);

    ico_uxf_enter_critical();

    p = ico_uxf_mng_window(surfaceid, 0);

    if (! p)    {
        ico_uxf_leave_critical();
        uifw_trace("ico_uxf_window_destroyedcb: Leave(Surface=%08x dose not exist)",
                   (int)surfaceid);
        return;
    }
    prc = p->mng_process;
    if ((prc != NULL) && (prc->attr.mainwin.window == (int)surfaceid))  {
        prc->attr.mainwin.window = 0;
    }

    strcpy(wkappid, p->attr.process);
    wksubwindow = p->attr.subwindow;

    if ((gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_DESTORYWINDOW) &&
        (p->attr.eventmask & ICO_UXF_EVENT_DESTORYWINDOW)) {

        que = ico_uxf_alloc_eventque();
        que->detail.event = ICO_UXF_EVENT_DESTORYWINDOW;
        que->detail.window.display = p->attr.display;
        que->detail.window.window = surfaceid;
        que->detail.window.layer = p->attr.layer;
        que->detail.window.x = p->attr.x;
        que->detail.window.y = p->attr.y;
        que->detail.window.w = p->attr.w;
        que->detail.window.h = p->attr.h;
        que->detail.window.visible = p->attr.visible;
        que->detail.window.raise = p->attr.raise;
        que->detail.window.active = p->attr.active;
        ico_uxf_regist_eventque(que);
    }

    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)   {
        p = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
        pp = (Ico_Uxf_Mng_Window *)0;
        while (p)  {
            if (p->attr.window == (int)surfaceid)  {
                freep = p;
                p = p->nextidhash;
                if (! pp)  {
                    gIco_Uxf_Api_Mng.Hash_WindowId[hash] = p;
                }
                else    {
                    pp->nextidhash = p;
                }
                uifw_trace("ico_uxf_window_destroyedcb: Free Window.%08x(%08x)",
                           (int)freep, freep->attr.window);
                free(freep);
                hash = ICO_UXF_MISC_HASHSIZE;
                break;
            }
            pp = p;
            p = p->nextidhash;
        }
    }
    ico_uxf_leave_critical();

    if (gIco_Uxf_Api_Mng.Hook_Window)   {
        (*gIco_Uxf_Api_Mng.Hook_Window)(wkappid, surfaceid,
                                        wksubwindow ? ICO_UXF_HOOK_WINDOW_DESTORY_SUB :
                                                      ICO_UXF_HOOK_WINDOW_DESTORY_MAIN);
    }
    uifw_trace("ico_uxf_window_destroyedcb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_visiblecb: wayland surface visible callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param[in]   surfaceid       ico_window_mgr surface Id
 * @param[in]   visibility      surface visible(1=visible/0=unvisible/9=nochange)
 * @param[in]   raise           surface raise(1=raise/0=lower/9=nochange)
 * @param[in]   hint            client request(1=client request(not changed)/0=changed)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_window_visiblecb(void *data, struct ico_window_mgr *ico_window_mgr,
                         uint32_t surfaceid, int32_t visible, int32_t raise, int32_t hint)
{
    Ico_Uxf_Mng_Window  *win;
    Ico_Uxf_Mng_EventQue *que;
    int                 ovisible;
    int                 oraise;

    ico_uxf_enter_critical();

    win = ico_uxf_mng_window(surfaceid, 0);

    if (!win)  {
        ico_uxf_leave_critical();
        uifw_trace("ico_uxf_window_visiblecb: Surface=%08x dose not exist",
                   (int)surfaceid);
        return;
    }

    ovisible = visible;
    oraise = raise;
    if (ovisible == 9) {
        ovisible = win->attr.visible;
    }
    if (oraise == 9)   {
        oraise = win->attr.raise;
    }

    if ((win->attr.visible != ovisible) ||
        (win->attr.raise != oraise))   {

        if (((hint == 0) && (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_VISIBLE) &&
             (win->attr.eventmask & ICO_UXF_EVENT_VISIBLE)) ||
            ((hint != 0) && (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_VISIBLE_REQ) &&
             (win->attr.eventmask & ICO_UXF_EVENT_VISIBLE_REQ)))   {

            que = ico_uxf_alloc_eventque();
            que->detail.event =
                    (hint == 0) ? ICO_UXF_EVENT_VISIBLE : ICO_UXF_EVENT_VISIBLE_REQ;
            que->detail.window.display = win->attr.display;
            que->detail.window.window = win->attr.window;
            que->detail.window.layer = win->attr.layer;
            que->detail.window.x = win->attr.x;
            que->detail.window.y = win->attr.y;
            que->detail.window.w = win->attr.w;
            que->detail.window.h = win->attr.h;
            que->detail.window.visible = ovisible;
            que->detail.window.raise = oraise;
            que->detail.window.active = win->attr.active;

            ico_uxf_regist_eventque(que);
        }
    }
    ico_uxf_leave_critical();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_configurecb: wayland surface configure callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param[in]   surfaceid       ico_window_mgr surface Id
 * @param[in]   appid           client application Id
 * @param[in]   x               surface upper-left X coodinate
 * @param[in]   y               surface upper-left Y coodinate
 * @param[in]   width           surface width
 * @param[in]   height          surface height
 * @param[in]   hint            client request(1=client request(not changed)/0=changed)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_window_configurecb(void *data, struct ico_window_mgr *ico_window_mgr,
                           uint32_t surfaceid, const char *appid, int32_t layer,
                           int32_t x, int32_t y, int32_t width, int32_t height,
                           int32_t hint)
{
    Ico_Uxf_Mng_Window  *win;
    Ico_Uxf_Mng_EventQue *que;
    int                 display;
    Ico_Uxf_Mng_Process *prc;

#if 0               /* too many logout, change to comment out   */
    uifw_trace("ico_uxf_window_configurecb: surf=%08x app=%s layer=%d "
               "x/y=%d/%d w/h=%d/%d hint=%d",
               (int)surfaceid, appid, layer, x, y, width, height, hint);
#endif              /* too many logout, change to comment out   */

    ico_uxf_enter_critical();

    win = ico_uxf_mng_window(surfaceid, 0);

    if (!win)  {
        win = ico_uxf_mng_window(surfaceid, 1);
        display = ICO_UXF_GETDISPLAYID(surfaceid);
        win->mng_display = ico_uxf_mng_display(display, 0);
        win->mng_layer = ico_uxf_mng_layer(display, layer, 0);
        win->attr.eventmask = ICO_UXF_EVENT_NEWWINDOW;
        win->attr.display = display;
        win->attr.layer = layer;
        memset(win->attr.process, 0, ICO_UXF_MAX_PROCESS_NAME + 1);
        strncpy(win->attr.process, appid, ICO_UXF_MAX_PROCESS_NAME);
        win->attr.x = x;
        win->attr.y = y;
        win->attr.w = width;
        win->attr.h = height;
        hint = 0;

        prc = ico_uxf_mng_process(appid, 0);
        win->mng_process = prc;
        if ((prc != NULL) && (prc->attr.mainwin.window != (int)surfaceid)) {
            /* sub-window */
            win->attr.subwindow = 1;
        }

        if (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_NEWWINDOW)    {

            que = ico_uxf_alloc_eventque();

            que->detail.event = ICO_UXF_EVENT_NEWWINDOW;
            que->detail.window.display = display;
            que->detail.window.window = surfaceid;
            que->detail.window.layer = layer;
            que->detail.window.x = win->attr.x;
            que->detail.window.y = win->attr.y;
            que->detail.window.w = win->attr.w;
            que->detail.window.h = win->attr.h;
            que->detail.window.visible = win->attr.visible;
            que->detail.window.raise = win->attr.raise;
            que->detail.window.active = win->attr.active;

            ico_uxf_regist_eventque(que);
        }
    }
    else    {
        if ((win->attr.layer != layer) ||
            (win->attr.x != x) || (win->attr.y != y) ||
            (win->attr.w != width) || (win->attr.h != height)) {

            if (((hint == 0) && (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_RESIZE) &&
                 (win->attr.eventmask & ICO_UXF_EVENT_RESIZE)) ||
                ((hint != 0) && (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_RESIZE_REQ) &&
                 (win->attr.eventmask & ICO_UXF_EVENT_RESIZE_REQ)))    {

                que = ico_uxf_alloc_eventque();

                que->detail.event =
                        (hint == 0) ? ICO_UXF_EVENT_RESIZE : ICO_UXF_EVENT_RESIZE_REQ;
                que->detail.window.display = win->mng_display->attr.display;
                que->detail.window.window = win->attr.window;
                que->detail.window.layer = layer;
                que->detail.window.x = x;
                que->detail.window.y = y;
                que->detail.window.w = width;
                que->detail.window.h = height;
                que->detail.window.visible = win->attr.visible;
                que->detail.window.raise = win->attr.raise;
                que->detail.window.active = win->attr.active;

                ico_uxf_regist_eventque(que);
            }
            if (hint == 0)  {
                win->attr.x = x;
                win->attr.y = y;
                win->attr.w = width;
                win->attr.h = height;
                if (win->attr.layer != layer)  {
                    win->attr.layer = layer;
                    win->mng_layer = ico_uxf_mng_layer(win->mng_display->attr.display,
                                                       layer, 0);
                }
            }
        }
    }
    ico_uxf_leave_critical();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_activecb: wayland surface active callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_window_mgr  wayland ico_window_mgr plugin interface
 * @param[in]   surfaceid       ico_window_mgr surface Id
 * @param[in]   active          surface active(1=active/0=not active)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_window_activecb(void *data, struct ico_window_mgr *ico_window_mgr,
                        uint32_t surfaceid, uint32_t active)
{
    Ico_Uxf_Mng_Window  *win;
    Ico_Uxf_Mng_EventQue *que;

    uifw_trace("ico_uxf_window_activecb: Enter(surf=%08x active=%d)", surfaceid, active);

    ico_uxf_enter_critical();

    win = ico_uxf_mng_window(surfaceid, 0);

    if (win)  {
        if (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_ACTIVEWINDOW)    {

            que = ico_uxf_alloc_eventque();

            que->detail.event = ICO_UXF_EVENT_ACTIVEWINDOW;
            que->detail.window.display = ICO_UXF_GETDISPLAYID(surfaceid);
            que->detail.window.window = surfaceid;
            que->detail.window.active = active;
            win->attr.active = active;

            ico_uxf_regist_eventque(que);
        }
    }
    ico_uxf_leave_critical();
    uifw_trace("ico_uxf_window_activecb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_input_capabilitiescb: wayland extended input
 *                                        input capabilities callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_exinput     wayland ico_input_mgr plugin interface
 * @param[in]   device          input device name
 * @param[in]   type            input device type
 * @param[in]   swname          input switch name
 * @param[in]   input           input switch number
 * @param[in]   codename        input code name
 * @param[in]   code            input code value
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_input_capabilitiescb(void *data, struct ico_exinput *ico_exinput,
                             const char *device, int32_t type, const char *swname,
                             int32_t input, const char *codename, int32_t code)
{
    int     i, j;
    Ico_Uxf_Sys_Config  *sysconf = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    Ico_Uxf_InputDev    *inputdev;
    Ico_Uxf_InputDev    *bpdev;
    Ico_Uxf_InputSw     *inputsw;
    Ico_Uxf_InputSw     *bpsw;

    uifw_trace("ico_uxf_input_capabilitiescb: "
               "Enter(device=%s type=%d sw=%s input=%d code=%s[%d])",
               device, type, swname, input, codename, code);

    if (! sysconf)  return;

    /* search and create input device table     */
    bpdev = NULL;
    inputdev = gIco_Uxf_Api_Mng.InputDev;
    while (inputdev)    {
        if (strcasecmp(inputdev->device, device) == 0)  break;
        bpdev = inputdev;
        inputdev = inputdev->next;
    }
    if (! inputdev) {
        inputdev = malloc(sizeof(Ico_Uxf_InputDev));
        if (! inputdev) {
            uifw_error("ico_uxf_input_capabilitiescb: Leave(No Memory)");
            return;
        }
        memset(inputdev, 0, sizeof(Ico_Uxf_InputDev));

        if (bpdev)  {
            bpdev->next = inputdev;
        }
        else    {
            gIco_Uxf_Api_Mng.InputDev = inputdev;
        }
    }
    strncpy(inputdev->device, device, sizeof(inputdev->device)-1);
    inputdev->type = type;

    bpsw = NULL;
    inputsw = inputdev->inputSw;
    while (inputsw) {
        if (strcasecmp(inputsw->swname, swname) == 0)   break;
        bpsw = inputsw;
        inputsw = inputsw->next;
    }
    if (! inputsw)  {
        inputsw = malloc(sizeof(Ico_Uxf_InputSw));
        if (! inputsw)  {
            uifw_error("ico_uxf_input_capabilitiescb: Leave(No Memory)");
            return;
        }
        if (bpsw)   {
            bpsw->next = inputsw;
        }
        else    {
            inputdev->inputSw = inputsw;
        }
        inputdev->numInputSw ++;
    }
    memset(inputsw, 0, sizeof(Ico_Uxf_InputSw));
    strncpy(inputsw->swname, swname, sizeof(inputsw->swname)-1);
    inputsw->input = input;
    inputsw->numCode = 1;
    inputsw->inputCode[0].code = code;
    strncpy(inputsw->inputCode[0].codename, codename,
            sizeof(inputsw->inputCode[0].codename)-1);
    uifw_trace("ico_uxf_input_capabilitiescb: create new inputsw");

    /* send fixed application if fixed switch   */
    for (i = 0; i < sysconf->inputdevNum; i++)  {
        if (strcasecmp(sysconf->inputdev[i].name, inputdev->device) != 0)   continue;

        for (j = 0; j < sysconf->inputdev[i].inputswNum; j++)   {
            if (! sysconf->inputdev[i].inputsw[j].appid)    continue;
            if (strcasecmp(sysconf->inputdev[i].inputsw[j].name,
                inputsw->swname) != 0)  continue;

            uifw_trace("ico_uxf_input_capabilitiescb: input %s.%s = %s",
                       inputdev->device, inputsw->swname,
                       sysconf->inputdev[i].inputsw[j].appid);
            ico_input_mgr_control_add_input_app(gIco_Uxf_Api_Mng.Wayland_InputMgr,
                                                sysconf->inputdev[i].inputsw[j].appid,
                                                inputdev->device, inputsw->input, 1);
            inputsw->fix = 1;
            break;
        }
    }
    uifw_trace("ico_uxf_input_capabilitiescb: Leave");
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_input_codecb: wayland extended input
 *                                input code callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_exinput     wayland ico_input_mgr plugin interface
 * @param[in]   device          input device name
 * @param[in]   input           input switch number
 * @param[in]   codename        input code name
 * @param[in]   code            input code value
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_input_codecb(void *data, struct ico_exinput *ico_exinput, const char *device,
                     int32_t input, const char *codename, int32_t code)
{
    int     i;
    Ico_Uxf_InputDev    *inputdev;
    Ico_Uxf_InputSw     *inputsw;

    uifw_trace("ico_uxf_input_codecb: device=%s input=%d code=%s[%d]",
               device, input, codename, code);

    /* search input device table                */
    inputdev = gIco_Uxf_Api_Mng.InputDev;
    while (inputdev)    {
        if (strcasecmp(inputdev->device, device) == 0)  break;
        inputdev = inputdev->next;
    }
    if (! inputdev) {
        uifw_error("ico_uxf_input_codecb: device.%s dose not exist", device);
        return;
    }

    /* search input switch                      */
    inputsw = inputdev->inputSw;
    while (inputsw) {
        if (inputsw->input == input)    break;
        inputsw = inputsw->next;
    }
    if (! inputsw)  {
        uifw_error("ico_uxf_input_codecb: device.%s input=%d dose not exist",
                   device, input);
        return;
    }
    for (i = 0; i < inputsw->numCode; i++)  {
        if (inputsw->inputCode[i].code == code) break;
    }
    if (i >= inputsw->numCode)  {
        if (i >= 8) {
            uifw_error("ico_uxf_input_codecb: device.%s input=%d number of codes overflow",
                       device, input);
            return;
        }
        inputsw->numCode ++;
        i = inputsw->numCode - 1;
    }
    memset(inputsw->inputCode[i].codename, 0, sizeof(inputsw->inputCode[i].codename));
    strncpy(inputsw->inputCode[i].codename, codename,
            sizeof(inputsw->inputCode[i].codename)-1);
    inputsw->inputCode[i].code = code;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_input_inputcb: wayland extended input
 *                                 input switch callback(static function)
 *
 * @param[in]   data            user data(unused)
 * @param[in]   ico_exinput     wayland ico_input_mgr plugin interface
 * @param[in]   time            input time(miri-sec)
 * @param[in]   device          input device name
 * @param[in]   input           input switch number
 * @param[in]   code            input code value
 * @param[in]   state           input state(1=switch On/0=switch Off)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_input_inputcb(void *data, struct ico_exinput *ico_exinput, uint32_t time,
                      const char *device, int32_t input, int32_t code, int32_t state)
{
    Ico_Uxf_Mng_EventQue *que;
    uifw_trace("ico_uxf_input_inputcb: device=%s input=%d code=%d state=%d",
               device, input, code, state);

    if (gIco_Uxf_Api_Mng.EventMask & ICO_UXF_EVENT_EXINPUT)    {

        ico_uxf_enter_critical();
        que = ico_uxf_alloc_eventque();

        strncpy(que->detail.exinput.device, device,
                sizeof(que->detail.exinput.device)-1);
        que->detail.exinput.input = input;
        que->detail.exinput.code = code;
        que->detail.exinput.state = state;

        ico_uxf_regist_eventque(que);
        ico_uxf_leave_critical();
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_aul_aulcb: AppCore(AUL) application launch/dead callback(static function)
 *
 * @param[in]   pid             target application process Id
 * @param[in]   data            user data(0=launch, 1=dead)
 * @return      result(always 0, means success)
 */
/*--------------------------------------------------------------------------*/
static int
ico_uxf_aul_aulcb(int pid, void *data)
{
    Ico_Uxf_Mng_Process     *proc;
    int                     hash;
    Ico_Uxf_Mng_EventQue    *que;
    int                     dead;
    char                    appid[ICO_UXF_MAX_PROCESS_NAME+1];

    dead = (int)data;
    memset(appid, 0, sizeof(appid));
    if (dead == 0)  {
        (void) aul_app_get_appid_bypid(pid, appid, sizeof(appid));
        uifw_trace("ico_uxf_aul_aulcb: Enter(pid=%d, dead=No, appid=%s)", pid, appid);
    }
    else    {
        uifw_trace("ico_uxf_aul_aulcb: Enter(pid=%d, dead=Yes)", pid);
    }
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++) {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)   {
            if (proc->attr.internalid == pid) break;
            if (strcmp(proc->attr.process, appid) == 0) break;
            proc = proc->nextidhash;
        }
        if (proc) break;
    }
    if (! proc) {
        /* not handle process,  */
        /* or the process is correctly terminated by ico_uxf_process_terminate */
        uifw_trace("ico_uxf_aul_aulcb: Leave(not find)");
        return 0;
    }
    uifw_trace("ico_uxf_aul_aulcb: find(appid=%s)", proc->attr.process);

    ico_uxf_enter_critical();
    /* set event */
    que = ico_uxf_alloc_eventque();
    strncpy(que->detail.process.process, proc->attr.process, ICO_UXF_MAX_PROCESS_NAME);
    if (dead)   {
        que->detail.event = ICO_UXF_EVENT_TERMPROCESS;
        que->detail.process.status = ICO_UXF_PROCSTATUS_STOP;

        /* delete process info */
        proc->attr.status = ICO_UXF_PROCSTATUS_STOP;
        proc->attr.internalid = -1;
        proc->attr.mainwin.window = 0;
        ico_uxf_free_procwin(proc);
    }
    else    {
        que->detail.event = ICO_UXF_EVENT_EXECPROCESS;
        que->detail.process.status = ICO_UXF_PROCSTATUS_RUN;

        /* setup process info */
        if (proc->attr.status != ICO_UXF_PROCSTATUS_RUN)    {
            proc->attr.internalid = pid;
            if (proc->attr.status != ICO_UXF_PROCSTATUS_INIT)   {
                /* child process, search parent process */
                uifw_trace("ico_uxf_aul_aulcb: fork&exec %s", proc->attr.process);
                proc->attr.child = 1;
                /* save parent application if exist     */
                if (gIco_Uxf_Api_Mng.Mng_LastProcess != proc)   {
                    proc->parent = gIco_Uxf_Api_Mng.Mng_LastProcess;
                }
                else    {
                    uifw_trace("ico_uxf_aul_aulcb: same process %s real %s", proc->attr.process,
                               proc->parent ? proc->parent->attr.process : "(None)");
                }
            }
            else    {
                proc->attr.child = 0;
            }
            proc->attr.status = ICO_UXF_PROCSTATUS_RUN;
        }
    }

    /* notify event */
    ico_uxf_regist_eventque(que);

    ico_uxf_leave_critical();

    uifw_trace("ico_uxf_aul_aulcb: Leave");
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_set_lastapp: save last application
 *
 * @param[in]   appid   applicationId (if NULL, no last application)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_set_lastapp(const char *appid)
{
    if (appid)  {
        gIco_Uxf_Api_Mng.Mng_LastProcess = ico_uxf_mng_process(appid, 0);
    }
    else    {
        gIco_Uxf_Api_Mng.Mng_LastProcess = NULL;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getchild_appid: get child applicationId
 *
 * @param[in]   appid   parent applicationId
 * @return      last child applicationId
 * @retval      !=NULL      success(applicationId)
 * @retval      ==NULL      no child application
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API char *
ico_uxf_getchild_appid(const char *appid)
{
    Ico_Uxf_Mng_Process *pproc;
    Ico_Uxf_Mng_Process *proc;
    int     hash;

    pproc = ico_uxf_mng_process(appid, 0);
    if (! pproc)    {
        /* unknown parent application, no child     */
        uifw_trace("ico_uxf_getchild_appid: Unknown parent(%s)", appid);
        return NULL;
    }

    /* search parent        */
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)    {
            if (proc->parent == pproc)  {
                uifw_trace("ico_uxf_getchild_appid: child(%s) parent(%s)",
                           proc->attr.process, appid);
                return proc->attr.process;
            }
            proc = proc->nextidhash;
        }
    }
    uifw_trace("ico_uxf_getchild_appid: parent(%s) has no child", appid);
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_set_lastapp: save last application
 *
 * @param[in]   appid   applicationId (if NULL, no last application)
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_timer_wake(const int msec)
{
    Ico_Uxf_Mng_Process *proc;
    int     hash;

    if (gIco_Uxf_Api_Mng.InitTimer > 0) {
        gIco_Uxf_Api_Mng.InitTimer -= msec;
        if (gIco_Uxf_Api_Mng.InitTimer > 0) {
            return;
        }
        gIco_Uxf_Api_Mng.InitTimer = 0;
    }

    if (gIco_Uxf_Api_Mng.NeedTimer == 0)    {
        return;
    }
    gIco_Uxf_Api_Mng.NeedTimer = 0;
    for (hash = 0; hash < ICO_UXF_MISC_HASHSIZE; hash++)    {
        proc = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
        while (proc)    {
            if (proc->susptimer > 0)    {
                if (msec >= proc->susptimer)    {
                    proc->susptimer = 0;
                    if (proc->attr.suspend) {
                        if (proc->susp == 0)    {
                            proc->susp = 1;
                            uifw_trace("ico_uxf_timer_wake: CPU suspend pid=%d",
                                       proc->attr.internalid);
                            kill(proc->attr.internalid, SIGSTOP);
                        }
                    }
                    else if (proc->susp == 0)    {
                        uifw_trace("ico_uxf_timer_wake: CPU resume pid=%d(show=%d)",
                                   proc->attr.internalid, proc->showmode);
                        ico_window_mgr_set_visible(gIco_Uxf_Api_Mng.Wayland_WindowMgr,
                                                   proc->attr.mainwin.window,
                                                   proc->showmode ==
                                                       ICO_WINDOW_MGR_VISIBLE_SHOW_ANIMATION ?
                                                     ICO_WINDOW_MGR_VISIBLE_SHOW_ANIMATION :
                                                     ICO_WINDOW_MGR_VISIBLE_SHOW,
                                                   ICO_WINDOW_MGR_RAISE_NOCHANGE);
                        wl_display_flush(gIco_Uxf_Api_Mng.Wayland_Display);
                    }
                }
                else    {
                    proc->susptimer -= msec;
                    gIco_Uxf_Api_Mng.NeedTimer ++;
                }
            }
            proc = proc->nextidhash;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_enter_critical: enter critical section(internal function)
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_enter_critical(void)
{
    if (pthread_mutex_lock(&sMutex)) {
        uifw_error("ico_uxf_enter_critical: Error<%d>", errno);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_leave_critical: leave critical section(internal function)
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_leave_critical(void)
{
    if (pthread_mutex_unlock(&sMutex))   {
        uifw_error("ico_uxf_leave_critical: Error<%d>", errno);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_alloc_callback: allocate callback management table(internal function)
 *
 * @param       none
 * @return      allocated callback management table address
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_Callback *
ico_uxf_alloc_callback(void)
{
    Ico_Uxf_Mng_Callback    *p;
    int                     idx;

    p = gIco_Uxf_Api_Mng.CallbackFree;
    if (! p)   {
        p = (Ico_Uxf_Mng_Callback *)malloc(sizeof(Ico_Uxf_Mng_Callback) *
                                           ICO_UXF_MNG_BLOCK_ALLOCS);
        if (! p)   {
            uifw_error("ico_uxf_alloc_callback: NO Memory (malloc Error)");
            exit(1);
        }
        gIco_Uxf_Api_Mng.CallbackFree = p;

        for (idx = 0; idx < ICO_UXF_MNG_BLOCK_ALLOCS; idx++, p++)  {
            if (idx != (ICO_UXF_MNG_BLOCK_ALLOCS-1))   {
                p->next = (p+1);
            }
            else    {
                p->next = (struct _Ico_Uxf_Mng_Callback *)0;
            }
        }
        p = gIco_Uxf_Api_Mng.CallbackFree;
    }
    gIco_Uxf_Api_Mng.CallbackFree = p->next;
    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_free_callback: release callback management table(internal function)
 *
 * @param[in]   p           callback management table address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_free_callback(Ico_Uxf_Mng_Callback *p)
{
    p->next = gIco_Uxf_Api_Mng.CallbackFree;
    gIco_Uxf_Api_Mng.CallbackFree = p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_regist_callback: register a callback management table(internal function)
 *
 * @param[in]   p           callback management table address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_regist_callback(Ico_Uxf_Mng_Callback *p)
{
    p->next = (struct _Ico_Uxf_Mng_Callback *)0;

    if (gIco_Uxf_Api_Mng.Callback) {
        gIco_Uxf_Api_Mng.CallbackLast->next = p;
        gIco_Uxf_Api_Mng.CallbackLast = p;
    }
    else    {
        gIco_Uxf_Api_Mng.Callback = p;
        gIco_Uxf_Api_Mng.CallbackLast = gIco_Uxf_Api_Mng.Callback;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_remove_callback: remove a callback management table(internal function)
 *
 * @param[in]   p           callback management table address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_remove_callback(Ico_Uxf_Mng_Callback *p)
{
    Ico_Uxf_Mng_Callback    *pp;
    Ico_Uxf_Mng_Callback    *before;

    pp = gIco_Uxf_Api_Mng.Callback;
    before = (Ico_Uxf_Mng_Callback *)0;

    while (pp) {
        if (pp == p)   break;
        before = pp;
        pp = pp->next;
    }
    if (pp)    {
        if (before)    {
            before->next = p->next;
            if (! p->next) {
                gIco_Uxf_Api_Mng.CallbackLast = before;
            }
        }
        else    {
            gIco_Uxf_Api_Mng.Callback = p->next;
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_alloc_eventque: allocate event queue block(internal function)
 *
 * @param       none
 * @return      allocated event queue control block address
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_EventQue
*ico_uxf_alloc_eventque(void)
{
    Ico_Uxf_Mng_EventQue    *p;
    int                 idx;

    p = gIco_Uxf_Api_Mng.EventQueFree;
    if (! p)   {
        p = (Ico_Uxf_Mng_EventQue *)
                malloc(sizeof(Ico_Uxf_Mng_EventQue) * ICO_UXF_MNG_BLOCK_ALLOCS);
        if (! p)   {
            uifw_error("ico_uxf_alloc_eventque: NO Memory (malloc Error)");
            exit(1);
        }
        gIco_Uxf_Api_Mng.EventQueFree = p;

        for (idx = 0; idx < ICO_UXF_MNG_BLOCK_ALLOCS; idx++, p++)  {
            if (idx != (ICO_UXF_MNG_BLOCK_ALLOCS-1))   {
                p->next = (p+1);
            }
            else    {
                p->next = (struct _Ico_Uxf_Mng_EventQue *)0;
            }
        }
        p = gIco_Uxf_Api_Mng.EventQueFree;
    }
    gIco_Uxf_Api_Mng.EventQueFree = p->next;

    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_free_eventque: release event queue block(internal function)
 *
 * @param[in]   p       event queue control block address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_free_eventque(Ico_Uxf_Mng_EventQue *p)
{
    p->next = gIco_Uxf_Api_Mng.EventQueFree;
    gIco_Uxf_Api_Mng.EventQueFree = p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_regist_eventque: register a event queue(internal function)
 *
 * @param[in]   p       event queue control block address
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_regist_eventque(Ico_Uxf_Mng_EventQue *p)
{
    p->next = (struct _Ico_Uxf_Mng_EventQue *)0;

    if (gIco_Uxf_Api_Mng.EventQue) {
        gIco_Uxf_Api_Mng.EventQueLast->next = p;
        gIco_Uxf_Api_Mng.EventQueLast = p;
    }
    else    {
        gIco_Uxf_Api_Mng.EventQue = p;
        gIco_Uxf_Api_Mng.EventQueLast = gIco_Uxf_Api_Mng.EventQue;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_mng_display: get display management table(internal function)
 *
 * @param[in]   display     display Id
 * @param[in]   create      create flag(1=create tabell if not exist/0=not create)
 * @return      display management table address
 * @retval      !=NULL      display management table address
 * @retval      ==NULL      display dose not exist(parameter 'create' only 0)
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_Display *
ico_uxf_mng_display(const int display, const int create)
{
    Ico_Uxf_Mng_Display *p;
    Ico_Uxf_Mng_Display *plast;

    p = gIco_Uxf_Api_Mng.Mng_Display;
    plast = (Ico_Uxf_Mng_Display *)0;
    while (p)  {
        if (p->attr.display == display)    break;
        plast = p;
        p = p->next;
    }

    if ((! p) && (create != 0))   {

        p = (Ico_Uxf_Mng_Display *) malloc(sizeof(Ico_Uxf_Mng_Display));
        if (! p)   {
            uifw_error("ico_uxf_mng_display: NO Memory (malloc Error)");
            exit(1);
        }
        memset((char *)p, 0, sizeof(Ico_Uxf_Mng_Display));
        p->attr.display = display;
        if (plast) {
            plast->next = p;
        }
        else    {
            gIco_Uxf_Api_Mng.Mng_Display = p;
        }
    }
    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_mng_layer: get layer management table(internal function)
 *
 * @param[in]   display     display Id
 * @param[in]   layer       layer Id
 * @param[in]   create      create flag(1=create tabell if not exist/0=not create)
 * @return      layer management table address
 * @retval      !=NULL      layer management table address
 * @retval      ==NULL      layer dose not exist(parameter 'create' only 0)
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_Layer *
ico_uxf_mng_layer(const int display, const int layer, const int create)
{
    Ico_Uxf_Mng_Layer   *p;
    Ico_Uxf_Mng_Layer   *plast;

    p = gIco_Uxf_Api_Mng.Mng_Layer;
    plast = (Ico_Uxf_Mng_Layer *)0;
    while (p)  {
        if ((p->attr.display == display) &&
            (p->attr.layer == layer))  break;
        plast = p;
        p = p->next;
    }

    if ((! p) && (create != 0))   {

        p = (Ico_Uxf_Mng_Layer *) malloc(sizeof(Ico_Uxf_Mng_Layer));
        if (! p)   {
            uifw_error("ico_uxf_mng_layer: NO Memory (malloc Error)");
            exit(1);
        }
        memset((char *)p, 0, sizeof(Ico_Uxf_Mng_Layer));
        p->attr.layer = layer;
        if (plast) {
            plast->next = p;
        }
        else    {
            gIco_Uxf_Api_Mng.Mng_Layer = p;
        }
    }
    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_mng_window: get window management table(internal function)
 *
 * @param[in]   window      window Id(same as ico_window_mgr surface Id)
 * @param[in]   create      create flag(1=create tabell if not exist/0=not create)
 * @return      window management table address
 * @retval      !=NULL      window management table address
 * @retval      ==NULL      window dose not exist(parameter 'create' only 0)
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_Window *
ico_uxf_mng_window(const int window, const int create)
{
    Ico_Uxf_Mng_Window  *p;
    Ico_Uxf_Mng_Window  *plast = (Ico_Uxf_Mng_Window *)0;
    int                 hash;

    hash = ICO_UXF_MISC_HASHBYID(window);
    p = gIco_Uxf_Api_Mng.Hash_WindowId[hash];
    while (p)  {
        if (p->attr.window == window)  break;
        plast = p;
        p = p->nextidhash;
    }

    if ((! p) && (create != 0))    {

        p = (Ico_Uxf_Mng_Window *) malloc(sizeof(Ico_Uxf_Mng_Window));
        if (! p)   {
            uifw_error("ico_uxf_mng_window: NO Memory (malloc Error)");
            exit(1);
        }
        memset((char *)p, 0, sizeof(Ico_Uxf_Mng_Window));
        p->attr.window = window;
        if (plast) {
            plast->nextidhash = p;
        }
        else    {
            gIco_Uxf_Api_Mng.Hash_WindowId[hash] = p;
        }
        uifw_trace("ico_uxf_mng_window: Create New Table=%08x", (int)p);
    }
    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_mng_process: get process management table(internal function)
 *
 * @param[in]   process     application Id
 * @param[in]   create      create flag(1=create tabell if not exist/0=not create)
 * @return      process management table address
 * @retval      !=NULL      process management table address
 * @retval      ==NULL      process dose not exist(parameter 'create' only 0)
 */
/*--------------------------------------------------------------------------*/
Ico_Uxf_Mng_Process *
ico_uxf_mng_process(const char *process, const int create)
{
    Ico_Uxf_Mng_Process *p;
    Ico_Uxf_Mng_Process *plast = (Ico_Uxf_Mng_Process *)0;
    int                 hash;

    hash = ICO_UXF_MISC_HASHBYNAME(process);
    p = gIco_Uxf_Api_Mng.Hash_ProcessId[hash];
    while (p)  {
        if(strncmp(p->attr.process, process, ICO_UXF_MAX_PROCESS_NAME) == 0) break;
        plast = p;
        p = p->nextidhash;
    }

    if ((! p) && (create != 0))   {

        p = (Ico_Uxf_Mng_Process *) malloc(sizeof(Ico_Uxf_Mng_Process));
        if (! p)   {
            uifw_error("ico_uxf_mng_process: NO Memory (malloc Error)");
            exit(1);
        }
        memset((char *)p, 0, sizeof(Ico_Uxf_Mng_Process));
        strncpy(p->attr.process, process, ICO_UXF_MAX_PROCESS_NAME);
        if (plast) {
            plast->nextidhash = p;
        }
        else    {
            gIco_Uxf_Api_Mng.Hash_ProcessId[hash] = p;
        }
    }
    return p;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_update_procwin: update a process management table
 *
 * @param[in]   appid       application id
 * @param[in]   type        type(install/uninstall)
 * @param[in]   func        window create/destroy hook function
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_update_procwin(const char *appid, int type)
{
    Ico_Uxf_Mng_Process     *prc = NULL;
    Ico_Uxf_Sys_Config      *sysconf;
    Ico_Uxf_App_Config      *appconf;
    Ico_Uxf_conf_appdisplay *appdsp;
    int                     dn, tn;
    Ico_Uxf_conf_application *app = NULL;

    uifw_trace("ico_uxf_update_procwin: Enter(appid=%s, type=%d)", appid, type);

    if (gIco_Uxf_Api_Mng.Initialized <= 0) {
        uifw_trace("ico_uxf_callback_remove: Leave(not initialized)");
        return;
    }
    prc = ico_uxf_mng_process(appid, 0);

    if (type == ICO_UXF_CONF_EVENT_INSTALL) {
        if (!prc) {
            sysconf = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
            appconf = (Ico_Uxf_App_Config *)ico_uxf_getAppConfig();
            if ((sysconf == NULL) || (appconf == NULL)) {
                uifw_trace("ico_uxf_update_procwin: Leave(cannot find tables)");
                return;
            }
            for (dn = 0;  dn < appconf->applicationNum; dn++) {
                app = &appconf->application[dn];
                if (strncmp(app->appid, appid, ICO_UXF_MAX_PROCESS_NAME) == 0) {
                    uifw_trace("ico_uxf_update_procwin: Install(%s)", appid);
                    /* add process management table */
                    prc = ico_uxf_mng_process(appid, 1);
                    prc->attr.internalid = 0;
                    prc->attr.status = ICO_UXF_PROCSTATUS_STOP;
                    prc->attr.type = app->categoryId;
                    prc->attr.hostId = app->hostId;
                    prc->attr.myHost = (prc->attr.hostId == sysconf->misc.myhostId) ? 1 : 0;

                    appdsp = &app->display[0];
                    prc->attr.mainwin.window = 0;
                    prc->attr.mainwin.windowtype = app->categoryId;
                    prc->attr.mainwin.display = appdsp->displayId;
                    prc->attr.mainwin.layer = appdsp->layerId;
                    prc->attr.mainwin.x =
                            sysconf->display[appdsp->displayId].zone[appdsp->zoneId].x;
                    prc->attr.mainwin.y =
                            sysconf->display[appdsp->displayId].zone[appdsp->zoneId].y;
                    prc->attr.mainwin.w =
                            sysconf->display[appdsp->displayId].zone[appdsp->zoneId].width;
                    prc->attr.mainwin.h =
                            sysconf->display[appdsp->displayId].zone[appdsp->zoneId].height;
                    prc->attr.mainwin.name[ICO_UXF_MAX_WIN_NAME] = 0;
                    prc->attr.numwindows = app->displayzoneNum;
                    /* get sub windows                          */
                    if (prc->attr.numwindows > 1)   {
                        prc->attr.subwin = malloc(sizeof(Ico_Uxf_ProcessWin) *
                                                  (prc->attr.numwindows - 1));
                        if (! prc->attr.subwin) {
                            uifw_trace("ico_uxf_update_procwin: Install(No memory)");
                            app->displayzoneNum = 1;
                            prc->attr.numwindows = 1;
                        }
                        else    {
                            memset(prc->attr.subwin, 0, (prc->attr.numwindows - 1));
                            for (tn = 0; tn < (prc->attr.numwindows - 1); tn++) {
                                appdsp ++;
                                prc->attr.subwin[tn].windowtype = prc->attr.mainwin.windowtype;
                                prc->attr.subwin[tn].display = appdsp->displayId;
                                prc->attr.subwin[tn].layer = appdsp->layerId;
                                prc->attr.subwin[tn].x =
                                    sysconf->display[appdsp->displayId].zone[appdsp->zoneId].x;
                                prc->attr.subwin[tn].y =
                                    sysconf->display[appdsp->displayId].zone[appdsp->zoneId].y;
                                prc->attr.subwin[tn].w =
                                    sysconf->display[appdsp->displayId].zone[appdsp->zoneId].width;
                                prc->attr.subwin[tn].h =
                                    sysconf->display[appdsp->displayId].zone[appdsp->zoneId].height;
                            }
                        }
                    }
                }
            }
        }
    }
    else if (type == ICO_UXF_CONF_EVENT_UNINSTALL) {
        if (prc) {
            /* not delete process management table */
        }
    }

    uifw_trace("ico_uxf_update_procwin: Leave");

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_screen_size_get: get display physical size
 *
 * @param[out]  width       variable to receive the width of the display
 * @param[out]  height      variable to receive the height of the display
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_window_screen_size_get(int *width, int *height)
{
    Ico_Uxf_Mng_Display *dsp;
    dsp = ico_uxf_mng_display(gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display, 0);

    uifw_trace("ico_uxf_window_screen_size_get: Enter(dsp=%d(%d))",
               dsp, gIco_Uxf_Api_Mng.Mng_MyProcess->attr.mainwin.display);

    if ((dsp != NULL) && (width != NULL) && (height != NULL)) {
        uifw_trace("ico_uxf_window_screen_size_get: %dx%d",
                   dsp->attr.pWidth, dsp->attr.pHeight);
        *width = dsp->attr.pWidth;
        *height = dsp->attr.pHeight;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_window_hook: set window create/destroy hook function
 *
 * @param[in]   func        window create/destroy hook function
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_window_hook(Ico_Uxf_Hook func)
{
    uifw_trace("ico_uxf_window_hook: Window Control Hook=%08x", (int)func);

    gIco_Uxf_Api_Mng.Hook_Window = func;
}

