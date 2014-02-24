/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller
 *          for privilege and general applications
 *
 * @date    Feb-21-2014
 */

#ifndef _ICO_SYC_TYPE_H_
#define _ICO_SYC_TYPE_H_

#include <ico_window_mgr-client-protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* global API                                                                 */
/*============================================================================*/
#if defined(__GNUC__) && __GNUC__ >= 4
#define ICO_API __attribute__ ((visibility("default")))
#else
#define ICO_API
#endif

/*============================================================================*/
/* enum                                                                       */
/*============================================================================*/
/*
 * id of event
 */
typedef enum _event_id {
    ICO_SYC_EV_WIN_CREATE           = 1,  /* create window */
    ICO_SYC_EV_WIN_DESTROY          = 2,  /* destroy window */
    ICO_SYC_EV_WIN_ACTIVE           = 3,  /* active window */
    ICO_SYC_EV_WIN_ATTR_CHANGE      = 4,  /* change window attribute */
    ICO_SYC_EV_THUMB_CHANGE         = 5,  /* map thumbnail data */
    ICO_SYC_EV_THUMB_UNMAP          = 6,  /* unmap thumbnail data */
    ICO_SYC_EV_THUMB_ERROR          = 7,  /* error map thumbnail */
    ICO_SYC_EV_LAYER_ATTR_CHANGE    = 8,  /* change layer attribute */
    ICO_SYC_EV_USERLIST             = 9,  /* notify the user list */
    ICO_SYC_EV_AUTH_FAIL            = 10, /* fail in the user authentication */
    ICO_SYC_EV_RES_ACQUIRE          = 11, /* acquired resource */
    ICO_SYC_EV_RES_DEPRIVE          = 12, /* deprived resource */
    ICO_SYC_EV_RES_WAITING          = 13, /* waiting resource */
    ICO_SYC_EV_RES_REVERT           = 14, /* reverted resource */
    ICO_SYC_EV_RES_RELEASE          = 15, /* released resource */
    ICO_SYC_EV_INPUT_SET            = 16, /* set input region */
    ICO_SYC_EV_INPUT_UNSET          = 17, /* unset input region */
    ICO_SYC_EV_LASTINFO             = 18, /* notify the last information */
    ICO_SYC_EV_STATE_CHANGE         = 19, /* notify changed state */
    ICO_SYC_EV_WIN_NAME             = 20  /* notify window name */
} ico_syc_ev_e;

/*
 * general fixed value for window attribute
 * @ICO_SYC_WIN_NOCHANGE: no change value
 */
typedef enum _win_value {
    ICO_SYC_WIN_NOCHANGE    = 99999999
} ico_syc_win_value_e;

/*
 * thumbnail data format
 * @ICO_SYC_THUMB_FORMAT_ARGB: argb format
 * @ICO_SYC_THUMB_FORMAT_XRGB: xrgb format
 */
typedef enum _thumb_format {
    ICO_SYC_THUMB_FORMAT_ARGB = WL_SHM_FORMAT_ARGB8888,
    ICO_SYC_THUMB_FORMAT_XRGB = WL_SHM_FORMAT_XRGB8888
} ico_syc_thumb_format_e;

/*
 * window raise/lower status
 * @ICO_SYC_WIN_RAISE_RAISE: raise the surface
 * @ICO_SYC_WIN_RAISE_LOWER: lower the surface
 * @ICO_SYC_WIN_RAISE_NOCHANGE: order of showing surface is not change
 */
typedef enum _window_raise {
    ICO_SYC_WIN_RAISE_RAISE     = 1,
    ICO_SYC_WIN_RAISE_LOWER     = 0,
    ICO_SYC_WIN_RAISE_NOCHANGE  = 9
} ico_syc_win_raise_e;

/*
 * window show/hide status
 * @ICO_SYC_WIN_VISIBLE_SHOW: show the surface
 * @ICO_SYC_WIN_VISIBLE_HIDE: hide the surface
 * @ICO_SYC_WIN_VISIBLE_NOCHANGE: show/hide status is not change
 */
