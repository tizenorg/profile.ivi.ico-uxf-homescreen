/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library public header file(configuration)
 *
 * @date    Feb-28-2013
 */

#ifndef _ICO_UXF_CONF_ECORE_H_
#define _ICO_UXF_CONF_ECORE_H_

#include <glib.h>
#include "ico_uxf_conf_def.h"
#include "ico_uxf_sysdef.h"
#include "ico_uxf_def.h"
#include "ico_uxf_typedef.h"
#include "ico_apf_log.h"
#include "ico_uxf_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

int ico_uxf_econf_setAppUpdateCb(Ico_Uxf_AppUpdata_Cb func);

/* define log macros    */
#ifndef uifw_trace
#define uifw_debug      ICO_UXF_DEBUG
#define uifw_trace      ICO_UXF_DEBUG
#define uifw_critical   ICO_UXF_CRITICAL
#define uifw_info       ICO_UXF_INFO
#define uifw_warn       ICO_UXF_WARN
#define uifw_error      ICO_UXF_ERROR
#define uifw_logflush   ico_uxf_log_flush
#endif  /*uifw_trace*/

#ifdef __cplusplus
}
#endif
#endif /* _ICO_UXF_CONF_ECORE_H_ */

