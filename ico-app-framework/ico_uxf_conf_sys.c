/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library(read configuration file in all system)
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

static Ico_Uxf_Sys_Config   *reloadSysConfig(void);

static Ico_Uxf_Sys_Config   *_ico_sys_config = NULL;


/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysConfig: get system configurations
 *
 * @param       none
 * @return      system configuration table
 * @retval      != NULL         success(system configuration table address)
 * @retval      == NULL         error(can not read configuration file)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_Sys_Config *
ico_uxf_getSysConfig(void)
{
    if (_ico_sys_config) {
        return _ico_sys_config;
    }
    _ico_sys_config = g_new0(Ico_Uxf_Sys_Config,1);
    return reloadSysConfig();
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_ifGetSysConfig: get system configurations
 *
 * @param       none
 * @return      system configuration table
 * @retval      != NULL         success(system configuration table address)
 * @retval      == NULL         error(the configuration file is not yet read)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_Sys_Config *
ico_uxf_ifGetSysConfig(void)
{
    return _ico_sys_config;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   pars_dispposition: calculate an expression about display size(static function)
 *
 * @param[in]   exp             expression
 * @param[in]   display         display configuration table
 * @return      calculation result
 */
/*--------------------------------------------------------------------------*/
static int
pars_dispposition(const char *exp, Ico_Uxf_conf_display *display)
{
    int     val = 0;
    int     wval;
    int     i, j, sign;
    char    word[32];

    j = 0;
    sign = 0;
    for (i = 0; ; i++)  {
        if (exp[i] == ' ')  continue;
        if ((exp[i] == 0) || (exp[i] == '+') || (exp[i] == '-') ||
            (exp[i] == '*') || (exp[i] == '-') || (exp[i] == '/'))  {
            if (j > 0)  {
                word[j] = 0;
                if ((strcasecmp(word, "dispw") == 0) ||
                    (strcasecmp(word, "width") == 0))   {
                    wval = display->width;
                }
                else if ((strcasecmp(word, "disph") == 0) ||
                         (strcasecmp(word, "heigh") == 0))  {
                    wval = display->height;
                }
                else    {
                    wval = strtol(word, (char **)0, 0);
                }
                j = 0;
                if (sign >= 10) {
                    wval = 0 - wval;
                }
                switch (sign % 10)  {
                case 0:                     /* assign       */
                    val = wval;
                    break;
                case 1:                     /* '+'          */
                    val += wval;
                    break;
                case 2:                     /* '-'          */
                    val -= wval;
                    break;
                case 3:                     /* '*'          */
                    val *= wval;
                    break;
                case 4:                     /* '/'          */
                    val /= wval;
                    break;
                default:
                    break;
                }
                sign = 0;
                if (exp[i] == '+')      sign = 1;
                else if (exp[i] == '-') sign = 2;
                else if (exp[i] == '*') sign = 3;
                else if (exp[i] == '/') sign = 4;
                else                    sign = 0;
            }
            else    {
                if ((sign > 0) && (exp[i] == '-'))  {
                    sign += 10;
                }
                else    {
                    if (exp[i] == '+')      sign = 1;
                    else if (exp[i] == '-') sign = 2;
                    else if (exp[i] == '*') sign = 3;
                    else if (exp[i] == '/') sign = 4;
                    else                    sign = 0;
                }
            }
            if (exp[i] == 0)    break;
        }
        else    {
            if (j < ((int)sizeof(word)-1))  {
                word[j++] = exp[i];
            }
        }
    }
    return val;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   reloadSysConfig: load system configuration file(static function)
 *
 * @param       none
 * @return      system configuration table
 * @retval      != NULL         success(system configuration table address)
 * @retval      == NULL         error(can not read configuration file)
 */
/*--------------------------------------------------------------------------*/
static Ico_Uxf_Sys_Config *
reloadSysConfig(void)
{
    GKeyFile *keyfile;
    GKeyFileFlags flags;
    GError *error = NULL;
    gsize length;
    int i, j, k;
    int zoneidx;
    GList*  idlist;
    char*   dirp;
    char*   strp;

    apfw_trace("reloadSysConfig: Enter");

    /* decide top directory in all configurations       */
    dirp = getenv(ICO_UXF_TOP_ENV);
    if (! dirp)    {
        dirp = ICO_UXF_TOP_TOPDIR;
    }
    strncpy(_ico_sys_config->misc.topdir, dirp, ICO_UXF_CONF_DIR_MAX);

    /* decide top directory in configuration file's     */
    dirp = getenv(ICO_UXF_CONFIG_ENV);
    if (dirp)  {
        strncpy(_ico_sys_config->misc.confdir, dirp, ICO_UXF_CONF_DIR_MAX);
    }
    else    {
        snprintf(_ico_sys_config->misc.confdir, ICO_UXF_CONF_DIR_MAX,
                 "%s/%s", _ico_sys_config->misc.topdir, ICO_UXF_CONFIG_DIR);
        dirp = _ico_sys_config->misc.confdir;
    }

    /* A system-wide definition         */
    keyfile = g_key_file_new();
    flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

    GString* filepath = g_string_new("xx");
    g_string_printf(filepath, "%s/%s", dirp, ICO_UXF_CONFIG_SYSTEM);

    if (!g_key_file_load_from_file(keyfile, filepath->str, flags, &error)) {
        fprintf(stderr, "%s: %s\n", (char *)filepath->str, error->message);
        g_string_free(filepath, TRUE);
        return NULL;
    }
    g_string_free(filepath, TRUE);

    /* log                      */
    _ico_sys_config->misc.loglevel
             = g_key_file_get_integer(keyfile, "log", "loglevel", &error);
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.logflush =
        ico_uxf_getSysBoolByName(g_key_file_get_string(keyfile, "log", "logflush", &error));
    ico_uxf_conf_checkGerror(&error);

    /* log setting              */
    if (getenv("ICO_IVI_NO_LOG"))    {
        ico_uxf_log_setlevel(0);
    }
    else    {
        ico_uxf_log_setlevel(_ico_sys_config->misc.loglevel |
                             ((_ico_sys_config->misc.logflush) ?
                                ICO_APF_LOG_FLUSH : ICO_APF_LOG_NOFLUSH));
    }

    /* [category] application category name and attributes          */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "category");
    length = g_list_length(idlist);
    _ico_sys_config->categoryNum = length;
    _ico_sys_config->category = g_malloc0(sizeof(Ico_Uxf_conf_category) * length);

    for (i = 0; i < (int)length; i++) {
        const char* key = (const char*) g_list_nth_data(idlist, i);
        const char* g = "category";
        _ico_sys_config->category[i].id = ico_uxf_conf_getUint(key);
        ico_uxf_conf_checkGerror(&error);
        _ico_sys_config->category[i].name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);

        strp = (char *)g_key_file_get_string(keyfile, g,
                                             ico_uxf_conf_appendStr(key, ".view"), &error);
        ico_uxf_conf_checkGerror(&error);
        if (strncasecmp(strp, "alway", 5) == 0) {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_ALWAYS;
        }
        else if (strncasecmp(strp, "run", 3) == 0)  {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_RUNNING;
        }
        else if (strncasecmp(strp, "park", 4) == 0) {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_PARKED;
        }
        else if (strncasecmp(strp, "shift_park", 10) == 0)  {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_SHIFT_PARKING;
        }
        else if ((strncasecmp(strp, "shift_back", 10) == 0) ||
                 (strncasecmp(strp, "shift_rev", 9) == 0))  {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_SHIFT_REVERSES;
        }
        else if (strcasecmp(strp, "blinker_left") == 0)   {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_BLINKER_LEFT;
        }
        else if (strcasecmp(strp, "blinker_right") == 0)  {
            _ico_sys_config->category[i].view = ICO_UXF_POLICY_BLINKER_RIGHT;
        }
        else    {
            fprintf(stderr, "[%s.view: %s] Unknown view mode\n", key, strp);
            g_list_free_full(idlist,g_free);
            g_key_file_free(keyfile);
            return NULL;
        }

        strp = (char *)g_key_file_get_string(keyfile, g,
                                             ico_uxf_conf_appendStr(key, ".sound"), &error);
        ico_uxf_conf_checkGerror(&error);
        if (strncasecmp(strp, "alway", 5) == 0) {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_ALWAYS;
        }
        else if (strncasecmp(strp, "run", 3) == 0)  {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_RUNNING;
        }
        else if (strncasecmp(strp, "park", 4) == 0) {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_PARKED;
        }
        else if (strncasecmp(strp, "shift_park", 10) == 0)  {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_SHIFT_PARKING;
        }
        else if ((strncasecmp(strp, "shift_back", 10) == 0) ||
                 (strncasecmp(strp, "shift_rev", 9) == 0))  {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_SHIFT_REVERSES;
        }
        else if (strcasecmp(strp, "blinker_left") == 0)   {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_BLINKER_LEFT;
        }
        else if (strcasecmp(strp, "blinker_right") == 0)  {
            _ico_sys_config->category[i].sound = ICO_UXF_POLICY_BLINKER_RIGHT;
        }
        else    {
            fprintf(stderr, "[%s.view: %s] Unknown sound mode\n", key, strp);
            g_list_free_full(idlist,g_free);
            g_key_file_free(keyfile);
            return NULL;
        }

        strp = (char *)g_key_file_get_string(keyfile, g,
                                             ico_uxf_conf_appendStr(key, ".input"), &error);
        ico_uxf_conf_checkGerror(&error);
        if (strncasecmp(strp, "alway", 5) == 0) {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_ALWAYS;
        }
        else if (strncasecmp(strp, "run", 3) == 0)  {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_RUNNING;
        }
        else if (strncasecmp(strp, "park", 4) == 0) {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_PARKED;
        }
        else if (strncasecmp(strp, "shift_park", 10) == 0)  {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_SHIFT_PARKING;
        }
        else if ((strncasecmp(strp, "shift_back", 10) == 0) ||
                 (strncasecmp(strp, "shift_rev", 9) == 0))  {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_SHIFT_REVERSES;
        }
        else if (strcasecmp(strp, "blinker_left") == 0)   {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_BLINKER_LEFT;
        }
        else if (strcasecmp(strp, "blinker_right") == 0)  {
            _ico_sys_config->category[i].input = ICO_UXF_POLICY_BLINKER_RIGHT;
        }
        else    {
            fprintf(stderr, "[%s.view: %s] Unknown input mode\n", key, strp);
            g_list_free_full(idlist,g_free);
            g_key_file_free(keyfile);
            return NULL;
        }
        _ico_sys_config->category[i].priority =
                                g_key_file_get_integer(keyfile, g,
                                ico_uxf_conf_appendStr(key,".priority"), &error);
        ico_uxf_conf_checkGerror(&error);

        apfw_trace("Categry.%d id=%d name=%s view=%d sound=%d input=%d prio=%d",
                   i, _ico_sys_config->category[i].id, _ico_sys_config->category[i].name,
                   _ico_sys_config->category[i].view, _ico_sys_config->category[i].sound,
                   _ico_sys_config->category[i].input,
                   _ico_sys_config->category[i].priority);
    }
    g_list_free_full(idlist,g_free);

    /* [kind] application kind name and attributes          */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "kind");
    length = g_list_length(idlist);
    _ico_sys_config->kindNum = length;
    _ico_sys_config->kind = g_malloc0(sizeof(Ico_Uxf_conf_kind) * length);

    for (i = 0; i < (int)length; i++) {
        const char* key = (const char*) g_list_nth_data(idlist, i);
        const char* g = "kind";
        _ico_sys_config->kind[i].id = ico_uxf_conf_getUint(key);
        ico_uxf_conf_checkGerror(&error);
        _ico_sys_config->kind[i].name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);
        strp = (char *)g_key_file_get_string(
                            keyfile, g, ico_uxf_conf_appendStr(key, ".privilege"), &error);
        ico_uxf_conf_checkGerror(&error);
        if (strcasecmp(strp, "almighty") == 0)   {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_ALMIGHTY;
        }
        else if (strcasecmp(strp, "system") == 0)   {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_SYSTEM;
        }
        else if (strcasecmp(strp, "system.audio") == 0)   {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_SYSTEM_AUDIO;
        }
        else if (strcasecmp(strp, "system.visible") == 0)   {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_SYSTEM_VISIBLE;
        }
        else if (strcasecmp(strp, "maker") == 0)    {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_MAKER;
        }
        else if (strcasecmp(strp, "certificate") == 0)  {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_CERTIFICATE;
        }
        else if (strcasecmp(strp, "none") == 0) {
            _ico_sys_config->kind[i].priv = ICO_UXF_PRIVILEGE_NONE;
        }
        else    {
            fprintf(stderr, "[%s.view: %s] Unknown privilege mode\n", key, strp);
            g_list_free_full(idlist,g_free);
            g_key_file_free(keyfile);
            return NULL;
        }
        apfw_trace("Kind.%d id=%d name=%s privilege=%d",
                   i, _ico_sys_config->kind[i].id, _ico_sys_config->kind[i].name,
                   _ico_sys_config->kind[i].priv);
    }
    g_list_free_full(idlist,g_free);

    /* [port] port numbers              */
    _ico_sys_config->misc.appscontrollerport
        = g_key_file_get_integer(keyfile, "port", "appscontrollerport", &error);
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.soundpluginport
        = g_key_file_get_integer(keyfile, "port", "soundpluginport", &error);
    ico_uxf_conf_checkGerror(&error);
    if (_ico_sys_config->misc.appscontrollerport <= 0)
        _ico_sys_config->misc.appscontrollerport = 18081;
    if (_ico_sys_config->misc.soundpluginport <= 0)
        _ico_sys_config->misc.soundpluginport = 18082;

    /* hosts                        */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "host");
    length = g_list_length(idlist);
    _ico_sys_config->hostNum = length;
    _ico_sys_config->host = g_malloc0(sizeof(Ico_Uxf_conf_host) * length);

    for (i = 0; i < (int)length; i++)   {
        char* key = (char*) g_list_nth_data(idlist, i);
        const char* g = "host";
        _ico_sys_config->host[i].id  = ico_uxf_conf_getUint(key);
        _ico_sys_config->host[i].name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);

        _ico_sys_config->host[i].address
            = g_key_file_get_string(keyfile, g, ico_uxf_conf_appendStr(key, ".ip"), &error);
        ico_uxf_conf_checkGerror(&error);
        if (g_hostname_is_ip_address(_ico_sys_config->host[i].address)!= TRUE){
            apfw_error("host[%d] = %s is not ip address style",
                        _ico_sys_config->host[i].id, _ico_sys_config->host[i].address);
            exit(1);
        }

        _ico_sys_config->host[i].type =
            ico_uxf_getSysHostTypeByName(
                    g_key_file_get_string(keyfile, g,
                                          ico_uxf_conf_appendStr(key, ".type"), &error));
        ico_uxf_conf_checkGerror(&error);

        _ico_sys_config->host[i].homescreen =
            g_key_file_get_string(keyfile, g,
                                  ico_uxf_conf_appendStr(key, ".homescreen"), &error);
        ico_uxf_conf_checkGerror(&error);
        apfw_trace("Host.%d: Id=%d Name=%s Type=%d Ip=%s HomeScreen=%s",
            i, _ico_sys_config->host[i].id, (char *)_ico_sys_config->host[i].name,
            _ico_sys_config->host[i].type, (char *)_ico_sys_config->host[i].address,
            _ico_sys_config->host[i].homescreen);
    }
    g_list_free_full(idlist,g_free);

    /* decide myhost        */
    const char *penv = getenv(ICO_UXF_ENV_APPINF);
    int hostid = -1;
    int dmy1, dmy2, dmy3;
    char cdmy4[64];
    if (penv)  {
        if (sscanf(penv, "%d %d %d %s %d", &dmy1, &dmy2, &dmy3, cdmy4, &hostid) < 5) {
            hostid = -1;
        }
        apfw_trace("reloadSysConfig: Env(%s) = %s, hostId = %d",
                    ICO_UXF_ENV_APPINF, penv, hostid);
    }

    if (hostid < 0)    {
        if ((gethostname(cdmy4, sizeof(cdmy4)-1) >= 0) &&
            (strncmp(cdmy4, "localhost", 9) != 0))   {
            apfw_trace("hostname = %s", cdmy4);
            for (j = 0; cdmy4[j]; j++) {
                if ((cdmy4[j] == ' ') || (cdmy4[j] == '(') ||
                    (cdmy4[j] == '@') || (cdmy4[j] == ':'))    {
                    cdmy4[j] = 0;
                    break;
                }
            }
            for (j = 0; j < _ico_sys_config->hostNum; j++) {
                if (strcasecmp(_ico_sys_config->host[j].name, cdmy4) == 0)   {
                    hostid = _ico_sys_config->host[j].id;
                    break;
                }
            }
        }
    }

    if (hostid < 0)    {
        FILE *fp = popen("/sbin/ifconfig", "r");
        int foundeth = 0;
        char sBuf[128];

        if (fp)    {
            while (fgets(sBuf, sizeof(sBuf)-1, fp))  {
                if ((! foundeth) || (sBuf[0] != ' '))  {
                    foundeth = 0;
                    /* skip to "eth" device     */
                    if (sBuf[0] == ' ' || sBuf[0] == '\t') continue;
                    if (strncmp(sBuf, "eth", 3) != 0)    continue;
                }
                foundeth = 1;

                for (j = 0; sBuf[j]; j++)  {
                    if (strncmp(&sBuf[j], "inet6", 5) == 0)  continue;
                    if (strncmp(&sBuf[j], "inet", 4) == 0)   break;
                }
                if (sBuf[j])   {
                    /* forget "inet" device         */
                    j += 4;
                    for (; sBuf[j]; j++)   {
                        if (sBuf[j] == ':')    break;
                    }
                    if (sBuf[j] )  {
                        j++;
                        int k;
                        for (k = j +1; sBuf[k]; k++)   {
                            if (sBuf[k] == ' ' || sBuf[k] == '\t' ||
                                sBuf[k] == '\n' || sBuf[k] == '\r')    {
                                sBuf[k] = 0;
                                break;
                            }
                        }
                        apfw_trace("My Inet Addr = %s", &sBuf[j]);
                        for (k = 0; k < _ico_sys_config->hostNum; k++) {
                            if (strcmp(_ico_sys_config->host[k].address, &sBuf[j]) == 0) {
                                hostid = _ico_sys_config->host[k].id;
                                break;
                            }
                        }
                    }
                }
                if (hostid >= 0)   break;
            }
            fclose(fp);
        }
    }
    if (hostid >= 0)   {
        _ico_sys_config->misc.myhostId = hostid;
    }
    else    {
        apfw_trace("Default Host = 0, %s", ico_uxf_get_SysLocation(0));
        _ico_sys_config->misc.myhostId = _ico_sys_config->host[0].id;
    }

    /* The validity of MyHost is checked.       */
    Ico_Uxf_conf_host *myhost = (Ico_Uxf_conf_host *)
            ico_uxf_getSysHostById(_ico_sys_config->misc.myhostId);
    if (! myhost)  {
        apfw_critical("MyHostId.%d dose not exist, default 0",
                      _ico_sys_config->misc.myhostId);
        myhost = &_ico_sys_config->host[0];
        if (! myhost)  {
            apfw_error("HostId.0 dose not exist, GiveUp!");
            exit(1);
        }
        _ico_sys_config->misc.myhostId = myhost->id;
    }

    _ico_sys_config->misc.myaddress =
            (char *)ico_uxf_get_SysLocation(_ico_sys_config->misc.myhostId);
    _ico_sys_config->misc.myhostName = myhost->name;

    _ico_sys_config->misc.homescreen = myhost->homescreen;

    if (myhost->homescreen)    {
        _ico_sys_config->misc.masterhostId = myhost->id;
    }
    else    {
        _ico_sys_config->misc.masterhostId = _ico_sys_config->misc.myhostId;

        for (i = 0; i < _ico_sys_config->hostNum; i++) {
            if (_ico_sys_config->host[i].homescreen)   {
                _ico_sys_config->misc.masterhostId = _ico_sys_config->host[i].id;
                break;
            }
        }
    }
    _ico_sys_config->misc.masterhostaddress =
            (char *)ico_uxf_get_SysLocation(_ico_sys_config->misc.masterhostId);

    apfw_trace("MyHost %d[%s] Master Host %d(%s)", _ico_sys_config->misc.myhostId,
                _ico_sys_config->misc.myaddress, _ico_sys_config->misc.masterhostId,
                _ico_sys_config->misc.masterhostaddress);

    _ico_sys_config->misc.default_hostId = ico_uxf_getSysHostByName(
                            g_key_file_get_string(keyfile, "default", "host", &error));
    ico_uxf_conf_checkGerror(&error);

    /* display                  */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "display");
    length = g_list_length(idlist);
    _ico_sys_config->displayNum = length;
    _ico_sys_config->display = g_malloc0(sizeof(Ico_Uxf_conf_display) * length);
    zoneidx = 0;

    for (i = 0; i < (int)length; i++) {
        const char* key = (const char*) g_list_nth_data(idlist, i);
        const char* g = "display";
        Ico_Uxf_conf_display *display = &_ico_sys_config->display[i];
        display->id = ico_uxf_conf_getUint(key);
        ico_uxf_conf_checkGerror(&error);
        display->name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);

        display->hostId =
            ico_uxf_getSysHostByName(
                        g_key_file_get_string(keyfile, g,
                                              ico_uxf_conf_appendStr(key,".host"), &error));
        ico_uxf_conf_checkGerror(&error);
        display->displayno =
            g_key_file_get_integer(keyfile, g,
                                   ico_uxf_conf_appendStr(key,".displayno"), &error);
        if (error) {
            display->displayno = -1;
            g_clear_error(&error);
        }
        display->wayland =
            g_key_file_get_string(keyfile, g,
                                  ico_uxf_conf_appendStr(key,".wayland"), &error);
        ico_uxf_conf_checkGerror(&error);

        display->type =
            ico_uxf_getSysDispTypeByName(g_key_file_get_string(keyfile, g,
                                         ico_uxf_conf_appendStr(key,".type"), &error));
        ico_uxf_conf_checkGerror(&error);

        display->width =
            g_key_file_get_integer(keyfile, g, ico_uxf_conf_appendStr(key,".width"), &error);
        ico_uxf_conf_checkGerror(&error);
        display->height =
            g_key_file_get_integer(keyfile, g, ico_uxf_conf_appendStr(key,".height"), &error);
        ico_uxf_conf_checkGerror(&error);
        display->layerNum =
            g_key_file_get_integer(keyfile, g, ico_uxf_conf_appendStr(key,".layer"), &error);
        ico_uxf_conf_checkGerror(&error);

        /* inch                     */
        display->inch =
            g_key_file_get_integer(keyfile,g,ico_uxf_conf_appendStr(key,".inch"),&error);
        ico_uxf_conf_checkGerror(&error);

        apfw_trace("Display.%d Id=%d Name=%s Host=%d DispNo=%d W/H=%d/%d Wayland=%s inch=%d",
                   i, display->id, (char *)display->name, display->hostId,
                   display->displayno, display->width, display->height,
                   display->wayland, display->inch);

        /* display layer            */
        int displayer_length;
        gchar **layerlist;
        gsize layersize;
        GString* layer_key = g_string_new("");
        for (displayer_length = 0;
             displayer_length < ICO_UXF_DISPLAY_LAYER_MAX; displayer_length++)  {
            g_string_printf(layer_key,"%s.layer.%d",key,displayer_length);
            if (g_key_file_has_key(keyfile,g,layer_key->str,NULL) != TRUE)   break;
        }
        apfw_trace("Display.%d layerNum = %d" ,i, displayer_length);
        display->layerNum = displayer_length;
        display->layer = g_malloc0(sizeof(Ico_Uxf_conf_display_layer)*displayer_length);

        for (j = 0; j < displayer_length; j++)  {
            g_string_printf(layer_key,"%s.layer.%d",key,j);
            display->layer[j].id = j;
            layerlist = g_key_file_get_string_list(keyfile, g, layer_key->str,
                                                   &layersize, &error);
            ico_uxf_conf_checkGerror(&error);
            if (layersize <= 0) {
                apfw_error("Display.%d Layer.%d has no name", i, j);
            }
            else    {
                display->layer[j].name = strdup((char *)layerlist[0]);
                for (k = 1; k < (int)layersize; k++)    {
                    if (strcasecmp((char *)layerlist[k], "menuoverlap") == 0)   {
                        display->layer[j].menuoverlap = 1;
                    }
                }
                apfw_trace("Display.%d Layer.%d id=%d name=%s overlap=%d", i, j,
                           display->layer[j].id, display->layer[j].name,
                           display->layer[j].menuoverlap);
            }
            if (layerlist)  g_strfreev(layerlist);
        }
        g_string_free(layer_key,TRUE);

        /* display zone         */
        int dispzone_length;
        GString* zone_key = g_string_new("");
        for (dispzone_length = 0;
             dispzone_length < ICO_UXF_DISPLAY_ZONE_MAX; dispzone_length++) {
            g_string_printf(zone_key,"%s.zone.%d",key,dispzone_length);
            if (g_key_file_has_key(keyfile,g,zone_key->str,NULL) != TRUE)    break;
        }
        apfw_trace("Display.%d zoneNum = %d" ,i, dispzone_length);
        display->zoneNum = dispzone_length;
        display->zone = g_malloc0(sizeof(Ico_Uxf_conf_display_zone)*dispzone_length);

        gsize zonesizes[ICO_UXF_DISPLAY_ZONE_MAX];
        gchar **zonelists[ICO_UXF_DISPLAY_ZONE_MAX];
        gchar **zonelist;
        Ico_Uxf_conf_display_zone *zone;

        for (j = 0; j < dispzone_length; j++)   {
            g_string_printf(zone_key,"%s.zone.%d",key,j);
            zonelists[j] = g_key_file_get_string_list(
                                    keyfile, g, zone_key->str, &zonesizes[j], &error);
            ico_uxf_conf_checkGerror(&error);
            if ((! zonelists[j]) || (zonesizes[j] <= 0)) {
                apfw_error("reloadSysConfig: display.%d zone.%d dose not exist",i,j);
                zonesizes[j] = 0;
            }
        }
        for (j = 0; j < dispzone_length; j++)   {
            zone = &(display->zone[j]);
            zone->id = j;
            zone->zoneidx = zoneidx++;
            zone->display = display;
            zone->node = ICO_UXF_NODEDISP_2_NODEID(display->hostId, display->displayno);

            if (zonesizes[j] <= 0)  continue;

            zonelist = zonelists[j];
            zone->name = strdup((char*)zonelist[0]);
            if (zonesizes[j] < 5)   {
                /* default zone is full screen      */
                zone->x = 0;
                zone->y = 0;
                zone->width = display->width;
                zone->height = display->height;
            }
            else    {
                zone->x = pars_dispposition((char *)zonelist[1], display);
                zone->y = pars_dispposition((char *)zonelist[2], display);
                zone->width = pars_dispposition((char *)zonelist[3], display);
                zone->height = pars_dispposition((char *)zonelist[4], display);
            }
        }
        for (j = 0; j < dispzone_length; j++)   {
            int k, l, m, n;
            if (zonesizes[j] <= 5)  continue;
            zonelist = zonelists[j];
            zone = &(display->zone[j]);
            m = 0;
            for (k = 5; (m < ZONE_MAX_OVERLAP) && (k < (int)zonesizes[j]); k++) {
                for (l = 0; l < dispzone_length; l++)   {
                    if (l == j) continue;
                    if (strcasecmp(zonelist[k], display->zone[l].name) == 0)    {
                        for (n = 0; n < m; n++) {
                            if (zone->overlap[n] == display->zone[l].id)    break;
                        }
                        if (n >= m) {
                            zone->overlap[m++] = display->zone[l].id;
                        }
                        break;
                    }
                }
                if (l >= dispzone_length)   {
                    apfw_error("reloadSysConfig: disp.%d zone.%d overlap zone(%s) "
                               "dose not exist", i, j, (char *)zonelist[k]);
                }
            }
            zone->overlapNum = m;
        }
        for (j = 0; j < dispzone_length; j++)   {
            if (zonelists[j])   g_strfreev(zonelists[j]);
            zone = &(display->zone[j]);
            apfw_trace("Disp.%d zone.%d node=%x x/y=%d/%d w/h=%d/%d over=%d.%d %d %d %d %d",
                       i, j, zone->node, zone->x, zone->y, zone->width, zone->height,
                       zone->overlapNum, zone->overlap[0], zone->overlap[1],
                       zone->overlap[2], zone->overlap[3], zone->overlap[4]);
        }
        g_string_free(zone_key,TRUE);
    }
    g_list_free_full(idlist,g_free);

    /* sound devices                        */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "sound");
    length = g_list_length(idlist);
    _ico_sys_config->soundNum = length;
    _ico_sys_config->sound = g_malloc0(sizeof(Ico_Uxf_conf_sound) * length);
    zoneidx = 0;

    for (i = 0; i < (int)length; i++) {
        const char* key = (const char*) g_list_nth_data(idlist, i);
        const char* g = "sound";
        Ico_Uxf_conf_sound *sound = &_ico_sys_config->sound[i];
        sound->id = ico_uxf_conf_getUint(key);
        ico_uxf_conf_checkGerror(&error);
        sound->name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);

        sound->hostId =
            ico_uxf_getSysHostByName(g_key_file_get_string(
                                    keyfile, g, ico_uxf_conf_appendStr(key,".host"), &error));
        ico_uxf_conf_checkGerror(&error);
        sound->soundno =
            g_key_file_get_integer(keyfile, g,
                                   ico_uxf_conf_appendStr(key,".soundno"), &error);
        if (error) {
            sound->soundno = -1;
            g_clear_error(&error);
        }

        apfw_trace("Sound.%d Id=%d Name=%s Host=%d SoundNo=%d",
                   i, sound->id, (char *)sound->name, sound->hostId, sound->soundno);

        /* sound zone           */
        int sound_length;
        GString* sound_key = g_string_new("");
        for (sound_length = 0;
             sound_length < ICO_UXF_SOUND_ZONE_MAX; sound_length++) {
            g_string_printf(sound_key,"%s.zone.%d", key, sound_length);
            if (g_key_file_has_key(keyfile,g,sound_key->str,NULL) != TRUE)   break;
        }
        apfw_trace("Sound.%d zoneNum = %d" ,i, sound_length);
        sound->zoneNum = sound_length;
        sound->zone = g_malloc0(sizeof(Ico_Uxf_conf_sound_zone)*sound_length);

        gsize zonesizes[ICO_UXF_SOUND_ZONE_MAX];
        gchar **zonelists[ICO_UXF_SOUND_ZONE_MAX];
        gchar **zonelist;
        Ico_Uxf_conf_sound_zone *zone;

        for (j = 0; j < sound_length; j++)  {
            g_string_printf(sound_key,"%s.zone.%d",key,j);
            zonelists[j] =
                g_key_file_get_string_list(keyfile,g,sound_key->str,&zonesizes[j],&error);
            ico_uxf_conf_checkGerror(&error);
            if ((! zonelists[j]) || (zonesizes[j] <= 0)) {
                apfw_error("reloadSysConfig: sound.%d zone.%d dose not exist",i,j);
                zonesizes[j] = 0;
            }
        }
        for (j = 0; j < sound_length; j++)  {
            zone = &(sound->zone[j]);
            zone->id = j;
            zone->zoneidx = zoneidx++;
            zone->sound = sound;
            if (zonesizes[j] <= 0)  continue;

            zonelist = zonelists[j];
            zone->name = strdup((char*)zonelist[0]);
        }
        for (j = 0; j < sound_length; j++)  {
            int k, l, m, n;
            if (zonesizes[j] <= 1)  continue;
            zonelist = zonelists[j];
            zone = &(sound->zone[j]);
            m = 0;
            for (k = 1; (m < ZONE_MAX_OVERLAP) && (k < (int)zonesizes[j]); k++) {
                for (l = 0; l < sound_length; l++)  {
                    if (l == j) continue;
                    if (strcasecmp((char*)zonelist[k], sound->zone[l].name) == 0)   {
                        for (n = 0; n < m; n++) {
                            if (zone->overlap[n] == sound->zone[l].id)  break;
                        }
                        if (n >= m) {
                            zone->overlap[m++] = sound->zone[l].id;
                        }
                        break;
                    }
                }
                if (l >= sound_length)  {
                    apfw_error("reloadSysConfig: sound.%d zone.%d overlap zone(%s) "
                               "dose not exist", i, j, (char *)zonelist[k]);
                }
            }
            zone->overlapNum = m;
        }
        for (j = 0; j < sound_length; j++)  {
            if (zonelists[j])   g_strfreev(zonelists[j]);
            zone = &(sound->zone[j]);
            apfw_trace("Sound.%d zone.%d(%s) over=%d.%d %d %d %d %d",
                       i, j, zone->name,
                       zone->overlapNum, zone->overlap[0], zone->overlap[1],
                       zone->overlap[2], zone->overlap[3], zone->overlap[4]);
        }
        g_string_free(sound_key,TRUE);
    }
    g_list_free_full(idlist,g_free);

    /* input devices                        */
    idlist = ico_uxf_conf_countNumericalKey(keyfile, "input");
    length = g_list_length(idlist);
    _ico_sys_config->inputdevNum = length;
    _ico_sys_config->inputdev = g_malloc0(sizeof(Ico_Uxf_conf_inputdev) * length);

    for (i = 0; i < (int)length; i++) {
        const char* key = (const char*) g_list_nth_data(idlist, i);
        const char* g = "input";
        Ico_Uxf_conf_inputdev *inputdev = &_ico_sys_config->inputdev[i];
        inputdev->id = ico_uxf_conf_getUint(key);
        ico_uxf_conf_checkGerror(&error);
        inputdev->name = g_key_file_get_string(keyfile, g, key, &error);
        ico_uxf_conf_checkGerror(&error);

        /* input sw             */
        int sw_length;
        GString* sw_key = g_string_new("");
        for (sw_length = 0;; sw_length++)   {
            g_string_printf(sw_key,"%s.sw.%d",key,sw_length);
            if (g_key_file_has_key(keyfile,g,sw_key->str,NULL) != TRUE)  break;
        }
        apfw_trace("Input.%d swNum = %d" ,i, sw_length);
        inputdev->inputswNum = sw_length;
        inputdev->inputsw = g_malloc0(sizeof(Ico_Uxf_conf_inputsw)*sw_length);

        /* input sw         */
        gchar **swlist;
        gsize   swsize;
        for (j = 0; j < sw_length; j++) {
            g_string_printf(sw_key,"%s.sw.%d",key,j);
            swlist = g_key_file_get_string_list(keyfile,g,sw_key->str,&swsize,&error);
            ico_uxf_conf_checkGerror(&error);
            if ((!swlist) || (swsize <= 0))  {
                apfw_error("reloadSysConfig: input.%d sw.%d dose not exist",i,j);
                swsize = 0;
            }
            else    {
                inputdev->inputsw[j].inputdev = inputdev;
                inputdev->inputsw[j].name = strdup((char *)swlist[0]);
                if (swsize > 1) {
                    inputdev->inputsw[j].appid = strdup((char *)swlist[1]);
                    if (swsize > 2) {
                        inputdev->inputsw[j].keycode = strtol((char *)swlist[2],
                                                              (char **)0, 0);
                    }
                }
            }
            g_strfreev(swlist);
            uifw_trace("Input %s.%s app=%s key=%d",
                       inputdev->name,
                       inputdev->inputsw[j].name ? inputdev->inputsw[j].name : " ",
                       inputdev->inputsw[j].appid ? inputdev->inputsw[j].appid : " ",
                       inputdev->inputsw[j].keycode);
        }
        g_string_free(sw_key,TRUE);
    }
    g_list_free_full(idlist,g_free);

    _ico_sys_config->misc.default_kindId = ico_uxf_getSysKindByName(
                        g_key_file_get_string(keyfile, "default", "kind", &error));
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.default_categoryId = ico_uxf_getSysCategoryByName(
                        g_key_file_get_string(keyfile, "default", "category", &error));
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.default_displayId = ico_uxf_getSysDisplayByName(
                        g_key_file_get_string(keyfile, "default", "display", &error));
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.default_layerId = ico_uxf_getSysLayerByName(
                        _ico_sys_config->misc.default_displayId,
                        g_key_file_get_string(keyfile, "default", "layer", &error));
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.default_dispzoneId = ico_uxf_getSysDispZoneByName(
                        _ico_sys_config->misc.default_displayId,
                        g_key_file_get_string(keyfile, "default", "displayzone", &error));
    _ico_sys_config->misc.default_soundId = ico_uxf_getSysSoundByName(
                        g_key_file_get_string(keyfile, "default", "sound", &error));
    _ico_sys_config->misc.default_soundzoneId = ico_uxf_getSysSoundZoneByName(
                        _ico_sys_config->misc.default_soundId,
                        g_key_file_get_string(keyfile, "default", "soundzone", &error));
    ico_uxf_conf_checkGerror(&error);
    _ico_sys_config->misc.default_inputdevId = ico_uxf_getSysInputdevByName(
                        g_key_file_get_string(keyfile, "default", "inputdev", &error));
    _ico_sys_config->misc.default_inputswId = ico_uxf_getSysInputswByName(
                        _ico_sys_config->misc.default_inputdevId,
                        g_key_file_get_string(keyfile, "default", "inputsw", &error));
    ico_uxf_conf_checkGerror(&error);

    apfw_trace("Default host=%d kind=%d category=%d disp=%d layer=%d dispzone=%d "
               "sound=%d soundzone=%d input=%d/%d",
               _ico_sys_config->misc.default_hostId,
               _ico_sys_config->misc.default_kindId,
               _ico_sys_config->misc.default_categoryId,
               _ico_sys_config->misc.default_displayId,
               _ico_sys_config->misc.default_layerId,
               _ico_sys_config->misc.default_dispzoneId,
               _ico_sys_config->misc.default_soundId,
               _ico_sys_config->misc.default_soundzoneId,
               _ico_sys_config->misc.default_inputdevId,
               _ico_sys_config->misc.default_inputswId);
    g_key_file_free(keyfile);

    apfw_trace("reloadSysConfig: Leave");
    return _ico_sys_config;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysHostById: get host configuration by host Id
 *
 * @param[in]   hostId      host Id
 * @return      host configuration table
 * @retval      != NULL         success(host configuration table address)
 * @retval      == NULL         error(host Id dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_conf_host *
ico_uxf_getSysHostById(const int hostId)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }

    for (i = 0; i < _ico_sys_config->hostNum; i++) {
        if (_ico_sys_config->host[i].id == hostId) {
            return &_ico_sys_config->host[i];
        }
    }
    apfw_warn("ico_uxf_getSysHostById(%d) Illegal Host Number", hostId);
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_get_SysLocation: get host location by host Id
 *
 * @param[in]   hostId      host Id
 * @return      host location name
 * @retval      != NULL         success(host location name)
 * @retval      == NULL         error(host Id dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const char *
ico_uxf_get_SysLocation(const int hostId)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }

    for (i = 0; i < _ico_sys_config->hostNum; i++) {
        if (_ico_sys_config->host[i].id == hostId) {
            return _ico_sys_config->host[i].address;
        }
    }
    apfw_warn("Ico_uxf_get_SysLocation(%d) Illegal Host Number", hostId);
    return "127.0.0.1";
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysDisplayById: get display configuration by display Id
 *
 * @param[in]   displayId       display Id
 * @return      display configuration table address
 * @retval      != NULL         success(host oy configuration tabel address)
 * @retval      == NULL         error(displayd dose not exist)
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API const Ico_Uxf_conf_display *
ico_uxf_getSysDisplayById(const int displayId)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }

    for (i = 0; i < _ico_sys_config->displayNum; i++)  {
        if (_ico_sys_config->display[i].id == displayId)   {
            return &_ico_sys_config->display[i];
        }
    }
    apfw_warn("ico_uxf_getSysDisplayById(%d) Illegal Display Number", displayId);
    return NULL;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysHostByName: get host Id bu host name
 *
 * @param[in]   shost       host name
 * @return      host Id number
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysHostByName(const char *shost)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if (_ico_sys_config->hostNum <= 0)  {
        apfw_critical("ico_uxf_getSysHostByName: can not read configuration file");
        return _ico_sys_config->misc.default_hostId;
    }

    if ((! shost) || (*shost == 0))    {
        return _ico_sys_config->host[0].id;
    }

    for (i = 0; i < _ico_sys_config->hostNum; i++) {
        if (strcasecmp(shost, _ico_sys_config->host[i].name) == 0)    {
            return _ico_sys_config->host[i].id;
        }
    }
    apfw_warn("ico_uxf_getSysHostByName(%s) Illegal Host Name", shost);
    return _ico_sys_config->misc.default_hostId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysHostTypeByName: get host type number by name(internal function)
 *
 * @param[in]   stype       host type name
 * @return      host type number
 */
