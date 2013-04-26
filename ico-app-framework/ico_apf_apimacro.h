/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of AppsController interface(API) macros
 *
 * @date    Feb-28-2013
 */

#ifndef __ICO_APF_APIMACRO_H__
#define __ICO_APF_APIMACRO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Macros                                       */
#if defined(__GNUC__) && __GNUC__ >= 4
#define ICO_APF_API __attribute__ ((visibility("default")))
#else
#define ICO_APF_API
#endif

#ifdef  __cplusplus
}
#endif
#endif /*__ICO_APF_APIMACRO_H__*/

