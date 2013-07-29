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

#include "ico_uxf_conf_def.h"
#include "home_screen.h"
#include "home_screen_res.h"

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_image_path
 *          set the directory path of the image files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_get_image_path(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, image_path, pkg);
    }
    else {
        pkg = getenv(ICO_UXF_TOP_ENV);
        snprintf(buff, len, local_image_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_sound_path
 *          set the directory path of the sounds files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_get_sound_path(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, sound_path, pkg);
    }
    else {
        pkg = getenv(ICO_UXF_TOP_ENV);
        snprintf(buff, len, local_sound_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_edj_path
 *          set the directory path of efj files.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_get_edj_path(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, edj_path, pkg);
    }
    else {
        pkg = getenv(ICO_UXF_TOP_ENV);
        snprintf(buff, len, local_edj_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_ons_edj_path
 *          set the directory path of edj files for onscreen.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_get_ons_edj_path(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        snprintf(buff, len, edj_path, ICO_HS_APPID_DEFAULT_ONS);
    }
    else {
        pkg = getenv(ICO_UXF_TOP_ENV);
        snprintf(buff, len, local_edj_path, pkg ? pkg : local_top_path);
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_get_conf_path
 *          set the directory path of edj files for onscreen.
 *
 * @param[out]  buff                buffer that the pass set
 * @param[in]   len                 buffer length
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
hs_get_conf_path(char *buff, int len)
{
    char *pkg;

    memset(buff, 0, len);
    pkg = getenv("PKG_NAME");
    if (pkg) {
        uifw_trace("hs_get_conf_path: PKG_NAME=%s", pkg);
        snprintf(buff, len, manifest_path, pkg);
    }
    else {
        pkg = getenv(ICO_UXF_TOP_ENV);
        snprintf(buff, len, local_manifest_path, pkg ? pkg : local_top_path);
        uifw_warn("hs_get_conf_path: no PKG_NAME, path=%s", buff);
    }
}
