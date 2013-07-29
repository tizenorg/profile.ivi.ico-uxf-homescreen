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

#include <package-manager.h>

#include "ico_apf_log.h"
#include "ico_apf_apimacro.h"
#include "ico_uxf_conf_common.h"

/*==============================================================================*/
/* define                                                                       */
/*==============================================================================*/
#define APP_CONF_AIL_NULL_STR   "(NULL)"

#define APP_CONF_EVENT_OK           (0)
#define APP_CONF_EVENT_FAIL         (1)

typedef struct _conf_pkgmgr_event conf_pkgmgr_event_t;

struct _conf_pkgmgr_event {
    conf_pkgmgr_event_t *next;
    char pkg_name[ICO_UXF_MAX_PROCESS_NAME];
    int type;
};

/*==============================================================================*/
/* define static function prototype                                             */
/*==============================================================================*/
static Ico_Uxf_App_Config *readAilApplist(void);
static void ico_uxf_conf_remakeAppHash(void);
static int ico_uxf_conf_pkgmgrEvent(int req_id, const char *pkg_type,
                    const char *pkg_name, const char *key, const char *val,
                    const void *pmsg, void *data);
static int ico_uxf_conf_addPkgmgrEventListener(void);
static int ico_uxf_conf_startEvent(const char *pkg_name, int type);
static int ico_uxf_conf_endEvent(const char *pkg_name, int status);

/*==============================================================================*/
/* static tables                                                                */
/*==============================================================================*/
static Ico_Uxf_App_Config   *_ico_app_config = NULL;
static Ico_Uxf_App_Config   *_ico_app_config_update = NULL;
static Ico_Uxf_Sys_Config   *sys_config = NULL;
static GKeyFile             *sappfile = NULL;
static char                 *default_icon = NULL;

