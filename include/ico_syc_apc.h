/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of Apprications Controller
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_SYC_APC_H_
#define _ICO_SYC_APC_H_

#include "ico_apf_log.h"
#include "ico_uxf_sysdef.h"
#include "ico_uxf_conf.h"
#include "ico_uxf_def.h"
#include "ico_syc_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum values                           */
#define ICO_SYC_APC_REGULATION_LISTENERS    4   /* number of regulation listeners   */

/* Vehicle information values               */
#define ICO_SYC_APC_REGULATION_SPEED_RUNNING    (4.0)
#define ICO_SYC_APC_REGULATION_SHIFT_NEUTRALS   (0)
#define ICO_SYC_APC_REGULATION_SHIFT_1ST        (1)
#define ICO_SYC_APC_REGULATION_SHIFT_2ND        (2)
#define ICO_SYC_APC_REGULATION_SHIFT_3RD        (3)
#define ICO_SYC_APC_REGULATION_SHIFT_DRIVE      (4)
#define ICO_SYC_APC_REGULATION_SHIFT_REVERSES   (128)
#define ICO_SYC_APC_REGULATION_SHIFT_PARKING    (255)
#define ICO_SYC_APC_REGULATION_BLINKER_NONE     (0)
#define ICO_SYC_APC_REGULATION_BLINKER_RIGHT    (1)
#define ICO_SYC_APC_REGULATION_BLINKER_LEFT     (2)

#define ICO_SYC_APC_REGULATION_REGULATION       (0)
#define ICO_SYC_APC_REGULATION_NOREGULATION     (1)
#define ICO_SYC_APC_REGULATION_NOCHANGE         (9)

typedef struct _ico_apc_reguration_control  {
    unsigned char   display;                /* display(1=visible/0=invisible)   */
    unsigned char   sound;                  /* sound(1=play/0=mute)             */
    unsigned char   input;                  /* inputsw(1=use/0=unuse)           */
    unsigned char   res;                    /* (unused)                         */
} ico_apc_reguration_control_t;
                                            /* application context (pointer only)*/
typedef struct _ico_apc_context *ico_apc_context_h;

                                            /* regulation event callback function*/
typedef void (*ico_apc_regulation_cb_t)(const int appcategory,
                                        const ico_apc_reguration_control_t control,
                                        void *user_data);
                                            /* respurce control                 */
typedef void (*ico_apc_resource_control_t)(const Ico_Uxf_conf_application *conf,
                                           const int control);

/* API prototype                            */
/* Apps Controller initialize/terminate     */
                                            /* initialize Apps Controller       */
int ico_syc_apc_init(ico_apc_resource_control_t display,
                     ico_apc_resource_control_t sound,
                     ico_apc_resource_control_t input);
                                            /* Terminate Apps Controller        */
void ico_syc_apc_term(void);
                                            /* Select active application        */
void ico_syc_apc_active(const char *appid);

/* Apps Controller regulation control           */
                                            /* regulation controller initialize     */
int ico_syc_apc_regulation_init(void);
                                            /* regulation controller terminate  */
int ico_syc_apc_regulation_term(void);
                                            /* set regulation change listener       */
void ico_syc_apc_regulation_listener(ico_apc_regulation_cb_t func, void *user_data);
                                            /* get application visible control  */
int ico_syc_apc_regulation_app_visible(const int category);
                                            /* get application sound control    */
int ico_syc_apc_regulation_app_sound(const int category);
                                            /* get application input control    */
int ico_syc_apc_regulation_app_input(const int category);

#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_APC_H_*/

