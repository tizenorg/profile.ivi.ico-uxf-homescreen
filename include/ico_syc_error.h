/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller Error definition
 *          for privilege and general applications
 *
 * @date    July-31-2013
 */

#ifndef _ICO_SYC_ERROR_H_
#define _ICO_SYC_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
/*
 * id of error
 */
typedef enum _ico_syc_error {
    ICO_SYC_ERR_NONE            = 0,    /* success */
    ICO_SYC_ERR_CONNECT         = -100, /* fail to connect */
    ICO_SYC_ERR_CLOSED          = -101, /* closed */
    ICO_SYC_ERR_INVALID_PARAM   = -102, /* invalid parameter */
    ICO_SYC_ERR_OUT_OF_MEMORY   = -103, /* out of memory */
    ICO_SYC_ERR_SYSTEM          = -104,
    ICO_SYC_ERR_UNKNOWN         = -200  /* unknown error */
} ico_syc_err_e;


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_ERROR_H_*/
/* vim:set expandtab ts=4 sw=4: */
