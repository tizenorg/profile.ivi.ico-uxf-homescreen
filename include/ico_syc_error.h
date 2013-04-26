/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   System Controller public header for permanent value
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_SYC_DEF_H_
#define _ICO_SYC_DEF_H_

#include "ico_apf_error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* common API error code        */
#define ICO_SYC_EOK     ICO_APF_RESOURCE_E_NONE             /* OK                   */
#define ICO_SYC_ENOENT  ICO_APF_RESOURCE_E_UNKOWN           /* No such object       */
#define ICO_SYC_ESRCH   ICO_APF_RESOURCE_E_NOT_INITIALIZED  /* Not initialized      */
#define ICO_SYC_EIO     ICO_APF_RESOURCE_E_COMMUNICATION    /* I/O error            */
#define ICO_SYC_ENOMEM  ICO_APF_RESOURCE_E_NOT_IMPLEMENTED  /* Out of memory        */
#define ICO_SYC_EINVAL  ICO_APF_RESOURCE_E_INVAL            /* Invalid argument     */
#define ICO_SYC_ENOSYS  (-99)                               /* System error         */

/* Vehicle information internal key */
enum    _ico_syc_regulation_vehicleinfo_id  {
    ICO_SYC_VEHICLEINFO_VEHICLE_SPEED = 1,  /* vehicle speed                        */
    ICO_SYC_VEHICLEINFO_SHIFT_POSITION,     /* shift position                       */
    ICO_SYC_VEHICLEINFO_BLINKER,            /* blinker(not inpliment in AMB)        */

    ICO_SYC_VEHICLEINFO_MAX                 /* maximum number of type               */
};

#ifdef __cplusplus
}
#endif
#endif /* _ICO_SYC_DEF_H_ */

