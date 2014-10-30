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
 * @param[in]   conf    configuration file name
 * @param[in]   dir     configuration dir (or NULL)
 * @return      result
 * @retval      true    success
 * @retval      false   error
 */
//--------------------------------------------------------------------------
bool
CicoGKeyFileConfig::Initialize(const char *conf, const char *dir)
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
    if (dir)    {
        strncpy(path, dir, sizeof(path));
        g_string_printf(filepath, "%s/%s", path, conf);
        if (! g_file_test(filepath->str, G_FILE_TEST_IS_REGULAR)) {
            CicoResourceConfig::GetConfPath(path, sizeof(path));
        }
    }
    else    {
        CicoResourceConfig::GetConfPath(path, sizeof(path));
    }
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoGKeyFileConfig::ConfigGetString
 *          Return string value in homescreen configuration file.
 *          If not exist,return defaultvlaue in params.
 *          It will be changed
 *          if the environment variable of Tizen is contained in the character string.
 *
 * @param[in]   group_name          configuration group name
 * @param[in]   key                 configuration name
 * @param[in]   default_value       default value
 * @param[out]  buf                 return get strings
 * @param[in]   len                 byte length of buf
 * @return      0=success/-1=error
 */
/*--------------------------------------------------------------------------*/
int
CicoGKeyFileConfig::ConfigGetString(const char *group_name, const char *key,
                                    const char *default_value,
                                    char *buf, int len)
{
    GError *error = NULL;

    if (m_gKeyFile == NULL) {
        strncpy(buf, default_value, len);
        return -1;
    }

    const char *value = g_key_file_get_string(m_gKeyFile, group_name, key,
                                              &error);
    if (error != NULL) {
        ICO_WRN("%s", error->message);
        g_error_free(error);
        strncpy(buf, default_value, len);
        return -1;
    }
    if (value == NULL)  {
        buf[0] = 0;
        return -1;
    }
    return ConfigTizenPlatform(value, buf, len);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   CicoGKeyFileConfig::ConfigTizenPlatform
 *          It will be changed if the environment variable of Tizen is
 *          contained in the character string.
 *
 * @param[in]   str                 input string
 * @param[out]  buf                 return strings
 * @param[in]   len                 byte length of buf
 * @return      0=success/-1=error
 */
/*--------------------------------------------------------------------------*/
int
CicoGKeyFileConfig::ConfigTizenPlatform(const char *str, char *buf, int len)
{
    memset(buf, 0, len);

    if (str == NULL)  {
        return -1;
    }
    int i = 0;
    while ((i < (len-1)) && (*str != 0))  {
        int tzcount = tzplatform_getcount();
        const char *tzv = NULL;
        if ((*str == 'T') && (*(str+1) == 'Z') && (*(str+2) == '_'))  {
            if ((strncmp(str + 3, "SYS_", 4) == 0) ||
                (strncmp(str + 3, "USER_", 5) == 0) ||
                (strncmp(str + 3, "SDK_", 4) == 0))  {
                int j;
                for (j = 0; j < tzcount; j++)   {
                    const char *tzp = tzplatform_getname((enum tzplatform_variable)j);
                    if ((tzp != NULL) && (strncmp(tzp, str, strlen(tzp)) == 0)) {
                        tzv = tzplatform_getenv((enum tzplatform_variable)j);
                        if (tzv != NULL)    {
                            str += strlen(tzp);
                        }
                        break;
                    }
                }
            }
        }
        if (tzv == NULL)    {
            buf[i++] = *str;
            str ++;
        }
        else    {
            int tzlen = strlen(tzv);
            if ((i + tzlen) < (len-1))  {
                strncpy(&buf[i], tzv, tzlen);
                i += tzlen;
            }
        }
    }
    ICO_DBG("ConfigTizenPlatform <%s>", buf);
    return 0;
}
// vim:set expandtab ts=4 sw=4:
