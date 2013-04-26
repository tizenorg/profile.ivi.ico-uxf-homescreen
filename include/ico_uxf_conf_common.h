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

#ifndef ICO_UXF_CONF_COMMON_H_
#define ICO_UXF_CONF_COMMON_H_

#include <glib.h>

#include "ico_uxf_sysdef.h"
#include "ico_uxf_def.h"
#include "ico_uxf_conf.h"
#include "ico_apf_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Prototyypes of configuration functions           */
int ico_uxf_conf_getUint(const char* str);
GList* ico_uxf_conf_countNumericalKey(GKeyFile *keyfile, const char* group);
const char* ico_uxf_conf_appendStr(const char* str1, const char* str2);
void ico_uxf_conf_checkGerror(GError** error);

#ifdef __cplusplus
}
#endif
#endif /* ICO_UXF_CONF_COMMON_H_ */

