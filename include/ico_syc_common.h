/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller Common API
 *          for privilege and general applications
 *
 * @date    July-31-2013
 */

#ifndef _ICO_SYC_COMMON_H_
#define _ICO_SYC_COMMON_H_

#include "ico_syc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_connect
 *          Connect to System Controller,
 *          and set the listener to notify the event of System Controller.
 *          If user does not want to set callback function,
 *          set NULL to argument "callback".
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @return      result
 * @retval      0                       success
 * @retval      not 0                   error
 */
/*--------------------------------------------------------------------------*/
int ico_syc_connect(ico_syc_callback_t callback, void *user_data);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_disconnect
 *          Disconnect the link from System Controller,
 *          and unset the listener to notify the event of System Controller.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_disconnect(void);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_isconnect
 *          Check connect of System Controller.
 *
 * @param       none
 * @return      result
 * @retval      1                       connected to System Controller
 * @retval      0                       not connect
 */
/*--------------------------------------------------------------------------*/
int ico_syc_isconnect(void);

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_syc_service
 *          Service for communication to System Controller.
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_service(void);

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_COMMON_H_*/
/* vim:set expandtab ts=4 sw=4: */
