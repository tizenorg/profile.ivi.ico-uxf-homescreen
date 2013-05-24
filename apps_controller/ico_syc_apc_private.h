/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprications Controller internal(private)
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_SYC_APC_PRIVATE_H_
#define _ICO_SYC_APC_PRIVATE_H_

#include    <aul/aul.h>
#include    <ico_input_mgr-client-protocol.h>
#include    "ico_apf_log.h"
#include    "ico_uxf_sysdef.h"
#include    "ico_uxf_conf.h"
#include    "ico_apf_resource_control.h"

#ifdef __cplusplus
extern "C" {
#endif

/* D-Bus target                                 */
#define DBUS_SERVICE   "org.automotive.message.broker"
#define DBUS_INTERFACE "org.freedesktop.DBus.Properties"
#define DBUS_METHOD    "Get"

/* define struct                                */
/* request information                          */
typedef struct  _ico_apc_request    {
    struct _ico_apc_request     *next;          /* requestt list link               */
    char                        appid[ICO_UXF_MAX_PROCESS_NAME+1];
                                                /* application id                   */
    ico_apf_resid_e             resid;          /* resource id                      */
    char                        device[ICO_UXF_MAX_DEVICE_NAME+1];
                                                /* request device                   */
    int                         id;             /* request object                   */
    int                         bid;            /* request base object              */
    int                         pid;            /* request client pid               */
    int                         prio;           /* request priority                 */
    unsigned short              zoneidx;        /* request target zone index        */
    unsigned short              timer;          /* Reply wait timer                 */
    unsigned short              state;          /* status                           */
    unsigned short              reqtype;        /* Request type                     */
} ico_apc_request_t;

#define ICO_APC_REQTYPE_REQUEST 0               /* Request from application         */
#define ICO_APC_REQTYPE_CREATE  1               /* Request automaticaly             */

#define ICO_APC_REQREPLY_MAXTIME     500        /* Maximum reply wait (ms)          */
#define ICO_APC_REQSTATE_WAITREQ     0x01       /* Waitting resource                */
#define ICO_APC_REQSTATE_WAITPROC    0x08       /* Priotiry process work flag       */
#define ICO_APC_REQSTATE_MASK        0x0f       /* State mask                       */
#define ICO_APC_REQSTATE_REPLYACTIVE 0x20       /* Wait reply for active            */
#define ICO_APC_REQSTATE_REPLYQUIET  0x10       /* Wait reply for hide              */

/* application request table                    */
typedef struct  _ico_apc_apprequest {
} ico_apc_apprequest_t;

/* display zone                                 */
typedef struct  _ico_apc_dispzone   {
    ico_apc_request_t   *req;                   /* request list link                */
    Ico_Uxf_conf_display_zone   *conf;          /* configuration                    */
    short               res;                    /* (unused)                         */
    short               noverlap;               /* number of overlap zone           */
                                                /* overlap zone                     */
    struct _ico_apc_dispzone    *overlap[ICO_UXF_DISPLAY_ZONE_MAX];
} ico_apc_dispzone_t;

/* sound zone                                   */
typedef struct  _ico_apc_soundzone  {
    ico_apc_request_t   *req;                   /* request list link                */
    Ico_Uxf_conf_sound_zone     *conf;          /* configuration                    */
    short               res;                    /* (unused)                         */
    short               noverlap;               /* number of overlap zone           */
                                                /* overlap zone                     */
    struct _ico_apc_soundzone   *overlap[ICO_UXF_SOUND_ZONE_MAX];
} ico_apc_soundzone_t;

/* input sw                                     */
typedef struct  _ico_apc_inputsw    {
    ico_apc_request_t   *req;                   /* request list link                */
    Ico_Uxf_InputDev    *inputdev;              /* input device attribute           */
    Ico_Uxf_InputSw     *inputsw;               /* input switch attribute           */
} ico_apc_inputsw_t;

#ifdef __cplusplus
}
#endif
#endif  /* _ICO_SYC_APC_PRIVATE_H_ */

