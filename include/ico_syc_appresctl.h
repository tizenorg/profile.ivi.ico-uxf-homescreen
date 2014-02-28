/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Resource Control API
 *
 * @date    July-31-2013
 */

#ifndef _ICO_SYC_APPRESCTL_H_
#define _ICO_SYC_APPRESCTL_H_

#include "ico_syc_private.h"
#include "ico_syc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
struct ico_syc_res_context;

/*============================================================================*/
/* enum                                                                       */
/*============================================================================*/
/*
 * type of resource
 * @ICO_SYC_RES_BASIC: basic resource
 * @ICO_SYC_RES_INTERRUPT: interruption resource
 */
typedef enum _resource_type {
    ICO_SYC_RES_BASIC       = 0,
    ICO_SYC_RES_INTERRUPT   = 1,
    ICO_SYC_RES_ONSCREEN    = 2
} ico_syc_res_type_e;

/*
 * type of sound adjust
 * @ICO_SYC_SOUND_MUTE: mute the basic sound
 *                      while playing the interruption sound
 * @ICO_SYC_SOUND_WAITING: wait the basic sound
 *                         until the interruption sound end
 * @ICO_SYC_SOUND_TOGETHER: play the basic sound and the interruption sound
 *                          at the same time
 * @ICO_SYC_SOUND_VOL_DOWN: turn the volume down of the basic sound
 *                          while playing the interruption sound
 */
typedef enum _ {
    ICO_SYC_SOUND_MUTE      = 0,
    ICO_SYC_SOUND_WAITING   = 1,
    ICO_SYC_SOUND_TOGETHER  = 2,
    ICO_SYC_SOUND_VOL_DOWN  = 3
} ico_syc_sound_adjust_e;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_acquire_res
 *          Acquire the resources(window/sound/input).
 *          User can choose window and sound resource's type
 *          "basic" or "interruption".
 *          If user want to acquire the sound resource only,
 *          user sets NULL to argument "window" and "input".
 *
 * @param[in]   window                  window resource's information
 * @param[in]   sound                   sound resource's information
 * @param[in]   input                   input resource's information
 * @param[in]   type                    window and sound resource's type
 *                                      (basic or interruption)
 * @return      resource's context address
 * @retval      address                 success
 * @retval      NULL                    error
 * @see         ico_syc_res_type_e
 */
/*--------------------------------------------------------------------------*/
struct ico_syc_res_context * ico_syc_acquire_res(
                             const ico_syc_res_window_t *window,
                             const ico_syc_res_sound_t *sound,
                             const ico_syc_res_input_t *input,
                             int type);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_release_res
 *          Release the resources(window/sound/input).
 *
 * @param[in]   context                 resource context
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_release_res(struct ico_syc_res_context *context);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_set_input_region
 *          Set the input region. (for haptic device)
 *          Callback function notifies the result of setting the input region.
 *
 * @param[in]   input                   input region's information
 * @param[in]   attr                    input region's attribute
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_set_input_region(const ico_syc_input_region_t *input, int attr);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_unset_input_region
 *          Unset the input region. (for haptic device)
 *          Callback function notifies the result of unsetting the input region.
 *
 * @param[in]   input                   input region's information
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_unset_input_region(const ico_syc_input_region_t *input);


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_APPRESCTL_H_*/
/* vim:set expandtab ts=4 sw=4: */
