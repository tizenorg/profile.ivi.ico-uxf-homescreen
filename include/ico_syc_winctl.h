/*
 * Copyright (c) 2013-2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Window Control API
 *          for privilege applications
 *
 * @date    Feb-21-2014
 */

#ifndef _ICO_SYC_WINCTL_H_
#define _ICO_SYC_WINCTL_H_

#include <ico_window_mgr-client-protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* enum                                                                       */
/*============================================================================*/
/*
 * showing window
 * @ICO_SYC_WIN_SHOW_ALL: show all of the application windows
 */
typedef enum _window_show {
    ICO_SYC_WIN_SHOW_ALL = -1
} ico_syc_win_show_e;

/*============================================================================*/
/* structure                                                                  */
/*============================================================================*/
/*
 * animation information
 */
#define ICO_SYC_WIN_SURF_RAISE      0x01000000
#define ICO_SYC_WIN_SURF_LOWER      0x02000000
#define ICO_SYC_WIN_SURF_NOCHANGE   0x04000000
#define ICO_SYC_WIN_SURF_NORESCTL   0x40000000
#define ICO_SYC_WIN_SURF_FLAGS      0xff000000
typedef struct _animation_info {
    char *name;     /* name of animation (ex. fade,slide,zoom etc..) */
    int  time;      /* millisecond time to show the transition's animation */
} ico_syc_animation_t;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_show
 *          Show the application window with animation.
 *          If user sets argument surface "ICO_SYC_WIN_SHOW_ALL",
 *          show all of the application windows.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 * @see         ico_syc_win_show_e
 */
/*--------------------------------------------------------------------------*/
int ico_syc_show(const char *appid, int surface,
                 const ico_syc_animation_t *animation);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_hide
 *          Hide the application window with animation.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_hide(const char *appid, int surface,
                 const ico_syc_animation_t *animation);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_move
 *          Move the application window with animation.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   move                    move information (zone/position/size)
 * @param[in]   animation               animation information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_move(const char *appid, int surface,
                 const ico_syc_win_move_t *move,
                 const ico_syc_animation_t *animation);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_active
 *          Change the active window which receives the input-event notification
 *          from System Controller.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_change_active(const char *appid, int surface);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_change_layer
 *          Change the window's layer.
 *
 * @param[in]   appid                   application id
 * @param[in]   surface                 window's surface id
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_change_layer(const char *appid, int surface, int layer);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_map_get
 *          Get surface image pixel to the file.
 *
 * @param[in]   surface                 window's surface id
 * @param[in]   filepath                pixel image file path
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_map_get(int surface, const char *filepath);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_map_thumb
 *          Map the thumbnail data
 *
 * @param[in]   surface                 window's surface id
 * @param[in]   framerate               notify cycle [frames par sec]
 * @param[in]   filepath                pixel image file path
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_map_thumb(int surface, int framerate, const char *filepath);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_unmap_thumb
 *          Unmap the thumbnail data.
 *          User calls this API when receiving the notification that
 *          terminated the application.
 *
 * @param[in]   surface                 window's surface id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_unmap_thumb(int surface);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_show_layer
 *          Show the layer.
 *
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_show_layer(int layer);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_hide_layer
 *          Hide the layer.
 *
 * @param[in]   layer                   window's layer id
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_hide_layer(int layer);


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_WINCTL_H_*/
/* vim:set expandtab ts=4 sw=4: */
