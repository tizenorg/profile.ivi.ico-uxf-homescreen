#ifndef __CICO_RESOURCECONFIG_H__
#define __CICO_RESOURCECONFIG_H__
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

//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

//#include "CicoHomeScreenCommon.h"
//#include "home_screen_res.h"

#define ICO_EDJ_PATH            "/usr/apps/%s/res/edj/"
#define ICO_IMAGE_PATH          "/usr/apps/%s/res/images/"
#define ICO_SOUND_PATH          "/usr/apps/%s/res/sounds/"
#define ICO_MANIFEST_PATH       "/usr/apps/%s/res/config/"
#define ICO_LOCAL_TOP_PATH      "/home/tizen/HomeScreen"
#define ICO_LOCAL_EDJ_PATH      "%s/src/"
#define ICO_LOCAL_IMAGE_PATH    "%s/res/images/"
#define ICO_LOCAL_SOUND_PATH    "%s/res/sounds/"
#define ICO_LOCAL_MANIFEST_PATH "%s/res/apps/org.tizen.ico.homescreen/"

/* Environment variable for UI-FW top directory             */
#define ICO_HS_CONFIG_TOP_ENV         "UIFW_TOPDIR"
#if 0
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
#endif

/*============================================================================*/
/* Class Declaration (CicoResourceConfig)                           */
/*============================================================================*/
class CicoResourceConfig
{
  public:
    static void GetImagePath(char *buff, int len);
    static void GetSoundPath(char *buff, int len);
    static void GetEdjPath(char *buff, int len);
    static void GetConfPath(char *buff, int len);
  protected:
    CicoResourceConfig operator =
        (const CicoResourceConfig&);
    CicoResourceConfig(const CicoResourceConfig&);
};
#endif  // __CICO_RESOURCECONFIG_H__
// vim: set expandtab ts=4 sw=4:
