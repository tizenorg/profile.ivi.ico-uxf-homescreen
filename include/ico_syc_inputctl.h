/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Input Control API
 *          for privilege applications
 *
 * @date    Feb-21-2014
 */

#ifndef _ICO_SYC_INPUTCTL_H_
#define _ICO_SYC_INPUTCTL_H_

#include <ico-uxf-weston-plugin/ico_input_mgr-client-protocol.h>

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* enum                                                                       */
/*============================================================================*/
/*
 * type of input event's allocation
 * @ICO_SYC_INPUT_ALLOC_FIX: fixed allocation
 * @ICO_SYC_INPUT_ALLOC_VARIABLE: variable allocation
 */
typedef enum _input_alloc {
    ICO_SYC_INPUT_ALLOC_FIX         = 1,
    ICO_SYC_INPUT_ALLOC_VARIABLE    = 0
} ico_syc_input_alloc_e;

/*
 * type of input event
 * @ICO_SYC_INPUT_TYPE_POINTER: mouse event
 * @ICO_SYC_INPUT_TYPE_KEYBOARD: keyboard event
 * @ICO_SYC_INPUT_TYPE_TOUCH: touch panel event
 * @ICO_SYC_INPUT_TYPE_SWITCH: switch event
 */
typedef enum _input_type {
    ICO_SYC_INPUT_TYPE_POINTER  = ICO_INPUT_MGR_DEVICE_TYPE_POINTER,
    ICO_SYC_INPUT_TYPE_KEYBOARD = ICO_INPUT_MGR_DEVICE_TYPE_KEYBOARD,
    ICO_SYC_INPUT_TYPE_TOUCH    = ICO_INPUT_MGR_DEVICE_TYPE_TOUCH,
    ICO_SYC_INPUT_TYPE_SWITCH   = ICO_INPUT_MGR_DEVICE_TYPE_SWITCH
} ico_syc_input_type_e;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_add_input
 *          Add the input event to the application.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @param[in]   fix                     fixed allocation or variable allocation
 * @param[in]   keycode                 keycode
 *                                      (If set "0", not allocate to keycode)
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 * @see         ico_syc_input_alloc_e
 */
/*--------------------------------------------------------------------------*/
int ico_syc_add_input(const char *appid, const char *device,
                      int input, int fix, int keycode);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_delete_input
 *          Delete the input event assigned to the application.
 *
 * @param[in]   appid                   application id
 * @param[in]   device                  input device
 * @param[in]   input                   input number
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_delete_input(const char *appid, const char *device, int input);

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_INPUTCTL_H_*/
/* vim:set expandtab ts=4 sw=4: */