typedef enum _window_visible {
    ICO_SYC_WIN_VISIBLE_SHOW     = 1,
    ICO_SYC_WIN_VISIBLE_HIDE     = 0,
    ICO_SYC_WIN_VISIBLE_NOCHANGE = 9
} ico_syc_win_visible_e;

/*
 * window change hint
 * @ICO_SYC_WIN_HINT_HINT: hint information(no change)
 * @ICO_SYC_WIN_HINT_CHANGE: real changed
 */
typedef enum _window_change_hint {
    ICO_SYC_WIN_HINT_HINT     = 1,
    ICO_SYC_WIN_HINT_CHANGE   = 0
} ico_syc_win_change_hint_e;

/*
 * type of window active select
 * @ICO_SYC_WIN_ACTIVE_NONE: no device(surface not active)
 * @ICO_SYC_WIN_ACTIVE_POINTER: active by pointing device(mouse)
 * @ICO_SYC_WIN_ACTIVE_TOUCH: active by touchpanel device
 */
typedef enum _window_active_select {
    ICO_SYC_WIN_ACTIVE_NONE     = 0,
    ICO_SYC_WIN_ACTIVE_POINTER  = 1,
    ICO_SYC_WIN_ACTIVE_TOUCH = 2
} ico_syc_win_act_select_e;

/*
 * layer show/hide status
 * @ICO_SYC_LAYER_VISIBLE_SHOW: show the layer
 * @ICO_SYC_LAYER_VISIBLE_HIDE: hide the layer
 */
typedef enum _layer_visible {
    ICO_SYC_LAYER_VISIBLE_SHOW  = 1,
    ICO_SYC_LAYER_VISIBLE_HIDE  = 0,
    ICO_SYC_LAYER_VISIBLE_NOCHANGE  = 9
} ico_syc_layer_visible_e;

/*
 * surface animation on/off
 * @ICO_SYC_ANIMATION_ON: surface animation
 * @ICO_SYC_ANIMATION_OFF: no surface animation
 */
typedef enum _surface_animation {
    ICO_SYC_ANIMATION_ON  = 1,
    ICO_SYC_ANIMATION_OFF  = 0
} ico_syc_surface_animation;

/*
 * type of window aspect
 * @ICO_SYC_WIN_ASPECT_FIXED: fixed aspect
 * @ICO_SYC_WIN_ASPECT_ALIGN_LEFT: left align
 * @ICO_SYC_WIN_ASPECT_ALIGN_RIGHT: right align
 * @ICO_SYC_WIN_ASPECT_ALIGN_TOP: top align
 * @ICO_SYC_WIN_ASPECT_ALIGN_BOTTOM: bottom align
 */
typedef enum _window_aspect {
    ICO_SYC_WIN_ASPECT_FIXED     = 16,
    ICO_SYC_WIN_ASPECT_ALIGN_LEFT  = 1,
    ICO_SYC_WIN_ASPECT_ALIGN_RIGHT  = 2,
    ICO_SYC_WIN_ASPECT_ALIGN_TOP  = 4,
    ICO_SYC_WIN_ASPECT_ALIGN_BOTTOM  = 8,
} ico_syc_win_aspect_e;

/**
 *  system state
 */
typedef enum _system_state {
    ICO_SYC_STATE_REGULATION = 1,   /**< regulation state */
    ICO_SYC_STATE_NIGHTMODE  = 2    /**< night mode state */
} ico_syc_system_state_e;

/**
 *  state on/off
 */
typedef enum _state_onoff {
    ICO_SYC_STATE_OFF = 0,   /**< state of off */
    ICO_SYC_STATE_ON  = 1    /**< state of on */
} ico_syc_state_onoff_e;

/*============================================================================*/
/* structure                                                                  */
/*============================================================================*/
/*
 * window move information (zone/position/size)
 */
typedef struct _win_move {
    char *zone;     /* area of showing application window */
    int  layer;     /* layer of showing application window */
    int  pos_x;     /* window's display position (x) */
    int  pos_y;     /* window's display position (y) */
    int  width;     /* window width */
    int  height;    /* window height */
} ico_syc_win_move_t;

