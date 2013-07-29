/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   load configuratoin file
 *
 * @date    Feb-15-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <glib.h>

#include "ico_uxf_conf.h"
#include "home_screen_conf.h"
#include "home_screen.h"

/*============================================================================*/
/* variables and tables                                                       */
/*============================================================================*/
static GKeyFile *hs_config_key = NULL;
gboolean hs_is_open_config = FALSE;

/*============================================================================*/
/* functions                                                                  */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @brief   initHomeScreenConfig
 *          read and initialize for homescreen configuration
 *
 * @param[in]   conf                configuration file name
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
/*--------------------------------------------------------------------------*/
int
initHomeScreenConfig(const char *conf)
{
    GError *error = NULL;
    char path[ICO_HS_TEMP_BUF_SIZE];
    GKeyFile *homescreen;
    GString *filepath;

    uifw_trace("initHomeScreenConfig: open=%d", hs_is_open_config);
    if (hs_is_open_config == TRUE) {
        return ICO_HS_ERR;
    }

    homescreen = g_key_file_new();
    hs_config_key = homescreen;
    filepath = g_string_new("test");
    hs_get_conf_path(path, sizeof(path));
    g_string_printf(filepath, "%s/%s", path, conf);
    if (!g_file_test(filepath->str, G_FILE_TEST_IS_REGULAR)) {
        uifw_warn("%s: config file dose not exist", filepath->str);
        return ICO_HS_ERR;
    }

    g_key_file_load_from_file(homescreen, filepath->str,
                              G_KEY_FILE_KEEP_COMMENTS
                                      | G_KEY_FILE_KEEP_TRANSLATIONS, &error);
    if (error != NULL) {
        uifw_warn("%s: %s", filepath->str, error->message);
        g_error_free(error);
    }
    uifw_trace("config file(%s)", filepath->str);

    g_string_free(filepath, TRUE);
    if (error != NULL) {
        uifw_warn("%s: %s", filepath->str, error->message);
        g_error_free(error);
        hs_is_open_config = TRUE;
        return ICO_HS_ERR;
    }
    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_conf_get_integer
 *          Return integer value in homescreen configuration file.
 *          If not exist,return defaultvlaue in params.
 *
 * @param[in]   group_name          configuration group name
 * @param[in]   key                 configuration name
 * @param[in]   default_value       default value
 * @return      config integer
 */
/*--------------------------------------------------------------------------*/
int
hs_conf_get_integer(const char *group_name, const char *key, int default_value)
{
    GError *error = NULL;

    if (hs_config_key == NULL) {
        if (initHomeScreenConfig(ICO_HOMESCREEN_CONFIG_FILE) != ICO_HS_OK) {
            return default_value;
        }
    }

    int value = g_key_file_get_integer(hs_config_key, group_name, key, &error);
    if (error != NULL) {
        uifw_warn("%s:%s %s", group_name, key, error->message);
        g_error_free(error);
        return default_value;
    }
    return value;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   hs_conf_get_string
 *          Return string value in homescreen configuration file.
 *          If not exist,return defaultvlaue in params.
 *
 * @param[in]   group_name          configuration group name
 * @param[in]   key                 configuration name
 * @param[in]   default_value       default value
 * @return      config string
 */
/*--------------------------------------------------------------------------*/
const char *
hs_conf_get_string(const char *group_name, const char *key,
                   const char *default_value)
{
    GError *error = NULL;

    if (hs_config_key == NULL) {
        if (initHomeScreenConfig(ICO_HOMESCREEN_CONFIG_FILE) != ICO_HS_OK) {
            return default_value;
        }
    }

    const char *value = g_key_file_get_string(hs_config_key, group_name, key,
                                              &error);
    if (error != NULL) {
        uifw_warn("%s:%s %s", group_name, key, error->message);
        g_error_free(error);
        return default_value;
    }
    return value;
}

