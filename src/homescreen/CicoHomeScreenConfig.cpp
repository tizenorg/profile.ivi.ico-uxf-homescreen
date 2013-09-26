/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */

//==========================================================================
/**
 *  @file   CicoHomeScreenConfig
 *
 *  @brief  This file is implementation of CicoHomeScreenConfig class
 */
//==========================================================================

#include "CicoHomeScreenConfig.h"
#include "CicoHomeScreenResourceConfig.h"

//==========================================================================
//  public functions
//==========================================================================
//--------------------------------------------------------------------------
/**
 * @brief   CicoHomeScreenConfig::CicoHomeScreenConfig
 *          Constractor
 */
//--------------------------------------------------------------------------
CicoHomeScreenConfig::CicoHomeScreenConfig()
    : m_gKeyFile(NULL)
{
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoHomeScreenConfig::CicoHomeScreenConfig
 *          destructor
 */
//--------------------------------------------------------------------------
CicoHomeScreenConfig::~CicoHomeScreenConfig()
{
    if (NULL != m_gKeyFile) {
        g_key_file_free(m_gKeyFile);
    }
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoHomeScreenConfig::Initialize
 *          read and initialize for homescreen configuration
 *
 * @param[in]   conf                configuration file name
 * @return      result
 * @retval      ICO_HS_OK           success
 * @retval      ICO_HS_ERR          error
 */
//--------------------------------------------------------------------------
int
CicoHomeScreenConfig::Initialize(const char *conf)
{
    GError *error = NULL;
    char path[ICO_HS_TEMP_BUF_SIZE];
    GString *filepath;

    if (NULL != m_gKeyFile) {
        /*if initialize was done*/
        return ICO_HS_OK;
    }

    m_gKeyFile = g_key_file_new();
    
    filepath = g_string_new("test");
    CicoHomeScreenResourceConfig::GetConfPath(path, sizeof(path));
    g_string_printf(filepath, "%s/%s", path, conf);
    if (!g_file_test(filepath->str, G_FILE_TEST_IS_REGULAR)) {
        return ICO_HS_ERR;
    }

    // load config file
    g_key_file_load_from_file(m_gKeyFile, filepath->str,
                              (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS |
                                              G_KEY_FILE_KEEP_TRANSLATIONS),
                              &error);
    if (error != NULL) {
        ICO_WRN("%s: %s", filepath->str, error->message);
        g_error_free(error);
        return ICO_HS_ERR;
    }

    g_string_free(filepath, TRUE);

    return ICO_HS_OK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenConfig::ConfigGetInteger
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
CicoHomeScreenConfig::ConfigGetInteger(const char *group_name, const char *key, int default_value)
{
    GError *error = NULL;

    if (m_gKeyFile == NULL) {
        /* if initialize is didn't yet */
        if (Initialize(ICO_HOMESCREEN_CONFIG_FILE) != ICO_HS_OK) {
            return default_value;
        }
    }

    int value = g_key_file_get_integer(m_gKeyFile, group_name, key, &error);
    if (error != NULL) {
        ICO_WRN("%s", error->message);
        g_error_free(error);
        return default_value;
    }
    return value;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoHomeScreenConfig::ConfigGetString
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
CicoHomeScreenConfig::ConfigGetString(const char *group_name, const char *key,
                   const char *default_value)
{
    GError *error = NULL;

     if (m_gKeyFile == NULL) {
        /* if initialize is didn't yet */
        if (Initialize(ICO_HOMESCREEN_CONFIG_FILE) != ICO_HS_OK) {
            return default_value;
        }
    }

    const char *value = g_key_file_get_string(m_gKeyFile, group_name, key,
                                              &error);
    if (error != NULL) {
        ICO_WRN("%s", error->message);
        g_error_free(error);
        return default_value;
    }
    return value;
}
// vim:set expandtab ts=4 sw=4:
