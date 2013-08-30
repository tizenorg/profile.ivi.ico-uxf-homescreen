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

#include "CicoHomeScreenResourceConfig.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenResourceConfig::GetImagePath
 *          set the directory path of the image files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenResourceConfig::GetImagePath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, image_path, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, local_image_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenResourceConfig::GetSoundPath
 *          set the directory path of the sounds files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenResourceConfig::GetSoundPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, sound_path, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, local_sound_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenResourceConfig::GetEdjPath
 *          set the directory path of efj files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenResourceConfig::GetEdjPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, edj_path, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, local_edj_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenResourceConfig::GetOnscreenEdjPath
 *          set the directory path of edj files for onscreen.
 *void
hs_get_image_path(char *buff, int len)
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenResourceConfig::GetOnscreenEdjPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, edj_path, ICO_HS_APPID_DEFAULT_ONS);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, local_edj_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenResourceConfig::GetConfPath
 *          set the directory path of edj files for onscreen.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoHomeScreenResourceConfig::GetConfPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, manifest_path, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, local_manifest_path, pkg ? pkg : local_top_path);
    }
}
