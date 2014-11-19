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
#include <string.h>
#include <tzplatform_config.h>
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
 * @param[in]   conf    configuration file name or path
 * @param[in]   pkgname package name for default config file
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
//--------------------------------------------------------------------------
bool
CicoGKeyFileConfig::Initialize(const char *conf, const char *pkgname)
{
    GError *error = NULL;
    GString *filepath;

    if (NULL != m_gKeyFile) {
        /*if initialize was done*/
        return true;
    }

    m_gKeyFile = g_key_file_new();
    filepath = g_string_new("xx");

    // if absolute path, fix file path
    if (*conf == '/')   {
        g_string_assign(filepath, conf);
        if (! g_file_test(filepath->str, G_FILE_TEST_IS_REGULAR)) {
            g_string_free(filepath, TRUE);
            return false;
        }
    }
    else    {
        // search user dir ($HOME/ico/config)
        const char *userpath = tzplatform_mkpath3(TZ_USER_HOME,
                                                  ICO_SYC_CONFIGPATH_HOME_CONFIG, conf);
        if (g_file_test(userpath, G_FILE_TEST_IS_REGULAR))  {
            // exist user config file
            g_string_assign(filepath, userpath);
        }
        else    {
            if (! pkgname)  {
                pkgname = getenv("PKG_NAME");
                if (! pkgname)  {
                    g_string_free(filepath, TRUE);
                    return false;
                }
            }
            // search package dir ($TZ_SYS_RO_APP/pkgname/res/config)
            const char *pkgpath = tzplatform_mkpath4(TZ_SYS_RO_APP, pkgname,
                                                     ICO_SYC_CONFIGPATH_PACKAGE_CONFIG,
                                                     conf);
            if (g_file_test(pkgpath, G_FILE_TEST_IS_REGULAR))   {
                // exist package config file
                g_string_assign(filepath, pkgpath);
            }
            else    {
                g_string_free(filepath, TRUE);
                return false;
            }
        }
    }

    // load config file
    ICO_DBG("CicoGKeyFileConfig::Initialize load config file<%s>", filepath->str);
    g_key_file_load_from_file(m_gKeyFile, filepath->str,
                              (GKeyFileFlags)(G_KEY_FILE_KEEP_COMMENTS |
                                              G_KEY_FILE_KEEP_TRANSLATIONS), &error);
    g_string_free(filepath, TRUE);

    if (error != NULL) {
        ICO_WRN("%s: %s", filepath->str, error->message);
        g_error_free(error);
        return false;
    }
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
        ICO_WRN("%s:%s> %s", group_name, key, error->message);
        g_error_free(error);
        return default_value;
    }
    return value;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoGKeyFileConfig::ConfigGetFilePath
 *          Return string value of homescreen configuration file path.
 *          If not exist, return default vlaue in params.
 *
 * @param[in]   group_name          configuration group name
 * @param[in]   key                 configuration name
 * @param[in]   subdir              $HOME sub directory
 * @param[in]   confdir             package configuration sub directory
 * @param[in]   default_path        default file path
 * @return      config file path
 */
/*--------------------------------------------------------------------------*/
const char *
CicoGKeyFileConfig::ConfigGetFilePath(const char *group_name, const char *key,
                                      const char *subdir, const char *confdir,
                                      const char *default_path)
{
    GError *error = NULL;

    if (m_gKeyFile != NULL) {
        const char *value = g_key_file_get_string(m_gKeyFile, group_name, key, &error);
        if (error == NULL) {
            if (*value == '/')  {
                // absolute path
                return value;
            }

            // check user home directory
            const char *homepath = tzplatform_mkpath3(TZ_USER_HOME, subdir, value);
            if (g_file_test(homepath, G_FILE_TEST_IS_REGULAR))  {
                return homepath;
            }

            // check default config path
            const char *confpath = tzplatform_mkpath3(TZ_SYS_RO_APP, confdir, value);
            if (g_file_test(confpath, G_FILE_TEST_IS_REGULAR))  {
                return confpath;
            }
        }
        else    {
            ICO_WRN("%s", error->message);
            g_error_free(error);
        }
    }
    if (! default_path) {
        return NULL;
    }
    if (*default_path == '/')   {
        return default_path;
    }
    return tzplatform_mkpath(TZ_SYS_RO_APP, default_path);
}
// vim:set expandtab ts=4 sw=4:
