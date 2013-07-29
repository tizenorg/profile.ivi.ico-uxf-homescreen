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

#ifndef ICO_UXF_CONF_DEF_H_
#define ICO_UXF_CONF_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Environment variable for UI-FW top directory             */
#define ICO_UXF_TOP_ENV         "UIFW_TOPDIR"

/* Default top directory of UI-FW                           */
#define ICO_UXF_TOP_TOPDIR      "/usr/apps/org.tizen.ico.homescreen"

/* Environment variable for top directory of cconfiguration files   */
#define ICO_UXF_CONFIG_ENV      "UIFW_CONFDIR"

/* Default top directory of cconfiguration files            */
#define ICO_UXF_CONFIG_DIR      "res/config"

/* Configuration file names                                 */
#define ICO_UXF_CONFIG_SYSTEM   "system.conf"
#define ICO_UXF_CONFIG_APPATTR  "app_attr.conf"

/* Directory of application manifest                        */
#define ICO_UXF_CONFIG_APPSDIR  "apps"

/* Maximum length of the configuration directory name       */
#define ICO_UXF_CONF_DIR_MAX    63

/* Maximum length of Wayland connection name                */
#define ICO_UXF_CONF_WAYLAND_NAME   39

#ifdef __cplusplus
}
#endif
#endif /* ICO_UXF_CONF_DEF_H_ */

