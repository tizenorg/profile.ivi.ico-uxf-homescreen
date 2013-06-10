/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of user experience library for HomeScreen(data structure)
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_TYPEDEF_H_
#define _ICO_UXF_TYPEDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Structure of display attribute           */
typedef struct  _Ico_Uxf_DisplayAttr    {
    int                 display;        /* Display ID                           */
    int                 type;           /* Type                                 */
    short               num_layers;     /* Number of layers                     */
    short               hostId;         /* Host ID                              */
    short               displayNo;      /* Consecutive numbers                  */
    short               w;              /* The number of the lateral pixels     */
    short               h;              /* The number of vertical pixels        */
    short               inch;           /* Screen physical size(0.1 inches of units)*/
    short               pWidth;         /* compositor width                     */
    short               pHeight;        /* compositor height                    */
    short               orientation;
#define ICO_UXF_ORIENTATION_HORIZONTAL  (0)
#define ICO_UXF_ORIENTATION_VERTICAL    (1)
    char                name[ICO_UXF_MAX_WIN_NAME+1];
                                        /* Display name                         */
}   Ico_Uxf_DisplayAttr;

/* Structure of layer attribute             */
typedef struct  _Ico_Uxf_LayerAttr  {
    int                 layer;          /* Layer ID                             */
    int                 display;        /* Display ID                           */
    short               w;              /* The number of the lateral pixels     */
    short               h;              /* The number of vertical pixels        */
    char                menuoverlap;    /* Overlap HomeScreen menu              */
    char                res[3];         /* (unused)                             */
}   Ico_Uxf_LayerAttr;

/* Structure of display zone attribute      */
typedef struct  _Ico_Uxf_DispZoneAttr  {
    int                 zone;           /* Zone ID                              */
    int                 display;        /* Display ID                           */
    short               x;              /* Left upper X coordinate of the screen*/
    short               y;              /* Left upper Y coordinate of the screen*/
    short               w;              /* The number of the lateral pixels     */
    short               h;              /* The number of vertical pixels        */
}   Ico_Uxf_DispZoneAttr;

/* Structure of window attribute            */
typedef struct  _Ico_Uxf_WindowAttr {
    int                 window;         /* Window ID                            */
    int                 display;        /* Display ID                           */
    int                 layer;          /* Layer ID                             */
    int                 zone;           /* Display Zone ID                      */
    char                process[ICO_UXF_MAX_PROCESS_NAME+1];
                                        /* Application ID                       */
    int                 subwindow;      /* Main window(=0) or Sub window(>0)    */
    unsigned int        eventmask;      /* Event to receive through this window */
    short               x;              /* Upper left X coordinate of the window*/
    short               y;              /* Upper left Y coordinate of the window*/
    short               w;              /* Width of window                      */
    short               h;              /* height of window                     */
    char                control;        /* Visibility control from AppsController*/
    char                visible;        /* Visibility                           */
    char                raise;          /* Raise/Lower                          */
    char                active;         /* Active/Inactive                      */
}   Ico_Uxf_WindowAttr;

/* Structure of input code                  */
typedef struct _Ico_Uxf_InputCode   {
    int         code;                       /* input switch code value              */
    char        codename[20];               /* input switch code name               */
}   Ico_Uxf_InputCode;

/* Structure of input switch                */
typedef struct _Ico_Uxf_InputSw {
    struct _Ico_Uxf_InputSw     *next;      /* next input switch table              */
    char        swname[20];                 /* input switch name                    */
    int         input;                      /* input switch number                  */
    short       fix;                        /* fixed application switch             */
    short       numCode;                    /* number of codes                      */
    Ico_Uxf_InputCode   inputCode[8];       /* code table                           */
}   Ico_Uxf_InputSw;

/* Structure of input device                */
typedef struct _Ico_Uxf_InputDev    {
    struct _Ico_Uxf_InputDev    *next;      /* next input device                    */
    char        device[32];                 /* input device name                    */
    int         type;                       /* input device type(unused)            */
    int         numInputSw;                 /* number of input switch               */
    Ico_Uxf_InputSw     *inputSw;           /* input switch list                    */
}   Ico_Uxf_InputDev;

