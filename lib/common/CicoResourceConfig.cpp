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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <ico_log.h>
#include "CicoResourceConfig.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoResourceConfig::GetImagePath
 *          set the directory path of the image files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoResourceConfig::GetImagePath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, ICO_IMAGE_PATH, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, ICO_LOCAL_IMAGE_PATH,
                 pkg ? pkg : ICO_LOCAL_TOP_PATH);
    }
    ICO_DBG("buff=\"%s\"", buff);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoResourceConfig::GetSoundPath
 *          set the directory path of the sounds files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoResourceConfig::GetSoundPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, ICO_SOUND_PATH, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, ICO_LOCAL_SOUND_PATH,
                 pkg ? pkg : ICO_LOCAL_TOP_PATH);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoResourceConfig::GetEdjPath
 *          set the directory path of efj files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoResourceConfig::GetEdjPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, ICO_EDJ_PATH, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, ICO_LOCAL_EDJ_PATH,
                 pkg ? pkg : ICO_LOCAL_TOP_PATH);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoResourceConfig::GetConfPath
 *          set the directory path of edj files for onscreen.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoResourceConfig::GetConfPath(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, ICO_MANIFEST_PATH, pkg);
    }
    else {
        pkg = getenv(ICO_HS_CONFIG_TOP_ENV);
        snprintf(buff, len, ICO_LOCAL_MANIFEST_PATH,
                 pkg ? pkg : ICO_LOCAL_TOP_PATH);
    }
}
// vim: set expandtab ts=4 sw=4:
