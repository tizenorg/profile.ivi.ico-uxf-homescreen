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

#define ICO_SYC_EOK          0  /* OK                       */
#define ICO_SYC_EPERM       -1  /* Operation not permitted  */
#define ICO_SYC_ENOENT      -2  /* No such object           */
#define ICO_SYC_ESRCH       -3  /* Not initialized          */
#define ICO_SYC_EIO         -5  /* I/O(send/receive) error  */
#define ICO_SYC_ENXIO       -6  /* Not exist                */
#define ICO_SYC_E2BIG       -7  /* Buffer size too smale    */
#define ICO_SYC_EBADF       -9  /* Illegal data type        */
#define ICO_SYC_EAGAIN      -11 /* Try again                */
#define ICO_SYC_ENOMEM      -12 /* Out of memory            */
#define ICO_SYC_EFAULT      -14 /* Bad address              */
#define ICO_SYC_EBUSY       -16 /* Not available now        */
#define ICO_SYC_EEXIST      -17 /* Multiple define          */
#define ICO_SYC_EINVAL      -22 /* Invalid argument         */
#define ICO_SYC_ENOSYS      -38 /* System error             */

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_ERROR_H_*/
/* vim:set expandtab ts=4 sw=4: */