/* Structure of window of application       */
typedef struct  _Ico_Uxf_ProcessWin {
    int                 window;         /* Window ID                            */
                                        /* (0 if window not create)             */
    int                 windowtype;     /* Type                                 */
    int                 display;        /* Display ID                           */
    int                 layer;          /* Layer ID                             */
    int                 zone;           /* Zone ID                              */
    short               x;              /* Default X coordinate of the window   */
    short               y;              /* Default Y coordinate of the window   */
    short               w;              /* Default width of window              */
    short               h;              /* Default height of window             */
    char                name[ICO_UXF_MAX_WIN_NAME+1];
                                        /* Window name                          */
}   Ico_Uxf_ProcessWin;

/* Structure of application attribute       */
typedef struct  _Ico_Uxf_ProcessAttr    {
    char                process[ICO_UXF_MAX_PROCESS_NAME+1];
                                        /* Application ID                       */
    int                 internalid;     /* ProcessID                            */
    int                 type;           /* Type                                 */
    short               hostId;         /* Host ID who works of the application */
    short               myHost;         /* Works in my host(=1) or Other host(=0)*/
    short               numwindows;     /* Number of windows                    */
    short               numsounds;      /* Number of sound                      */
    short               numinputs;      /* Number of input                      */
    Ico_Uxf_ProcessWin  mainwin;        /* Main window of application           */
    Ico_Uxf_ProcessWin  *subwin;        /* Sub window of application            */
    short               status;         /* Running status                       */
    char                active;         /* Active/Inactive                      */
    char                child;          /* Child process                        */
    char                noicon;         /* No icon flag                         */
    char                autostart;      /* Autostart flag                       */
    char                suspend;        /* application susupend flag            */
    unsigned char       invisiblecpu;   /* cpu % at invisible                   */
}   Ico_Uxf_ProcessAttr;

/* Detail of event                          */
typedef union  _Ico_Uxf_EventDetail    {
    int                 event;          /* Event number                         */
    struct  {                           /* Window event                         */
        int             event;          /* Event number                         */
        /* Event about the state conversion of the window                       */
        /* ICO_UXF_EVENT_VISIBLE,RESIZE,MOVE,REDRAW,NEWWINDOW,DESTORYWINDOW     */
        int             display;        /* Display ID                           */
        int             window;         /* Window ID                            */
        int             layer;          /* layer ID                             */
        short           x;              /* Window position                      */
        short           y;
        short           w;              /* Window size                          */
        short           h;
        char            visible;        /* Visibility                           */
        char            raise;          /* Raise/Lower                          */
        char            active;         /* Active/inactive(not impliment)       */
        char            res;            /* (unuse)                              */
    }           window;

    struct  {                           /* Application event                    */
        int             event;          /* Event number                         */
        /* Event about the operating conditions change of the application       */
        /* EXECPROCESS, TERMPROCESS                                             */
        char            process[ICO_UXF_MAX_PROCESS_NAME+1];
                                        /* Application ID                       */
        int             status;         /* Running status                       */
        int             regulation;     /* Current regulation                   */
    }           process;

    struct {                            /* Input Manager event                  */
        int             event;          /* Event number                         */
        /* Multi Input Manager event                                            */
        /* ICO_UXF_EVENT_EXINPUT                                                */
        char            device[32];     /* input device name                    */
        int             input;          /* input number                         */
        int             code;           /* input code                           */
        int             state;          /* input status                         */
    }           exinput;

}   Ico_Uxf_EventDetail;

/* Event callback function                  */
typedef void (*Ico_Uxf_Event_Cb)(const int event,
                                 const Ico_Uxf_EventDetail detail, int arg);

/* Hook function                            */
typedef void (*Ico_Uxf_Hook)(const char *appid, const int info1, const int info2);

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_UXF_TYPEDEF_H_ */

