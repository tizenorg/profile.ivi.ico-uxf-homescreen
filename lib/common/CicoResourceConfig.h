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

#include "ico_syc_private.h"

#define ICO_EDJ_PATH            "%s/%s/res/edj/"
#define ICO_IMAGE_PATH          "%s/%s/res/images/"
#define ICO_SOUND_PATH          "%s/%s/res/sounds/"
#define ICO_MANIFEST_PATH       "%s/%s/res/config/"
#define ICO_LOCAL_TOP_PATH      "/home/tizen/HomeScreen"
#define ICO_LOCAL_EDJ_PATH      "%s/src/"
#define ICO_LOCAL_IMAGE_PATH    "%s/res/images/"
#define ICO_LOCAL_SOUND_PATH    "%s/res/sounds/"
#define ICO_LOCAL_MANIFEST_PATH "%s/res/apps/" ICO_SYC_PACKAGE_HOMESCREEN "/"

/* Environment variable for UI-FW top directory             */
#define ICO_HS_CONFIG_TOP_ENV         "UIFW_TOPDIR"

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
