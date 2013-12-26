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
 *  @file   CicoGKeyFileConfig
 *
 *  @brief  This file is implementation of CicoGKeyFileConfig class
 */
//==========================================================================

#include <ico_log.h>
#include "CicoGKeyFileConfig.h"
#include "CicoResourceConfig.h"

//==========================================================================
//  public functions
//==========================================================================
//--------------------------------------------------------------------------
/**
 * @brief   CicoGKeyFileConfig::CicoGKeyFileConfig
 *          Constractor
 */
//--------------------------------------------------------------------------
CicoGKeyFileConfig::CicoGKeyFileConfig()
    : m_gKeyFile(NULL)
{
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoGKeyFileConfig::CicoGKeyFileConfig
 *          destructor
 */
//--------------------------------------------------------------------------
CicoGKeyFileConfig::~CicoGKeyFileConfig()
{
    if (NULL != m_gKeyFile) {
        g_key_file_free(m_gKeyFile);
    }
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoGKeyFileConfig::Initialize
 *          read and initialize for homescreen configuration
 *
 * @param[in]   conf    configuration file name
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
//--------------------------------------------------------------------------
bool
CicoGKeyFileConfig::Initialize(const char *conf)
{
    GError *error = NULL;
    char path[ICO_TEMP_BUF_SIZE];
    GString *filepath;

    if (NULL != m_gKeyFile) {
        /*if initialize was done*/
        return true;
    }

    m_gKeyFile = g_key_file_new();
    
    filepath = g_string_new("test");
    CicoResourceConfig::GetConfPath(path, sizeof(path));
    g_string_printf(filepath, "%s/%s", path, conf);
    if (!g_file_test(filepath->str, G_FILE_TEST_IS_REGULAR)) {
        return false;
    }

    // load config file
    g_key_file_load_from_file(m_gKeyFile, filepath->str,
                              (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS |
                                              G_KEY_FILE_KEEP_TRANSLATIONS),
                              &error);
    if (error != NULL) {
        ICO_WRN("%s: %s", filepath->str, error->message);
        g_error_free(error);
        return false;
    }

    g_string_free(filepath, TRUE);

    return true;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoGKeyFileConfig::ConfigGetInteger
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
CicoGKeyFileConfig::ConfigGetInteger(const char *group_name, const char *key, int default_value)
{
    GError *error = NULL;

    if (m_gKeyFile == NULL) {
        return default_value;
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
 * @brief   CicoGKeyFileConfig::ConfigGetString
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
CicoGKeyFileConfig::ConfigGetString(const char *group_name, const char *key,
                                    const char *default_value)
{
    GError *error = NULL;

    if (m_gKeyFile == NULL) {
        return default_value;
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