static pkgmgr_client        *conf_pc = NULL;
static conf_pkgmgr_event_t  *conf_prog_event = NULL;
static Ico_Uxf_AppUpdata_Cb conf_cb_func = NULL;

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
    _ico_app_config_update = _ico_app_config;
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
    int     app_category_type;
    char    add_category[400];
    int     add_category_len;
    char    work[80];
    int     i;
    bool    bval;
    struct stat buff;
    Ico_Uxf_conf_application *apptbl;

    _ico_app_config_update->ailNum++;

    /* get package name for appid */
    ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &package);
    if (strcasecmp(package, APP_CONF_AIL_NULL_STR) == 0) {
        package = NULL;
    }
    /* get icon path */
    ail_appinfo_get_str(appinfo, AIL_PROP_ICON_STR, &icon);
    if (strcasecmp(icon, APP_CONF_AIL_NULL_STR) == 0) {
        icon = NULL;
    }
    if ((icon == NULL) || (*icon == 0)) {
        icon = default_icon;
    }
    /* get name */
    ail_appinfo_get_str(appinfo, AIL_PROP_NAME_STR, &name);
    if (strcasecmp(name, APP_CONF_AIL_NULL_STR) == 0) {
        name = NULL;
    }

    /* get default category of this application */
    add_category[0] = 0;
    error = NULL;
    app_category = g_key_file_get_string(sappfile, "app-attributes", package, &error);
    if (error == NULL)  {
        app_category_type = 0;
    }
    else    {
        g_clear_error(&error);
        error = NULL;
        app_category = g_key_file_get_string(sappfile, "app-attributes", name, &error);
        if (error == NULL)  {
            app_category_type = 1;
        }
    }
    add_category_len = 0;
    if (error != NULL)  {
        g_clear_error(&error);
        apfw_trace("infoAilpkg: %s(%s) dose not has app-attributes", package, name);
    }
    else    {
        for (i = 1;; i++)   {
            strncpy(&add_category[add_category_len], 
                    app_category, sizeof(add_category)-add_category_len-2);
            add_category[sizeof(add_category)-2] = 0;
            add_category_len = strlen(add_category);
            if (add_category_len > 0)  {
                if (add_category[add_category_len-1] != ';')   {
                    strcpy(&add_category[add_category_len++], ";");
                }
            }
            snprintf(work, sizeof(work)-1, "%s.%d",
                     app_category_type == 0 ? package : name, i);
            error = NULL;
            app_category = g_key_file_get_string(sappfile, "app-attributes", work, &error);
            if (error != NULL)  {
                g_clear_error(&error);
                break;
            }
        }
        apfw_trace("infoAilpkg: %s(%s) has app-category=%s", package, name, add_category);
    }
    g_clear_error(&error);

    /* get category */
    ail_appinfo_get_str(appinfo, AIL_PROP_CATEGORIES_STR, &category);
    if (strcasecmp(category, APP_CONF_AIL_NULL_STR) != 0) {
        apfw_trace("infoAilpkg: %s + %s", add_category, category);
        strncpy(&add_category[add_category_len],
                category, sizeof(add_category)-add_category_len-1);
        add_category[sizeof(add_category)-1] = 0;
    }
    if (add_category[0])    {
        category = add_category;
    }
    else    {
        category = NULL;
    }

    /* get type */
    ail_appinfo_get_str(appinfo, AIL_PROP_TYPE_STR, &type);
    /* get exec */
    ail_appinfo_get_str(appinfo, AIL_PROP_EXEC_STR, &exec);
    if (strcasecmp(exec, APP_CONF_AIL_NULL_STR) == 0) {
        exec = NULL;
    }

    if ((package != NULL) && (*package != 0))   {
        apptbl = &_ico_app_config_update->application[_ico_app_config_update->applicationNum];
        apptbl->appid = strdup(package);
        if (icon)   {
            if ((stat(icon, &buff) == 0) &&
                (! S_ISDIR(buff.st_mode))) {
                apptbl->icon_key_name = strdup(icon);
            }
            else    {
                apptbl->icon_key_name = "\0";
            }
        }
        else    {
            apptbl->icon_key_name = "\0";
        }
        if ((name != NULL) && (*name != 0)) {
            apptbl->name = strdup(name);
        }
        else    {
            apptbl->name = strdup(package);
        }
        if (strcasecmp(type, APP_CONF_AIL_NULL_STR) == 0) {
            apptbl->type = NULL;
        }
        else    {
            apptbl->type = strdup(type);
        }

        /* set default values       */
        apptbl->hostId = sys_config->misc.default_hostId;
        apptbl->kindId = sys_config->misc.default_kindId;
        apptbl->categoryId = sys_config->misc.default_categoryId;
        apptbl->invisiblecpu = 100;

        /* get NoDisplay    */
        if ((apptbl->icon_key_name != NULL) && (*apptbl->icon_key_name != 0)) {
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

            apfw_trace("Ail.%d category=%s", _ico_app_config_update->applicationNum, category);

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
                    /* find type                */
                    if (found == 0) {
                        if (strncasecmp(work, "type=", 5) == 0) {
                            found = 1;
                            if (apptbl->type)   {
                                free(apptbl->type);
                            }
                            apptbl->type = strdup(&work[5]);
                        }
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

                    /* NoDisplay                */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strncasecmp(work, "NoDisplay", 9) == 0)  {
                            apptbl->noicon = 1;
                            if (work[9] == '=') {
                                if (strcasecmp(&work[10], "false") == 0)    {
                                    apptbl->noicon = 0;
                                }
                            }
                            found = 9;
                        }
                    }

                    /* surface animation        */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strncasecmp(work, "Animation=", 10) == 0)  {
                            apptbl->animation = strdup(&work[10]);
                            found = 9;
                        }
                    }
                    /* surface animation time   */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strncasecmp(work, "Animation_time=", 15) == 0)  {
                            apptbl->animation_time = strtol(&work[15], (char **)0, 0);
                            found = 9;
                        }
                    }

                    /* cpu % at invisible       */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strncasecmp(work, "invisiblecpu", 12) == 0)  {
                            apptbl->invisiblecpu = 0;
                            if (work[12] == '=')    {
                                if (strcasecmp(&work[13], "yes") == 0)  {
                                    apptbl->invisiblecpu = 100;
                                }
                                else if (strcasecmp(&work[13], "no") != 0)  {
                                    apptbl->invisiblecpu = strtol(&work[13], (char **)0, 0);
                                    if (apptbl->invisiblecpu > 100)
                                        apptbl->invisiblecpu = 100;
                                }
                            }
                            found = 9;
                        }
                    }

                    /* configure event          */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strcasecmp(work, "noconfigure") == 0)  {
                            apptbl->noconfigure = 1;
                            found = 9;
                        }
                    }

                    /* overlap on HomeScreen menu   */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strcasecmp(work, "menuoverlap") == 0)  {
                            apptbl->menuoverlap = 1;
                            found = 9;
                        }
                    }

                    /* start mode               */
                    if ((found == 0) && (work[0] != 0)) {
                        if (strncasecmp(work, "auto", 4) == 0)  {
                            apptbl->autostart = 1;
                        }
                        else if (strncasecmp(work, "noauto", 6) == 0)   {
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
        apfw_trace("Ail.%d: appid=%s name=%s exec=%s icon=%s type=%s",
                   _ico_app_config_update->applicationNum, apptbl->appid, apptbl->name,
                   apptbl->exec, apptbl->icon_key_name, apptbl->type);
        apfw_trace("Ail.%d: categ=%d kind=%d disp=%d layer=%d zone=%d "
                   "sound=%d zone=%d auto=%d noicon=%d anim=%s.%d overlap=%d cpu=%d",
                   _ico_app_config_update->applicationNum, apptbl->categoryId, apptbl->kindId,
                   apptbl->display[0].displayId, apptbl->display[0].layerId,
                   apptbl->display[0].zoneId, apptbl->sound[0].soundId,
                   apptbl->sound[0].zoneId, apptbl->autostart, apptbl->noicon,
                   apptbl->animation ? apptbl->animation : "(none)",
                   apptbl->animation_time, apptbl->menuoverlap, apptbl->invisiblecpu);
        _ico_app_config_update->applicationNum++;
    }

    if (_ico_app_config_update->applicationNum > num)
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

    _ico_app_config_update->applicationNum = 0;

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

    if (g_key_file_load_from_file(sappfile, filepath->str,
                                  G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS,
                                  &error)) {
        error = NULL;
        default_icon = g_key_file_get_string(sappfile, "app-icon", "default-icon", &error);
        if (error != NULL)  {
            g_clear_error(&error);
            default_icon = NULL;
        }
    }
    else    {
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
        apfw_trace("readAilApplist: Leave(Ail:cannot count appinfo) = %d", ret);
        return NULL;
    }
    apfw_trace("readAilApplist: number of off AIL package = %d", num);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "menu");
    ret = ail_filter_count_appinfo(filter, &num);
    if (ret != AIL_ERROR_OK) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        apfw_trace("readAilApplist: Leave(Ail:cannot count appinfo(menu)) = %d", ret);
        return NULL;
    }
    apfw_trace("readAilApplist: number of menu AIL package = %d", num);
    ail_filter_destroy(filter);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "Application");
    ret = ail_filter_count_appinfo(filter, &wnum);
    if (ret != AIL_ERROR_OK) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        apfw_trace("readAilApplist: Leave(Ail:cannot count appinfo(Application)) = %d", ret);
        return NULL;
    }
    apfw_trace("readAilApplist: number of Application AIL package = %d", wnum);
    ail_filter_destroy(filter);
    num += wnum;

    _ico_app_config_update->application = g_new0(Ico_Uxf_conf_application, num);

    ail_filter_new(&filter);
    ret = ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "menu");
    ret = ail_filter_list_appinfo_foreach(filter, infoAilpkg, (void *)num);
    if (ret != AIL_ERROR_OK) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        ail_filter_destroy(filter);
        apfw_trace("readAilApplist: Leave(Ail:cannot get appinfo(menu)) = %d", ret);
        return NULL;
    }
    ail_filter_destroy(filter);

    ail_filter_new(&filter);
    ail_filter_add_str(filter, AIL_PROP_TYPE_STR, "Application");
    ret = ail_filter_list_appinfo_foreach(filter, infoAilpkg, (void *)num);
    if (ret != AIL_ERROR_OK) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        ail_filter_destroy(filter);
        apfw_trace("readAilApplist: Leave(Ail:cannot get appinfo(Application)) = %d", ret);
        return NULL;
    }
    ail_filter_destroy(filter);

    if (_ico_app_config_update->ailNum != num) {
        if( sappfile)   {
            g_key_file_free(sappfile);
            sappfile = NULL;
        }
        apfw_trace("readAilApplist: Leave(cannot read ail correctly %d =! %d",
                    _ico_app_config_update->ailNum, num);
        return NULL;
    }

    /* create Hash Table                    */
    ico_uxf_conf_remakeAppHash();

    if( sappfile)   {
        g_key_file_free(sappfile);
    }

    apfw_trace("readAilApplist: Leave");

    return _ico_app_config_update;
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
 * @brief   ico_uxf_conf_remakeAppHash: make application hash table(static function)
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_conf_remakeAppHash(void)
{
    int         i;
    int         hash;
    Ico_Uxf_conf_application    *app;

    memset(_ico_app_config_update->hashnametable, 0, sizeof(_ico_app_config_update->hashnametable));

    for (i = 0; i < _ico_app_config_update->applicationNum; i++)  {

        _ico_app_config_update->application[i].nextidhash = NULL;
        hash = ICO_UXF_MISC_HASHBYID(i);
        app = _ico_app_config_update->hashidtable[hash];
        if (! app) {
            _ico_app_config_update->hashidtable[hash] = &_ico_app_config_update->application[i];
        }
        else    {
            while (app->nextidhash)    {
                app = app->nextidhash;
            }
            app->nextidhash = &_ico_app_config_update->application[i];
        }

        _ico_app_config_update->application[i].nextnamehash = NULL;
        hash = ICO_UXF_MISC_HASHBYNAME(_ico_app_config_update->application[i].appid);
        app = _ico_app_config_update->hashnametable[hash];
        if (! app) {
            _ico_app_config_update->hashnametable[hash] = &_ico_app_config_update->application[i];
        }
        else    {
            while (app->nextnamehash)  {
                app = app->nextnamehash;
            }
            app->nextnamehash = &_ico_app_config_update->application[i];
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

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_addPkgmgrEventListener:
 *          request to listen the pkgmgr's broadcasting.
 *
 * @param       none
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ENOSYS  cannot regist listener
 */
/*--------------------------------------------------------------------------*/
static int
ico_uxf_conf_addPkgmgrEventListener(void)
{
    int     ret;

    if (conf_pc) {
        /* already registied listener */
        return ICO_UXF_EOK;
    }

    conf_pc = pkgmgr_client_new(PC_LISTENING);
    if (conf_pc == NULL) {
        apfw_trace("ico_uxf_conf_addPkgmgrEventListener: cannot create pkgmgr client");
        return ICO_UXF_ENOSYS;
    }

    ret = pkgmgr_client_listen_status(conf_pc, ico_uxf_conf_pkgmgrEvent, NULL);
    if (ret < 0) {
        apfw_trace("ico_uxf_conf_addPkgmgrEventListener: "
                   "cannot register listener of pkgmgr(%d)", ret);
        pkgmgr_client_free(conf_pc);
        conf_pc = NULL;
        return ICO_UXF_ENOSYS;
    }

    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_pkgmgrEvent:
 *          This is callback function from pkgmgr.
 *
 * @param[in]   req_id          request id
 * @param[in]   pkg_type        package type
 * @param[in]   pkg_name        package name
 * @param[in]   key             broadcast key(=start/end/...)
 * @param[in]   val             broadcast value(=install/uninstall/ok/fail)
 * @param[in]   pmsg            broadcast comment
 * @param[in]   data            user data
 * @return      always 0
 */
/*--------------------------------------------------------------------------*/
static int
ico_uxf_conf_pkgmgrEvent(int req_id, const char *pkg_type, const char *pkg_name,
                    const char *key, const char *val, const void *pmsg, void *data)
{
    apfw_trace("ico_uxf_conf_PkgmgrEvent: "
               "Enter(pkg_type=%s, pkg_name=%s, key=%s, val=%s, pmsg=%s)",
               pkg_type, pkg_name, key, val, pmsg);

    if (strcasecmp(key, "start") == 0) {
        if (strcasecmp(val, "install") == 0) {
            ico_uxf_conf_startEvent(pkg_name, ICO_UXF_CONF_EVENT_INSTALL);
        }
        else if (strcasecmp(val, "uninstall") == 0) {
            ico_uxf_conf_startEvent(pkg_name, ICO_UXF_CONF_EVENT_UNINSTALL);
        }
    }
    else if (strcasecmp(key, "install_percent") == 0) {
        ico_uxf_conf_startEvent(pkg_name, ICO_UXF_CONF_EVENT_INSTALL);
    }
    else if (strcasecmp(key, "progress_percent") == 0) {

    }
    else if (strcasecmp(key, "error") == 0) {
    }
    else if (strcasecmp(key, "end") == 0) {
        if (strcasecmp(val, "ok") == 0) {
            ico_uxf_conf_endEvent(pkg_name, APP_CONF_EVENT_OK);
        }
        else if (strcasecmp(val, "fail") == 0) {
            ico_uxf_conf_endEvent(pkg_name, APP_CONF_EVENT_FAIL);
        }
    }

    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_startEvent: mark event is start
 *
 * @param[in]   pkg_name        package name
 * @param[in]   type            event type
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ENOMEM  cannot allocate memory
 */
/*--------------------------------------------------------------------------*/
static int
ico_uxf_conf_startEvent(const char *pkg_name, int type)
{
    conf_pkgmgr_event_t *new = NULL;
    conf_pkgmgr_event_t *event = NULL;

    /* check the queue whether the package' event is exist */
    event = conf_prog_event;
    while (event) {
        if (strncmp(event->pkg_name, pkg_name, ICO_UXF_MAX_PROCESS_NAME) == 0) {
            new = event;
            break;
        }
        event = event->next;
    }

    if (!new) {
        new = malloc(sizeof(conf_pkgmgr_event_t));
        if (!new) {
            apfw_warn("ico_uxf_conf_startEvent: cannot allocate memory");
            return ICO_UXF_ENOMEM;
        }
        memset(new, 0, sizeof(new));
        /* insert queue */
        event = conf_prog_event;
        while (event) {
            if (!event->next) {
                break;
            }
            event = event->next;
        }
        if (!event) {
            conf_prog_event = new;
        }
        else {
            event->next = new;
        }
        strncpy(new->pkg_name, pkg_name, ICO_UXF_MAX_PROCESS_NAME);
        new->type = type;
    }



    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_conf_endEvent: mark event is end
 *
 * @param[in]   pkg_name        package name
 * @param[in]   status          end status
 * @return      result
 * @retval      ICO_UXF_EOK     success
 */
/*--------------------------------------------------------------------------*/
static int
ico_uxf_conf_endEvent(const char *pkg_name, int status)
{
    int     type;
    int     cnt;
    int     ii, jj;
    int     exist = 0;
    conf_pkgmgr_event_t *current = NULL;
    conf_pkgmgr_event_t *event;
    conf_pkgmgr_event_t *bevent;
    static Ico_Uxf_App_Config *config = NULL;
    static Ico_Uxf_App_Config *tmp = NULL;

    apfw_trace("ico_uxf_conf_endEvent: Enter(pkg=%s, stat=%d)", pkg_name, status);
    /* get start event from queue */
    event = conf_prog_event;
    bevent = NULL;
    while (event) {
        if (strncmp(event->pkg_name, pkg_name, ICO_UXF_MAX_PROCESS_NAME) == 0) {
            current = event;
            break;
        }
        bevent = event;
        event = event->next;
    }
    if (current) {
        if (!bevent) {
            /* top */
            conf_prog_event = current->next;
            current->next = NULL;
        }
        else {
            bevent->next = current->next;
            current->next = NULL;
        }
    }
    else {
        return ICO_UXF_ENOSYS;
    }

    type = current->type;
    free(current);

    if (status == APP_CONF_EVENT_OK) {
        config = g_new0(Ico_Uxf_App_Config, 1);
        _ico_app_config_update = config;
        config = readAilApplist();
        cnt = 0;
        while (! config) {
            usleep(10000);
            apfw_trace("ico_uxf_conf_endEvent: Retry %d", cnt);
            config = readAilApplist();
            if (cnt > 500) {
                break;
            }
            else {
                cnt++;
            }
        }
        if (! config) {
            apfw_warn("ico_uxf_getAppByAppid: cannot access ail normally");
            return ICO_UXF_EBUSY;
        }
        tmp = config;
        /* old list */
        config = _ico_app_config;
        /* new list */
        _ico_app_config = tmp;

        if (type == ICO_UXF_CONF_EVENT_INSTALL) {
            for (ii = 0; ii < _ico_app_config->applicationNum; ii++) {
                exist = 0;
                for (jj = 0; jj < config->applicationNum; jj++) {
                    if (strcmp(_ico_app_config->application[ii].appid,
                               config->application[jj].appid) == 0) {
                        exist = 1;
                        break;
                    }
                }
                if ((exist == 0) && conf_cb_func) {
                    conf_cb_func(_ico_app_config->application[ii].appid,
                                 ICO_UXF_CONF_EVENT_INSTALL);
                }
            }
        }
        else if (type == ICO_UXF_CONF_EVENT_UNINSTALL) {
            for (jj = 0; jj < config->applicationNum; jj++) {
                exist = 0;
                for (ii = 0; ii < _ico_app_config->applicationNum; ii++) {
                    if (strcmp(config->application[jj].appid,
                               _ico_app_config->application[ii].appid) == 0) {
                        exist = 1;
                        break;
                    }
                }
                if ((exist == 0) && conf_cb_func) {
                    conf_cb_func(config->application[jj].appid,
                                 ICO_UXF_CONF_EVENT_UNINSTALL);
                }
            }
        }
        /* free old list */
        if (config != NULL) {
            g_free(config->application);
            g_free(config);
        }
    }

    apfw_trace("ico_uxf_conf_endEvent: Leave");

    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_setAppUpdateCb:
 *
 * @param       none
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ENOSYS  cannot regist callback
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_conf_setAppUpdateCb(Ico_Uxf_AppUpdata_Cb func)
{
    int     ret;

    ret = ico_uxf_conf_addPkgmgrEventListener();

    if (ret != ICO_UXF_EOK) {
        apfw_trace("ico_uxf_conf_setAppUpdateCb: cannot add listener");
        return ICO_UXF_ENOSYS;
    }
    conf_cb_func = func;

    return ICO_UXF_EOK;
}

