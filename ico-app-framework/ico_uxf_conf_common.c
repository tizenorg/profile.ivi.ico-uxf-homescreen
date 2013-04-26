/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library(common function in read configuration)
 *
 * @date    Feb-28-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/file.h>
#include <errno.h>

#include "ico_apf_log.h"
#include "ico_apf_apimacro.h"
#include "ico_uxf_conf_common.h"

/* Number of hash table                         */
#define ICO_UXF_MISC_HASHSIZE   64          /* Must be 2's factorial            */

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_misc_hashByName: create hash value by name string
 *          there is no distinction of an english capital letter/small
 *
 * @param[in]   name        character string changed into a hash value(NULL termination)
 *
 * @return  create hash value
 */
/*--------------------------------------------------------------------------*/
int
ico_uxf_misc_hashByName(const char *name)
{
    int     hash = 0;
    int     i;

    for (i = 0; name[i]; i++)  {
        hash = (hash << 1) | (name[i] & 0x1f);
    }
    return hash & (ICO_UXF_MISC_HASHSIZE-1);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_getUint: convert value string to value
 *
 * @param[in]   str         string
 * @return      converted value
 */
/*--------------------------------------------------------------------------*/
int
ico_uxf_conf_getUint(const char* str)
{
    int key = -1;
    if (str != NULL)    {
        if ((strcasecmp(str, "true") == 0) ||
            (strcasecmp(str, "yes") == 0))   {
            key = 1;
        }
        else if ((strcasecmp(str, "false") == 0) ||
                 (strcasecmp(str, "no") == 0))   {
            key = 0;
        }
        else    {
            char    *errpt = (char *)0;
            key = strtol(str, &errpt, 0);
            if ((errpt) && (*errpt != 0))  {
                key = -1;
            }
        }
    }
    return key;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_countNumericalKey: count numerical keys in configuration file
 *
 * @param[in]   keyfile         configuration file
 * @param[in]   group           configuration groupe(ex.[display])
 * @return      configuration list
 */
/*--------------------------------------------------------------------------*/
GList *
ico_uxf_conf_countNumericalKey(GKeyFile *keyfile, const char* group)
{
    GList*  list = NULL;
    char    **result;
    gsize   length;
    int     i;

    result = g_key_file_get_keys(keyfile, group, &length,NULL);

    for (i = 0; i < (int)length; i++) {
        int id = ico_uxf_conf_getUint(result[i]);
        if (id >= 0) {
            list=g_list_append(list,g_strdup(result[i]));
        }
    }
    g_strfreev(result);
    return list;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_appendStr: connect strings
 *
 * @param[in]   str1            string 1
 * @param[in]   str2            string 2
 * @return      static connected string (str1+str2)
 */
/*--------------------------------------------------------------------------*/
const char *
ico_uxf_conf_appendStr(const char* str1, const char* str2)
{
    static char buf[256];
    sprintf(buf, "%s%s", str1, str2);
    return buf;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_checkGerror: output configuration error message
 *
 * @param[in]   error           error information
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
ico_uxf_conf_checkGerror(GError** error)
{
    if (*error != NULL) {
        uifw_warn("%s",(*error)->message);
    }
    g_clear_error(error);
}

