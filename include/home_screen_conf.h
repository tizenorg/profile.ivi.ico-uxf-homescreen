/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of homescreen configuration
 *
 * @date    Feb-15-2013
 */

#ifndef _HOME_SCREEN_CONF_H_
#define _HOME_SCREEN_CONF_H_

#include <glib.h>
#include "ico_uxf_sysdef.h"
#include "ico_uxf_def.h"
#include "ico_apf_log.h"

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
#define ICO_HOMESCREEN_CONFIG_FILE "homescreen.conf"
#define ICO_ONSCREEN_CONFIG_FILE   "onscreen.conf"
#define ICO_STATUSBAR_CONFIG_FILE  "statusbar.conf"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
extern int initHomeScreenConfig(const char *conf);
extern int hs_conf_get_integer(const char *group_name, 
                     const char *key, int default_value);
extern const char *hs_conf_get_string(const char *group_name, 
                        const char *key, const char *default_value);

#endif /* _HOME_SCREEN_CONF_H_ */

