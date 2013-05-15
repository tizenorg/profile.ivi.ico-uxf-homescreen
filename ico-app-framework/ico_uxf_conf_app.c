/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library(read apprication's configuration file)
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
#include <ail.h>

#include "ico_apf_log.h"
#include "ico_apf_apimacro.h"
#include "ico_uxf_conf_common.h"

#define APP_CONF_AIL_NULL_STR   "(null)"

static Ico_Uxf_App_Config *readAilApplist(void);
static void Ico_Uxf_conf_remakeAppHash(void);

static Ico_Uxf_App_Config   *_ico_app_config = NULL;
static Ico_Uxf_Sys_Config   *sys_config = NULL;
static GKeyFile             *sappfile = NULL;


/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getAppConfig: get application configurations
 *
 * @param       none
 * @return      application configuration table
 * @retval      !=NULL          success(application configuration table address)
 * @retval      ==NULL          error(can not read configuration files)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_App_Config *
ico_uxf_getAppConfig(void)
{
    if (_ico_app_config) {
        return _ico_app_config;
    }
    _ico_app_config = g_new0(Ico_Uxf_App_Config,1);
    return readAilApplist();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   infoAilpkg: callback function for configuration from AppCore(static function)
 *
 * @param[in]   appinfo         AppCore(AIL) application information
 * @param[in]   data            user data(application index)
 * @return      result
 * @retval      AIL_CB_RET_CONTINUE success(continue next application)
 * @retval      AIL_CB_RET_CANCEL   error(stop all application search)
 */
/*--------------------------------------------------------------------------*/
static ail_cb_ret_e
infoAilpkg(const ail_appinfo_h appinfo, void *data)
{
    int     num = (int)data;
    char    *package;
    char    *icon;
    char    *name;
    char    *category;
    char    *type;
    char    *exec;
    GError  *error;
    char    *app_category;
    char    add_category[256];
    int     i;
    bool    bval;
    struct stat buff;
    Ico_Uxf_conf_application *apptbl;

    /* get package name for appid */
    ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &package);
    if (strcmp(package, APP_CONF_AIL_NULL_STR) == 0) {
        package = NULL;
    }
    /* get icon path */
    ail_appinfo_get_str(appinfo, AIL_PROP_ICON_STR, &icon);
    if (strcmp(icon, APP_CONF_AIL_NULL_STR) == 0) {
        icon = NULL;
    }
    else if (icon != NULL) {
        /* file check */
        memset(&buff, 0, sizeof(buff));
        if (stat(icon, &buff) == 0) {
            if (S_ISDIR(buff.st_mode)) {
                /* is directory */
                icon = NULL;
            }
        }
        else {
            /* is not exist */
            icon = NULL;
        }
    }
    /* get name */
    ail_appinfo_get_str(appinfo, AIL_PROP_NAME_STR, &name);
    if (strcmp(name, APP_CONF_AIL_NULL_STR) == 0) {
        name = NULL;
    }

    /* get default category of this application */
    add_category[0] = 0;
    error = NULL;
    app_category = g_key_file_get_string(sappfile, "app-attributes", package, &error);
    if (error != NULL)  {
        g_clear_error(&error);
        error = NULL;
        app_category = g_key_file_get_string(sappfile, "app-attributes", name, &error);
    }
    if (error != NULL)  {
        g_clear_error(&error);
        apfw_trace("infoAilpkg: %s(%s) dose not has app-attributes", package, name);
    }
    else    {
        apfw_trace("infoAilpkg: %s(%s) has app-category=%s", package, name, app_category);
        strncpy(add_category, app_category, sizeof(add_category)-2);
        add_category[sizeof(add_category)-2] = 0;
        i = strlen(add_category);
        if (i > 0)  {
            if (add_category[i-1] != ';')   {
                strcpy(&add_category[i], ";");
            }
        }
    }
    g_clear_error(&error);

    /* get category */
    ail_appinfo_get_str(appinfo, AIL_PROP_CATEGORIES_STR, &category);
    if (strcmp(category, APP_CONF_AIL_NULL_STR) != 0) {
        apfw_trace("infoAilpkg: %s + %s", add_category, category);
        i = strlen(add_category);
        strncpy(&add_category[i], category, sizeof(add_category)-i-1);
        add_category[sizeof(add_category)-i-1] = 0;
    }
    if (add_category[0])    {
        category = add_category;
    }
    else    {
        category = NULL;
    }

    /* get type */
    ail_appinfo_get_str(appinfo, AIL_PROP_TYPE_STR, &type);
    if (strcmp(type, APP_CONF_AIL_NULL_STR) == 0) {
        type = NULL;
    }
    /* get exec */
    ail_appinfo_get_str(appinfo, AIL_PROP_EXEC_STR, &exec);
    if (strcmp(exec, APP_CONF_AIL_NULL_STR) == 0) {
        exec = NULL;
    }

    if ((package != NULL) && (*package != 0))   {
        apptbl = &_ico_app_config->application[_ico_app_config->applicationNum];
        apptbl->appid = strdup(package);
        if (icon)   {
            apptbl->icon_key_name = strdup(icon);
        }
        else    {
            apptbl->icon_key_name = strdup("\0");
        }
        if ((name != NULL) && (*name != 0)) {
            apptbl->name = strdup(name);
        }
        else    {
            apptbl->name = strdup(package);
        }

        /* set default values       */
        apptbl->hostId = sys_config->misc.default_hostId;
        apptbl->kindId = sys_config->misc.default_kindId;
        apptbl->categoryId = sys_config->misc.default_categoryId;

        /* get NoDisplay    */
        if ((icon != NULL) && (*icon != 0)) {
            bval = false;
            ail_appinfo_get_bool(appinfo, AIL_PROP_NODISPLAY_BOOL, &bval);
            apptbl->noicon = (int)bval;
        }
        else    {
            apfw_trace("infoAilpkg: %s(%s) has no icon", package, name);
            apptbl->noicon = 1;
        }

        /* analize categorys for extended attributes    */
        if (category)   {
            int     i, j, k, m, n;
            int     found;
            int     dispidx = sys_config->misc.default_displayId;;
            int     soundidx = sys_config->misc.default_soundId;;
            int     inputidx = sys_config->misc.default_inputdevId;
            Ico_Uxf_conf_display    *display;
            Ico_Uxf_conf_sound      *sound;
            char    work[64];

            apfw_trace("Ail.%d category=%s", _ico_app_config->applicationNum, category);

            j = 0;
            for (i = 0;; i++)   {
                if ((category[i] == 0) || (category[i] == ';')) {
                    k = i - j;
                    if (k >= ((int)sizeof(work)-1)) k = sizeof(work)-1;
                    memcpy(work, &category[j], k);
                    work[k] = 0;
                    found = 0;

                    /* find running host        */
                    if (strncasecmp(work, "run=", 4) == 0)  {
                        found = 4;
                    }
                    for (k = 0; k < sys_config->hostNum; k++)  {
                        if (strcasecmp(&work[found], sys_config->host[k].name) == 0)   {
                            apptbl->hostId = sys_config->host[k].id;
                            found = 1;
                            break;
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown running host", work);
                    }
                    /* find kind                */
                    if (found == 0) {
                        if (strncasecmp(work, "kind=", 5) == 0) {
                            found = 5;
                        }
                        for (k = 0; k < sys_config->kindNum; k++)  {
                            if (strcasecmp(&work[found],
                                           sys_config->kind[k].name) == 0)   {
                                apptbl->kindId = sys_config->kind[k].id;
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown kind", work);
                    }
                    /* find category            */
                    if (found == 0) {
                        if (strncasecmp(work, "category=", 9) == 0) {
                            found = 9;
                        }
                        for (k = 0; k < sys_config->categoryNum; k++)  {
                            if (strcasecmp(&work[found],
                                           sys_config->category[k].name) == 0)   {
                                apptbl->categoryId = sys_config->category[k].id;
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown category", work);
                    }
                    /* find display             */
                    if ((found == 0) &&
                        (strncasecmp(work, "display", 7) == 0))   {
                        if (work[7] == '=') {
                            dispidx = 0;
                            found = 8;
                        }
                        else if ((work[7] == '.') && (work[9] == '='))  {
                            dispidx = work[8] - '0';
                            found = 10;
                            if ((dispidx < 0) || (dispidx >= ICO_UXF_APPDISPLAY_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown display number", work);
                                dispidx = 0;
                                break;
                            }
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown display number", work);
                            dispidx = 0;
                            break;
                        }
                        if (apptbl->displayzoneNum <= dispidx)  {
                            apptbl->display[dispidx].displayId
                                    = sys_config->misc.default_displayId;
                            apptbl->display[dispidx].layerId
                                    = sys_config->misc.default_layerId;
                            apptbl->display[dispidx].zoneId
                                    = sys_config->misc.default_dispzoneId;
                        }
                        for (k = 0; k < sys_config->displayNum; k++)   {
                            if (strcasecmp(&work[found],
                                           sys_config->display[k].name) == 0)    {
                                apptbl->display[dispidx].displayId
                                        = sys_config->display[k].id;
                                if (apptbl->displayzoneNum <= dispidx)  {
                                    apptbl->displayzoneNum = dispidx + 1;
                                }
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown display", work);
                    }
                    /* find display layer       */
                    if ((found == 0) &&
                        (strncasecmp(work, "layer", 5) == 0)) {
                        if (work[5] == '=') {
                            dispidx = 0;
                            found = 6;
                        }
                        else if ((work[5] == '.') && (work[7] == '='))  {
                            dispidx = work[6] - '0';
                            found = 8;
                            if ((dispidx < 0) || (dispidx >= ICO_UXF_APPDISPLAY_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown display number", work);
                                dispidx = 0;
                                break;
                            }
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown display number", work);
                            dispidx = 0;
                            break;
                        }
                        if (apptbl->displayzoneNum <= dispidx)  {
                            apptbl->display[dispidx].displayId
                                    = sys_config->misc.default_displayId;
                            apptbl->display[dispidx].layerId
                                    = sys_config->misc.default_layerId;
                            apptbl->display[dispidx].zoneId
                                    = sys_config->misc.default_dispzoneId;
                        }
                        display = &sys_config->display[apptbl->display[dispidx].displayId];

                        for (k = 0; k < display->layerNum; k++) {
                            if (strcasecmp(&work[found], display->layer[k].name) == 0)  {
                                apptbl->display[dispidx].layerId = display->layer[k].id;
                                if (apptbl->displayzoneNum <= dispidx)  {
                                    apptbl->displayzoneNum = dispidx + 1;
                                }
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown layer", work);
                    }
                    /* find display zone        */
                    if ((found == 0) &&
                        (strncasecmp(work, "dispzone", 8) == 0))  {
                        if (work[8] == '=') {
                            dispidx = 0;
                            found = 9;
                        }
                        else if ((work[8] == '.') && (work[10] == '=')) {
                            dispidx = work[9] - '0';
                            found = 11;
                            if ((dispidx < 0) || (dispidx >= ICO_UXF_APPDISPLAY_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown display number", work);
                                dispidx = 0;
                                break;
                            }
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown display number", work);
                            dispidx = 0;
                            break;
                        }
                        if (apptbl->displayzoneNum <= dispidx)  {
                            apptbl->display[dispidx].displayId
                                    = sys_config->misc.default_displayId;
                            apptbl->display[dispidx].layerId
                                    = sys_config->misc.default_layerId;
                            apptbl->display[dispidx].zoneId
                                    = sys_config->misc.default_dispzoneId;
                        }
                        display = &sys_config->display[apptbl->display[dispidx].displayId];

                        for (k = 0; k < display->zoneNum; k++)  {
                            if (strcasecmp(&work[found], display->zone[k].name) == 0)   {
                                apptbl->display[dispidx].zoneId = display->zone[k].id;
                                if (apptbl->displayzoneNum <= dispidx)  {
                                    apptbl->displayzoneNum = dispidx + 1;
                                }
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown display zone", work);
                    }

                    /* find sound zone      */
                    if ((found == 0) &&
                        (strncasecmp(work, "soundzone", 9) == 0)) {
                        if (work[9] == '=') {
                            soundidx = 0;
                            found = 10;
                        }
                        else if ((work[9] == '.') && (work[11] == '=')) {
                            soundidx = work[10] - '0';
                            found = 12;
                            if ((soundidx < 0) || (soundidx >= ICO_UXF_APPSOUND_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown sound number", work);
                                soundidx = 0;
                                break;
                            }
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown sound number", work);
                            soundidx = 0;
                            break;
                        }
                        if (apptbl->soundzoneNum <= soundidx)   {
                            apptbl->sound[soundidx].soundId
                                    = sys_config->misc.default_soundId;
                            apptbl->sound[soundidx].zoneId
                                    = sys_config->misc.default_soundzoneId;
                        }
                        sound = &sys_config->sound[apptbl->sound[soundidx].soundId];

                        for (k = 0; k < sound->zoneNum; k++)    {
                            if (strcasecmp(&work[found], sound->zone[k].name) == 0) {
                                apptbl->sound[soundidx].zoneId = sound->zone[k].id;
                                if (apptbl->soundzoneNum <= soundidx)   {
                                    apptbl->soundzoneNum = soundidx + 1;
                                }
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown sound zone", work);
                    }

                    /* find sound           */
                    if ((found == 0) &&
                        (strncasecmp(work, "sound", 5) == 0)) {
                        if (work[5] == '=') {
                            soundidx = 0;
                            found = 5;
                        }
                        else if ((work[5] == '.') && (work[7] == '='))  {
                            soundidx = work[6] - '0';
                            found = 8;
                            if ((soundidx < 0) || (soundidx >= ICO_UXF_APPSOUND_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown sound number", work);
                                soundidx = 0;
                                break;
                            }
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown sound number", work);
                            soundidx = 0;
                            break;
                        }
                        if (apptbl->soundzoneNum <= soundidx)   {
                            apptbl->sound[soundidx].soundId
                                    = sys_config->misc.default_soundId;
                            apptbl->sound[soundidx].zoneId
                                    = sys_config->misc.default_soundzoneId;
                        }
                        for (k = 0; k < sys_config->soundNum; k++) {
                            if (strcasecmp(&work[found], sys_config->sound[k].name) == 0)  {
                                apptbl->sound[soundidx].soundId = sys_config->sound[k].id;
                                if (apptbl->soundzoneNum <= soundidx)   {
                                    apptbl->soundzoneNum = soundidx + 1;
                                }
                                found = 1;
                                break;
                            }
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown sound", work);
                    }

                    /* find input device    */
                    if ((found == 0) &&
                        (strncasecmp(work, "input", 5) == 0)) {
                        if (work[5] == '=') {
                            inputidx = 0;
                            found = 6;
                        }
                        else if ((work[5] == '.') && (work[7] == '='))  {
                            inputidx = work[6] - '0';
                            if ((inputidx < 0) || (inputidx >= ICO_UXF_APPINPUT_MAX))   {
                                apfw_error("infoAilpkg: [%s] unknown input number", work);
                                found = 1;
                                break;
                            }
                            found = 8;
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown input number", work);
                            break;
                        }
                        if (apptbl->inputdevNum <= inputidx)    {
                            apptbl->input[inputidx].inputdevId
                                    = sys_config->misc.default_inputdevId;
                        }
                        for (k = 0; k < sys_config->inputdevNum; k++)  {
                            m = strlen(sys_config->inputdev[k].name);
                            if (strncasecmp(&work[found],
                                            sys_config->inputdev[k].name, m) == 0)  {
                                if (work[found+m] != '.')   {
                                    apfw_error("infoAilpkg: [%s] unknown input sw", work);
                                    break;
                                }
                                apptbl->input[inputidx].inputdevId
                                        = sys_config->inputdev[k].id;
                                m += (found + 1);
                                for (n = 0; n < sys_config->inputdev[k].inputswNum; n++)   {
                                    if (strcasecmp(&work[m],
                                                   sys_config->inputdev[k].inputsw[n].name)
                                            == 9) {
                                        apptbl->input[inputidx].inputswId = n;
                                        break;
                                    }
                                }
                                if (n < sys_config->inputdev[k].inputswNum)    {
                                    if (apptbl->inputdevNum <= inputidx)   {
                                        apptbl->inputdevNum = inputidx + 1;
                                    }
                                }
                                else    {
                                    apfw_error("infoAilpkg: [%s] unknown input sw", work);
                                }
                                found = 1;
                                break;
                            }
                        }
                        if (k >= sys_config->inputdevNum)  {
                            apfw_error("infoAilpkg: [%s] unknown input device", work);
                            found = 1;
                            break;
                        }
                    }
                    if (found > 1)  {
                        apfw_error("infoAilpkg: [%s] unknown input", work);
                    }

                    /* start mode               */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strcasecmp(work, "auto") == 0)  {
                            apptbl->autostart = 1;
                        }
                        else if (strcasecmp(work, "noauto") == 0)   {
                            apptbl->autostart = 0;
                        }
                        else    {
                            apfw_error("infoAilpkg: [%s] unknown key", work);
                        }
                    }
                    if (category[i] == 0)   break;
                    j = i + 1;
                }
            }
        }
        apptbl->exec = strdup(exec);
        apptbl->type = strdup(type);
        if (apptbl->displayzoneNum == 0)    {
            apptbl->displayzoneNum = 1;
            apptbl->display[0].displayId = sys_config->misc.default_displayId;
            apptbl->display[0].layerId = sys_config->misc.default_layerId;
            apptbl->display[0].zoneId = sys_config->misc.default_dispzoneId;
        }
        if (apptbl->soundzoneNum == 0)  {
            apptbl->soundzoneNum = 1;
            apptbl->sound[0].soundId = sys_config->misc.default_soundId;
            apptbl->sound[0].zoneId = sys_config->misc.default_soundzoneId;
        }
        apfw_trace("Ail.%d: appid=%s name=%s exec=%s type=%s",
                   _ico_app_config->applicationNum, apptbl->appid, apptbl->name,
                   apptbl->exec, apptbl->type);
        apfw_trace("Ail.%d: categ=%d kind=%d disp=%d layer=%d zone=%d "
                   "sound=%d zone=%d auto=%d noicon=%d",
                   _ico_app_config->applicationNum, apptbl->categoryId, apptbl->kindId,
                   apptbl->display[0].displayId, apptbl->display[0].layerId,
                   apptbl->display[0].zoneId, apptbl->sound[0].soundId,
                   apptbl->sound[0].zoneId, apptbl->autostart, apptbl->noicon);
        _ico_app_config->applicationNum++;
    }
    else    {
    }

    if (_ico_app_config->applicationNum > num)
        return AIL_CB_RET_CANCEL;

    return AIL_CB_RET_CONTINUE;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   readAilApplist: get all application configuration from AppCore(static function)
 *
 * @param       none
 * @return      application configuration table
 * @retval      !=NULL          success(application configuration table address)
 * @retval      ==NULL          error(can not read configuration files)
 */
/*--------------------------------------------------------------------------*/
static Ico_Uxf_App_Config *
readAilApplist(void)
{
    int     ret, num, wnum;
    ail_filter_h filter;
    GError  *error = NULL;

    /* get system configuration */
    sys_config = (Ico_Uxf_Sys_Config *)ico_uxf_getSysConfig();
    if (! sys_config)   {
        apfw_error("readAilApplist: can not read system configuration");
        return NULL;
    }

    /* read system configuration file for application default category  */
    sappfile = g_key_file_new();

    GString* filepath = g_string_new("xx");
    g_string_printf(filepath, "%s/%s", sys_config->misc.confdir, ICO_UXF_CONFIG_APPATTR);

    if (! g_key_file_load_from_file(sappfile, filepath->str,
                                    G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
                                    &error)) {
        apfw_error("readAilApplist: %s %s", (char *)filepath->str, error->message);
        g_key_file_free(sappfile);
        sappfile = NULL;
    }
    g_string_free(filepath, TRUE);

    /* count packages */
    ret = ail_filter_count_appinfo(NULL, &num);
    if (ret != AIL_ERROR_OK) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        return NULL;
    }
    apfw_trace("readAilApplist: number of off AIL package = %d", num);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "menu");
    ail_filter_count_appinfo(filter, &num);
    apfw_trace("readAilApplist: number of menu AIL package = %d", num);
    ail_filter_destroy(filter);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "Application");
    ail_filter_count_appinfo(filter, &wnum);
    apfw_trace("readAilApplist: number of Application AIL package = %d", wnum);
    ail_filter_destroy(filter);
    num += wnum;

    _ico_app_config->application = g_new0(Ico_Uxf_conf_application, num);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "menu");
    ail_filter_list_appinfo_foreach(filter, infoAilpkg, (void *)num);
    ail_filter_destroy(filter);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "Application");
    ail_filter_list_appinfo_foreach(filter, infoAilpkg, (void *)num);
    ail_filter_destroy(filter);

    /* create Hash Table                    */
    Ico_Uxf_conf_remakeAppHash();

    if( sappfile)   {
        g_key_file_free(sappfile);
    }

    return _ico_app_config;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_closeAppConfig: close application configuration table
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_closeAppConfig(void)
{
    if(_ico_app_config != NULL){
        g_free(_ico_app_config->application);
        g_free(_ico_app_config);
        _ico_app_config = NULL;
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   Ico_Uxf_conf_remakeAppHash: make application hash table(static function)
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
Ico_Uxf_conf_remakeAppHash(void)
{
    int         i;
    int         hash;
    Ico_Uxf_conf_application    *app;

    memset(_ico_app_config->hashnametable, 0, sizeof(_ico_app_config->hashnametable));

    for (i = 0; i < _ico_app_config->applicationNum; i++)  {

        _ico_app_config->application[i].nextidhash = NULL;
        hash = ICO_UXF_MISC_HASHBYID(i);
        app = _ico_app_config->hashidtable[hash];
        if (! app) {
            _ico_app_config->hashidtable[hash] = &_ico_app_config->application[i];
        }
        else    {
            while (app->nextidhash)    {
                app = app->nextidhash;
            }
            app->nextidhash = &_ico_app_config->application[i];
        }

        _ico_app_config->application[i].nextnamehash = NULL;
        hash = ICO_UXF_MISC_HASHBYNAME(_ico_app_config->application[i].appid);
        app = _ico_app_config->hashnametable[hash];
        if (! app) {
            _ico_app_config->hashnametable[hash] = &_ico_app_config->application[i];
        }
        else    {
            while (app->nextnamehash)  {
                app = app->nextnamehash;
            }
            app->nextnamehash = &_ico_app_config->application[i];
        }
    }
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getAppByAppid: get application configuration by application Id
 *
 * @param[in]   appid       application Id
 * @return      application configuration table
 * @retval      !=NULL          success(application configuration table address)
 * @retval      ==NULL          error(application dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_conf_application *
ico_uxf_getAppByAppid(const char *appid)
{
    Ico_Uxf_conf_application    *app;

    if (!_ico_app_config)  {
        (void)ico_uxf_getAppConfig();
    }
    app = _ico_app_config->hashnametable[ICO_UXF_MISC_HASHBYNAME(appid)];

    while (app)    {
        if (strcasecmp(app->appid, appid) == 0)   break;
        app = app->nextnamehash;
    }
    if (! app)  {
        apfw_warn("ico_uxf_getAppByAppid: appid(%s) dose not exist", appid);
    }
    return app;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getAppByName: get application configuration by application name
 *
 * @param[in]   name            application name
 * @return      application configuration table
 * @retval      !=NULL          success(application configuration table address)
 * @retval      ==NULL          error(application dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_conf_application *
ico_uxf_getAppByName(const char *name)
{
    int     i;
    Ico_Uxf_conf_application    *app = NULL;

    if (!_ico_app_config)  {
        (void)ico_uxf_getAppConfig();
    }

    for (i = 0; i < _ico_app_config->applicationNum; i++)   {
        if (strcasecmp(_ico_app_config->application[i].name, name) == 0)    {
            app = &_ico_app_config->application[i];
            break;
        }
    }
    if (! app)  {
        apfw_warn("ico_uxf_getAppByName: name(%s) dose not exist", name);
    }
    return app;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getAppDisplay: get application display
 *
 * @param[in]   app         application configuration table
 * @param[in]   idx         display zone index
 * @param[out]  x           zone X coodinate
 * @param[out]  y           zone Y coodinate
 * @param[out]  width       zone width
 * @param[out]  height      zone height
 * @return      result
 * @retval      ICO_UXF_EOK     succes
 * @retval      ICO_UXF_EINVAL  error(illegal idx)
 * @retval      ICO_UXF_ENOSYS  error(can not read configuration files)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getAppDisplay(const Ico_Uxf_conf_application *app, const int idx,
                      int *x, int *y, int *width, int *height)
{
    Ico_Uxf_conf_display_zone *zone;

    if (! sys_config)  {
        return ICO_UXF_ENOSYS;
    }
    if ((! app) || (idx < 0) || (idx >= app->displayzoneNum))  {
        return ICO_UXF_EINVAL;
    }

    zone = &sys_config->display[app->display[idx].displayId].zone[app->display[idx].zoneId];

    if (x)      *x = zone->x;
    if (y)      *y = zone->y;
    if (width)  *width = zone->width;
    if (height) *height = zone->height;

    return 0;
}

