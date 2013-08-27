/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experience library internal header
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_INTERNAL_H_
#define _ICO_UXF_INTERNAL_H_

#include    <wayland-client.h>              /* Wayland client library               */
#include    <wayland-egl.h>                 /* Wayland EGL library                  */
#include    <wayland-util.h>                /* Wayland Misc library                 */
#include    <aul/aul.h>                     /* AppCore                              */

#include    "ico_uxf_sysdef.h"              /* UX-FW System convention value        */
#include    "ico_uxf_def.h"                 /* UX-FW definition value               */
#include    "ico_apf_log.h"                 /* wide use log definition              */
#include    "ico_apf_apimacro.h"

#include    "ico_input_mgr-client-protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

/* macro                                            */
                                            /* get DisplayId from SurfaceId         */
#define ICO_UXF_GETDISPLAYID(surfaceid)   \
            ((surfaceid >> 16) & 0x00ff)

/* define struct                                    */
/* struct management callback function              */
typedef struct  _Ico_Uxf_Mng_Callback   {
    struct _Ico_Uxf_Mng_Callback    *next;  /* next callback struct                 */
    unsigned int        eventmask;          /* notify event mask                    */
    Ico_Uxf_Event_Cb    func;               /* callback function address            */
    int                 arg;                /* function's argument                  */
}   Ico_Uxf_Mng_Callback;

/* struct management event queue                    */
typedef struct  _Ico_Uxf_Mng_EventQue   {
    struct _Ico_Uxf_Mng_EventQue    *next;  /* next event queue struct              */
    Ico_Uxf_EventDetail detail;             /* event detail                         */
}   Ico_Uxf_Mng_EventQue;

/* struct management window which proccess owns     */
typedef struct  _Ico_Uxf_Mng_ProcWin    {
    struct _Ico_Uxf_Mng_ProcWin *next;      /* next window                          */
    Ico_Uxf_ProcessWin  attr;               /* window configuration                 */
}   Ico_Uxf_Mng_ProcWin;

/* struct management proccess                       */
typedef struct  _Ico_Uxf_Mng_Process    {
    struct _Ico_Uxf_Mng_Process *nextidhash;/* next process of the same id and hash */
    Ico_Uxf_Mng_ProcWin *procwin;           /* process's subwindow                  */
    void                *appconf;           /* configuration table address          */
    struct _Ico_Uxf_Mng_Process *parent;    /* parent process                       */
    short               susptimer;          /* suspend timer                        */
    char                susp;               /* real suspend flag                    */
    char                showmode;           /* resume show mode                     */
    Ico_Uxf_ProcessAttr attr;               /* process configuration                */
}   Ico_Uxf_Mng_Process;

/* struct management display                        */
typedef struct  _Ico_Uxf_Mng_Display    {
    struct _Ico_Uxf_Mng_Display *next;      /* next display                         */
    Ico_Uxf_DisplayAttr attr;               /* display configuration                */
}   Ico_Uxf_Mng_Display;

/* struct management layer                          */
typedef struct  _Ico_Uxf_Mng_Layer  {
    struct _Ico_Uxf_Mng_Layer   *next;      /* next layer                           */
    Ico_Uxf_Mng_Display *mng_display;       /* display local cache                  */
    Ico_Uxf_LayerAttr   attr;               /* layer attribute                      */
}   Ico_Uxf_Mng_Layer;

#define ICO_UXF_RESREQ_CLIENT       1       /* Request from client                  */
#define ICO_UXF_RESREQ_AUTO         2       /* Automatic request                    */
#define ICO_UXF_RESSTAT_NONE        0       /* Resource status: None                */
#define ICO_UXF_RESSTAT_ACQUIRED    1       /* Resource status: Acruired            */
#define ICO_UXF_RESSTAT_DEPRIVED    2       /* Resource status: Deprived            */

typedef struct  _Ico_Uxf_Mng_Window {
    struct _Ico_Uxf_Mng_Window  *nextidhash;/* next window of the same id and hash  */
    Ico_Uxf_Mng_Display *mng_display;       /* display local cache                  */
    Ico_Uxf_Mng_Layer   *mng_layer;         /* layer local cache                    */
    Ico_Uxf_Mng_Process *mng_process;       /* process local cache                  */
    struct _Ico_Uxf_Mng_Window *mng_parent; /* parent window                        */
    short               state;              /* Window status                        */
    unsigned char       request;            /* Request                              */
    char                res;                /* (unused)                             */
    Ico_Uxf_WindowAttr  attr;               /* window attribute                     */
}   Ico_Uxf_Mng_Window;

/* struct management listener added by user         */
typedef struct _Ico_Uxf_Mng_EventListener {
    struct _Ico_Uxf_Mng_EventListener *next;/* next table management listener function*/
    void        (*func)(void *);            /* listener function address            */
    void        *arg;                       /* listener function's argument         */
}   Ico_Uxf_Mng_EventListener;