/*
 * window information
 */
typedef struct _win_info {
    char *appid;    /* application id */
    char *name;     /* window's surface name */
    int  surface;   /* window's surface id */
} ico_syc_win_info_t;

/*
 * window attributes
 */
typedef struct _win_attr {
    char *appid;    /* application id */
    char *name;     /* window's surface name */
    char *zone;     /* window's current zone name */
    int  surface;   /* window's surface id */
    int  nodeid;    /* ecu number and display number in the ecu */
    int  layer;     /* id of layer that the application's window is on */
    int  pos_x;     /* window's display position (x) */
    int  pos_y;     /* window's display position (y) */
    int  width;     /* window width */
    int  height;    /* window height */
    int  raise;     /* changed order showing surface in the layer */
    int  visible;   /* visible status */
    int  active;    /* window active status */
} ico_syc_win_attr_t;

/*
 * thumbnail information
 * (notify from callback function)
 */
typedef struct _thumb_info {
    char *appid;    /* application id */
    int  surface;   /* window's surface id */
    int  type;      /* buffer type(ICO_WINDOW_MGR_MAP_TYPE_EGL/SHM/PIXEL)*/
    int  width;     /* window width */
    int  height;    /* window height */
    int  stride;    /* byte par line of frame buffer */
    int  format;    /* format of buffer */
} ico_syc_thumb_info_t;

/*
 * layer attributes
 */
typedef struct _layer_attr {
    int  layer;     /* layer id */
    int  visible;   /* visible status */
} ico_syc_layer_attr_t;

/*
 * user list
 */
typedef struct _user_list {
    int  user_num;      /* number of user */
    char **userlist;    /* address of user list */
    char *user_login;   /* name of login user */
} ico_syc_userlist_t;

/*
 * window resource information
 */
typedef struct _res_win {
    char *zone;         /* area of showing application window */
    char *name;         /* window's surface name */
    char *id;           /* window id */
} ico_syc_res_window_t;

/*
 * sound resource information
 */
typedef struct _res_sound {
    char *zone;         /* area of playing sound */
    char *name;         /* sound stream name */
    char *id;           /* sound id */
    int  adjust;        /* adjust action */
} ico_syc_res_sound_t;

/*
 * input resource information
 */
typedef struct _res_input {
    char *name;         /* input device name */
    int  event;         /* input event id */
} ico_syc_res_input_t;

/*
 * input region information
 */
#define ICO_SYC_MAX_WINNAME_LEN 40
typedef struct _input_region {
    char    winname[ICO_SYC_MAX_WINNAME_LEN];
                            /* target window name                   */
    short   pos_x;          /* input region X coordinate of surface */
    short   pos_y;          /* input region Y coordinate of surface */
    short   width;          /* input region's width                 */
    short   height;         /* input region's height                */
    short   hotspot_x;      /* hotspot of X relative coordinate     */
    short   hotspot_y;      /* hotspot of Y relative coordinate     */
    short   cursor_x;       /* cursor region X coordinate           */
    short   cursor_y;       /* cursor region X coordinate           */
    short   cursor_width;   /* cursor region width                  */
    short   cursor_height;  /* cursor region height                 */
    int     attr;           /* region attributes(currently unused)  */
} ico_syc_input_region_t;

/*
 * resource(window/sound/input) information
 */
typedef struct _res_info {
    ico_syc_res_window_t *window;  /* window resource information */
    ico_syc_res_sound_t  *sound;   /* sound resource information */
    ico_syc_res_input_t  *input;   /* input resource information */
} ico_syc_res_info_t;

/*
 * notify changed state information
 */
typedef struct _state_info {
    int id;
    int state;
} ico_syc_state_info_t;

/*============================================================================*/
/* callback function                                                          */
/*============================================================================*/
typedef void (*ico_syc_callback_t) (const ico_syc_ev_e event,
                                    const void *detail,
                                    void *user_data);
#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_TYPE_H_*/
/* vim:set expandtab ts=4 sw=4: */
