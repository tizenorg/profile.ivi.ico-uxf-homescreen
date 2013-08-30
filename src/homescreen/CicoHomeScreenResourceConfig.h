#ifndef __CICOHOMESCREENRESOURCECONFIG_H__
#define __CICOHOMESCREENRESOURCECONFIG_H__
/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   configuration of resource path
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CicoHomeScreenCommon.h"
#include "home_screen_res.h"

/* Environment variable for UI-FW top directory             */
#define ICO_HS_CONFIG_TOP_ENV         "UIFW_TOPDIR"
/* Default top directory of UI-FW                           */
#define ICO_HS_CONFIG_TOP_TOPDIR      "/usr/apps/org.tizen.ico.homescreen"
/* Environment variable for top directory of cconfiguration files   */
#define ICO_HS_CONFIG_CONFIG_ENV      "UIFW_CONFDIR"
/* Default top directory of cconfiguration files            */
#define ICO_HS_CONFIG_CONFIG_DIR      "res/config"
/* Configuration file names                                 */
#define ICO_HS_CONFIG_CONFIG_SYSTEM   "system.conf"
#define ICO_HS_CONFIG_CONFIG_APPATTR  "app_attr.conf"
/* Directory of application manifest                        */
#define ICO_HS_CONFIG_CONFIG_APPSDIR  "apps"
/* Maximum length of the configuration directory name       */
#define ICO_HS_CONFIG_CONF_DIR_MAX    63
/* Maximum length of Wayland connection name                */
#define ICO_UXF_CONF_WAYLAND_NAME   39

/*============================================================================*/
/* Class Declaration (CicoHomeScreenResourceConfig)                           */
/*============================================================================*/
class CicoHomeScreenResourceConfig
{
  public:
    static void GetImagePath(char *buff, int len);
    static void GetSoundPath(char *buff, int len);
    static void GetEdjPath(char *buff, int len);
    static void GetOnscreenEdjPath(char *buff, int len);
    static void GetConfPath(char *buff, int len);
  protected:
    CicoHomeScreenResourceConfig operator =
        (const CicoHomeScreenResourceConfig&);
    CicoHomeScreenResourceConfig(const CicoHomeScreenResourceConfig&);
};

#endif

