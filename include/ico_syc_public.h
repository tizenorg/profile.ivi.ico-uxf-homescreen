/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller internal definition
 *
 * @date    Sep-12-2013
 */

#ifndef _ICO_SYC_PUBLIC_H_
#define _ICO_SYC_PUBLIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
/* login user name bundle key name */
#define ICO_SYC_APP_BUNDLE_KEY1 "HS_PARAM_U"

/* Individual history file of the logged-in user. parameter key */
#define ICO_SYC_APP_BUNDLE_KEY2 "HS_PARAM_D"

/* History file of common. parameter key */
#define ICO_SYC_APP_BUNDLE_KEY3 "HS_PARAM_DD"

/* change user status flag file. parameter key */
#define ICO_SYC_APP_BUNDLE_KEY4 "HS_PARAM_FLG"

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_PUBLIC_H_*/