/* struct management UX-FW API                      */
typedef struct  _Ico_Uxf_Api_Mng    {
    /* variable                             */
    int                     Initialized;    /* initialized flag                     */
    int                     EventMask;      /* receive event                        */
    unsigned int            LastEvent;      /* last receive event(end synchronization)*/
    int                     AppsCtlVisible; /* Tempolary visible all windows        */
    short                   Win_Show_Anima; /* Window animation at show             */
    short                   Win_Hide_Anima; /* Window animation at hide             */
    int                     NeedTimer;      /* need timer control                   */
    int                     InitTimer;      /* initial timer                        */

    /* process information myprocess        */
    char                    MyProcess[ICO_UXF_MAX_PROCESS_NAME+1];
                                            /* process id                           */
    Ico_Uxf_Mng_Process     *Mng_MyProcess; /* process management table             */
    int                     MyMainWindow;   /* main window id                       */
    Ico_Uxf_Mng_Process     *Mng_LastProcess;/* last active process management table*/

    /* table management callback function            */
    Ico_Uxf_Mng_Callback    *Callback;      /* callback management table            */
    Ico_Uxf_Mng_Callback    *CallbackLast;  /* last at callback management table's  */
    Ico_Uxf_Mng_Callback    *CallbackFree;  /* free callback management table       */

    /* event queue                                   */
    Ico_Uxf_Mng_EventQue    *EventQue;      /* event queue list                     */
    Ico_Uxf_Mng_EventQue    *EventQueLast;  /* last at event queue list             */
    Ico_Uxf_Mng_EventQue    *EventQueFree;  /* opening event queue                  */

    /* table management resource                     */
    Ico_Uxf_Mng_Display     *Mng_Display;   /* display attribute cache              */
    Ico_Uxf_Mng_Layer       *Mng_Layer;     /* layer attribute cache                */
    Ico_Uxf_Mng_Process     *Hash_ProcessId[ICO_UXF_MISC_HASHSIZE];
                                            /* process attribute table's hash       */
    Ico_Uxf_Mng_Window      *Hash_WindowId[ICO_UXF_MISC_HASHSIZE];
                                            /* window attribute table's hash        */
    Ico_Uxf_InputDev        *InputDev;      /* Input device table                   */

    /* Wayland                          */
    struct wl_display       *Wayland_Display;   /* Wayland's display                */
    struct wl_registry      *Wayland_Registry;  /* Wayland's Registry               */
    struct wl_compositor    *Wayland_Compositor;/* Wayland's compositor             */
    struct wl_seat          *Wayland_Seat;      /* Wayland's device input event     */
    struct ico_window_mgr   *Wayland_WindowMgr; /* Wayland's Window Manager PlugIn  */
    struct wl_output        *Wayland_Output;    /* Wayland's output describes       */
    int                     WaylandFd;          /* Wayland's file descriptor        */
    struct ico_input_mgr_control
                            *Wayland_InputMgr;  /* Input Manager PulgIn             */
    struct ico_exinput      *Wayland_exInput;   /* extra input event                */

    /* Hook routines                    */
    Ico_Uxf_Hook            Hook_Window;        /* Window control hook              */
}   Ico_Uxf_Api_Mng;

/* define inner misc function               */
void ico_uxf_enter_critical(void);          /* Exclusion section start              */
void ico_uxf_leave_critical(void);          /* Exclusion section end                */
void ico_uxf_broadcast_signal(void);        /* Dissolution of MainLoop's wait       */
Ico_Uxf_Mng_Callback *ico_uxf_alloc_callback(void);
                                            /* get a callback management table      */
void ico_uxf_free_callback(Ico_Uxf_Mng_Callback *p);
                                            /* free a callback management table     */
void ico_uxf_regist_callback(Ico_Uxf_Mng_Callback *p);
                                            /* register callback management table   */
void ico_uxf_remove_callback(Ico_Uxf_Mng_Callback *p);
                                            /* deregister callback management table */
Ico_Uxf_Mng_EventQue *ico_uxf_alloc_eventque(void);
                                            /* allocate event queue block           */
void ico_uxf_free_eventque(Ico_Uxf_Mng_EventQue *p);
                                            /* free a event queue block             */
void ico_uxf_regist_eventque(Ico_Uxf_Mng_EventQue *p);
                                            /* register event queue block           */
Ico_Uxf_Mng_Display *ico_uxf_mng_display(const int display, const int create);
                                            /* get a display management table       */
Ico_Uxf_Mng_Layer *ico_uxf_mng_layer(const int display, const int layer,
                                     const int create);
                                            /* get a layer management table         */
Ico_Uxf_Mng_Window *ico_uxf_mng_window(const int window, const int create);
                                            /* get a window management table        */
Ico_Uxf_Mng_Process *ico_uxf_mng_process(const char *process, const int create);
                                            /* get a process management table       */
Ico_Uxf_Mng_Process *ico_uxf_mng_process_find(const char *process, const int pid);
                                            /* get a process management table       */
void ico_uxf_update_procwin(const char *appid, int type);
                                            /* update a process management table    */
void ico_uxf_free_procwin(Ico_Uxf_Mng_Process *prc);
                                            /* release application windows          */
#define ICO_UXF_WITHANIMA   0x0100          /* with animation                       */
void ico_uxf_window_visible_control(Ico_Uxf_Mng_Window *winmng,
                                    const int show, const int raise);
                                            /* Window visible and cpu control       */
int ico_uxf_gl_create_window(const int display, const int layer, const int x,
                             const int y, const int w, const int h, const int full);
                                            /* create window by OpenGL ES/EGL       */

/* define log macros    */
#ifndef uifw_trace
#define uifw_trace      ICO_UXF_DEBUG
#define uifw_critical   ICO_UXF_CRITICAL
#define uifw_info       ICO_UXF_INFO
#define uifw_warn       ICO_UXF_WARN
#define uifw_error      ICO_UXF_ERROR
#define uifw_logflush   ico_uxf_log_flush
#endif  /*uifw_trace*/

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_UXF_INTERNAL_H_ */