/*--------------------------------------------------------------------------*/
int
ico_uxf_getSysHostTypeByName(const char *stype)
{
    if ((! stype) || (*stype == 0))    {
        return ICO_UXF_HOSTTYPE_CENTER;
    }
    if (strcasecmp(stype, "center") == 0)    {
        return ICO_UXF_HOSTTYPE_CENTER;
    }
    else if (strcasecmp(stype, "meter") == 0)    {
        return ICO_UXF_HOSTTYPE_METER;
    }
    else if (strcasecmp(stype, "remote") == 0)   {
        return ICO_UXF_HOSTTYPE_REMOTE;
    }
    else if (strcasecmp(stype, "passenger") == 0)    {
        return ICO_UXF_HOSTTYPE_PASSENGER;
    }
    else if (strcasecmp(stype, "rear") == 0) {
        return ICO_UXF_HOSTTYPE_REAR;
    }
    else if (strcasecmp(stype, "rearleft") == 0) {
        return ICO_UXF_HOSTTYPE_REARLEFT;
    }
    else if (strcasecmp(stype, "rearright") == 0)    {
        return ICO_UXF_HOSTTYPE_REARRIGHT;
    }
    apfw_warn("ico_uxf_getSysHostTypeByName(%s) Illegal HostType Name", stype);
    return strtol(stype, (char **)0, 0);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysDispTypeByName: get display type number by name(internal function)
 *
 * @param[in]   stype       display type name
 * @return      display type number
 */
/*--------------------------------------------------------------------------*/
int
ico_uxf_getSysDispTypeByName(const char *stype)
{
    if ((! stype) || (*stype == 0))    {
        apfw_critical("ico_uxf_getSysDispTypeByName(NULL) Illegal HostType Name");
        return 0;
    }
    if (strcasecmp(stype, "center") == 0)    {
        return ICO_UXF_DISPLAYTYPE_CENTER;
    }
    else if (strcasecmp(stype, "meter") == 0)    {
        return ICO_UXF_DISPLAYTYPE_METER;
    }
    else if (strcasecmp(stype, "remote") == 0)   {
        return ICO_UXF_DISPLAYTYPE_REMOTE;
    }
    else if (strcasecmp(stype, "passenger") == 0)    {
        return ICO_UXF_DISPLAYTYPE_PASSENGER;
    }
    else if (strcasecmp(stype, "rear") == 0) {
        return ICO_UXF_DISPLAYTYPE_REAR;
    }
    else if (strcasecmp(stype, "rearleft") == 0) {
        return ICO_UXF_DISPLAYTYPE_REARLEFT;
    }
    else if (strcasecmp(stype, "rearright") == 0)    {
        return ICO_UXF_DISPLAYTYPE_REARRIGHT;
    }
    apfw_warn("ico_uxf_getSysDispTypeByName(%s) Illegal DisplayType Name", stype);
    return strtol(stype, (char **)0, 0);
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysBoolByName: get boolian value(internal function)
 *
 * @param[in]   sbool       boolian value string
 * @return      boolian value
 * @retval      = 0             false
 * @retval      = 1             true
 */
/*--------------------------------------------------------------------------*/
int
ico_uxf_getSysBoolByName(const char *sbool)
{
    if ((! sbool) || (*sbool == 0))    {
        return 0;
    }
    if ((strcasecmp(sbool, "true") == 0) ||
        (strcasecmp(sbool, "yes") == 0) ||
        (strcasecmp(sbool, "1") == 0))  {
        return 1;
    }
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysDisplayByName: get display Id by display name
 *
 * @param[in]   sdisplay        display name
 * @return      display Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysDisplayByName(const char *sdisplay)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if ((! sdisplay) || (*sdisplay == 0))  {
        apfw_trace("ico_uxf_getSysDisplayByName(NULL) Illegal Display Name");
        return _ico_sys_config->misc.default_displayId;
    }

    for (i = 0; i < _ico_sys_config->displayNum; i++) {
        if (strcasecmp(sdisplay, _ico_sys_config->display[i].name) == 0)    {
            return _ico_sys_config->display[i].id;
        }
    }
    apfw_warn("ico_uxf_getSysDisplayByName(%s) Illegal Display Name", sdisplay);
    return _ico_sys_config->misc.default_displayId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysLayerByName: get layer Id by layer name
 *
 * @param[in]   display     display Id
 * @param[in]   slayer      layer name
 * @return      layer Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysLayerByName(const int display, const char *slayer)
{
    int     i;
    char    *errp;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }

    if ((display < 0) || (display >= _ico_sys_config->displayNum)) {
        apfw_warn("ico_uxf_getSysLayerByName(%d,%s) Illegal Display Number",
                            display, slayer);
        return _ico_sys_config->misc.default_layerId;
    }
    if ((! slayer) || (*slayer == 0))  {
        apfw_trace("ico_uxf_getSysLayerByName(NULL) Illegal Layer Name");
        return _ico_sys_config->misc.default_layerId;
    }

    for (i = 0; i < _ico_sys_config->display[display].layerNum; i++)    {
        if (strcasecmp(slayer, _ico_sys_config->display[display].layer[i].name) == 0)   {
            return _ico_sys_config->display[display].layer[i].id;
        }
    }

    errp = NULL;
    i = strtol(slayer, &errp, 0);
    if ((errp) && (*errp != 0)) {
        apfw_warn("ico_uxf_getSysLayerByName(%d,%s) Illegal Layer Name", display, slayer);
        i = _ico_sys_config->misc.default_layerId;
    }
    return i;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysKindByName: get kind Id by kind name
 *
 * @param[in]   skind       kind name
 * @return      kind Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysKindByName(const char *skind)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if (_ico_sys_config->kindNum <= 0)  {
        apfw_critical("ico_uxf_getSysKindByName: can not read configuration file");
        return _ico_sys_config->misc.default_kindId;
    }

    if ((skind) && (*skind != 0))  {
        for (i = 0; i < _ico_sys_config->kindNum; i++) {
            if (strcasecmp(skind, _ico_sys_config->kind[i].name) == 0)    {
                return _ico_sys_config->kind[i].id;
            }
        }
    }
    apfw_warn("ico_uxf_getSysKindByName(%s) Illegal kind name", skind);
    return _ico_sys_config->misc.default_kindId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysCategoryByName: get category Id by category name
 *
 * @param[in]   scategory   category name
 * @return      category Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysCategoryByName(const char *scategory)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if (_ico_sys_config->categoryNum <= 0)  {
        apfw_critical("ico_uxf_getSysCategoryByName: can not read configuration file");
        return _ico_sys_config->misc.default_categoryId;
    }

    if ((scategory) && (*scategory != 0))  {
        for (i = 0; i < _ico_sys_config->categoryNum; i++) {
            if (strcasecmp(scategory, _ico_sys_config->category[i].name) == 0)    {
                return _ico_sys_config->category[i].id;
            }
        }
    }
    apfw_warn("ico_uxf_getSysCategoryByName(%s) Illegal category name", scategory);
    return _ico_sys_config->misc.default_categoryId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysDispZoneByName: get display zone Id by display zone name
 *
 * @param[in]   display     display Id
 * @param[in]   sdispzone   displahy zone name
 * @return      display zone Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysDispZoneByName(const int display, const char *sdispzone)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if ((display < 0) || (display >= _ico_sys_config->displayNum) ||
        (_ico_sys_config->display[display].zoneNum <= 0))   {
        apfw_critical("ico_uxf_getSysDispZoneByName: can not read configuration file");
        return _ico_sys_config->misc.default_dispzoneId;
    }

    if ((sdispzone) && (*sdispzone != 0))  {
        for (i = 0; i < _ico_sys_config->display[display].zoneNum; i++) {
            if (strcasecmp(sdispzone, _ico_sys_config->display[display].zone[i].name) == 0) {
                return _ico_sys_config->display[display].zone[i].id;
            }
        }
    }
    apfw_warn("ico_uxf_getSysDispZoneByName(%s) Illegal zone name", sdispzone);
    return _ico_sys_config->misc.default_dispzoneId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysSoundByName: get sound Id by sound name
 *
 * @param[in]   ssound      sound name
 * @return      sound Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysSoundByName(const char *ssound)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if (_ico_sys_config->soundNum <= 0) {
        apfw_critical("ico_uxf_getSysSoundByName: can not read configuration file");
        return _ico_sys_config->misc.default_soundId;
    }

    if ((ssound) && (*ssound != 0))    {
        for (i = 0; i < _ico_sys_config->soundNum; i++) {
            if (strcasecmp(ssound, _ico_sys_config->sound[i].name) == 0)    {
                return _ico_sys_config->sound[i].id;
            }
        }
    }
    apfw_warn("ico_uxf_getSysSoundByName(%s) Illegal sound name", ssound);
    return _ico_sys_config->misc.default_soundId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysSoundZoneByName: get sound zone Id by sound zone name
 *
 * @param[in]   sound       sound Id
 * @param[in]   ssoundzone  sound zone name
 * @return      sound zone Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysSoundZoneByName(const int sound, const char *ssoundzone)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if ((sound < 0) || (sound >= _ico_sys_config->soundNum) ||
        (_ico_sys_config->sound[sound].zoneNum <= 0))   {
        apfw_critical("ico_uxf_getSysSoundZoneByName: can not read configuration file");
        return _ico_sys_config->misc.default_soundzoneId;
    }

    if ((ssoundzone) && (*ssoundzone != 0))    {
        for (i = 0; i < _ico_sys_config->sound[sound].zoneNum; i++) {
            if (strcasecmp(ssoundzone, _ico_sys_config->sound[sound].zone[i].name) == 0)    {
                return _ico_sys_config->sound[sound].zone[i].id;
            }
        }
    }
    apfw_warn("ico_uxf_getSysSoundZoneByName(%s) Illegal zone name", ssoundzone);
    return _ico_sys_config->misc.default_soundzoneId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysInputdevByName: get input device Id by input device name
 *
 * @param[in]   sinputdev   input device name
 * @return      input device Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysInputdevByName(const char *sinputdev)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if (_ico_sys_config->inputdevNum <= 0)  {
        apfw_critical("ico_uxf_getSysInputdevByName: can not read configuration file");
        return _ico_sys_config->misc.default_inputdevId;
    }

    if ((! sinputdev) || (*sinputdev == 0))    {
        return _ico_sys_config->misc.default_inputdevId;
    }

    for (i = 0; i < _ico_sys_config->inputdevNum; i++) {
        if (strcasecmp(sinputdev, _ico_sys_config->inputdev[i].name) == 0)    {
            return _ico_sys_config->inputdev[i].id;
        }
    }
    apfw_warn("ico_uxf_getSysInputdevByName(%s) Illegal device Name", sinputdev);
    return _ico_sys_config->misc.default_inputdevId;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_getSysInputswByName: get input switch Id by input switch name
 *
 * @param[in]   inputdev    input device Id
 * @param[in]   sinputsw    input switch name
 * @return      input switch Id
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_getSysInputswByName(const int inputdev, const char *sinputsw)
{
    int     i;

    if (!_ico_sys_config) {
        (void)ico_uxf_getSysConfig();
    }
    if ((inputdev < 0) || (inputdev >= _ico_sys_config->inputdevNum) ||
        (_ico_sys_config->inputdev[inputdev].inputswNum <= 0))  {
        apfw_critical("ico_uxf_getSysInputswByName: can not read configuration file");
        return 0;
    }

    if ((sinputsw) && (*sinputsw != 0)) {
        for (i = 0; i < _ico_sys_config->inputdev[inputdev].inputswNum; i++)    {
            if (strcasecmp(sinputsw, _ico_sys_config->inputdev[inputdev].inputsw[i].name)
                    == 0)    {
                return i;
            }
        }
    }
    apfw_warn("ico_uxf_getSysInputswByName(%s) Illegal sw name",
              sinputsw ? sinputsw : "(NULL)");
    return 0;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_closeSysConfig: close and release system configuration table
 *
 * @param       none
 * @return      none
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API void
ico_uxf_closeSysConfig(void)
{
    if (_ico_sys_config != NULL)    {
        g_free(_ico_sys_config->host);
        g_free(_ico_sys_config->display);
        g_free(_ico_sys_config);
        _ico_sys_config = NULL;
    }
}

