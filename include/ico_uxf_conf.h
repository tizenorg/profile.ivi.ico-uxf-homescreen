/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library public header file(configuration)
 *
 * @date    Feb-28-2013
 */

#ifndef ICO_UXF_CONF_H_
#define ICO_UXF_CONF_H_

#include <glib.h>
#include "ico_uxf_conf_def.h"
#include "ico_uxf_sysdef.h"
#include "ico_uxf_def.h"
#include "ico_uxf_typedef.h"
#include "ico_apf_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _ico_uxf_conf_misc {
    char topdir[ICO_UXF_CONF_DIR_MAX+1];
    char confdir[ICO_UXF_CONF_DIR_MAX+1];
    short myhostId;
    short masterhostId;
    char *myhostName;
    char *myaddress;
    char *homescreen;
    char *masterhostaddress;
    short default_hostId;
    short default_kindId;
    short default_categoryId;
    short default_displayId;
    short default_layerId;
    short default_dispzoneId;
    short default_soundId;
    short default_soundzoneId;
    short default_inputdevId;
    short default_inputswId;
    int appscontrollerport;
    int soundpluginport;
    int loglevel;
    int logflush;
} Ico_Uxf_conf_misc;

typedef struct _ico_uxf_conf_host {
    int id;
    char *name;
    char *address;
    int type;
    char *homescreen;
} Ico_Uxf_conf_host;

typedef struct _ico_uxf_conf_category {
    int id;
    char *name;
    short view;
    short type;
    short sound;
    short input;
    short priority;
    short res;                                  /* (unused)         */
} Ico_Uxf_conf_category;

typedef struct _ico_uxf_conf_kind {
    int id;
    char *name;
    short priv;
    short priority;
} Ico_Uxf_conf_kind;

typedef struct _ico_uxf_conf_appdisplay {
    short displayId;
    short layerId;
    short zoneId;
    short res;                                  /* (unused)         */
} Ico_Uxf_conf_appdisplay;

typedef struct _ico_uxf_conf_appsound {
    short soundId;
    short zoneId;
} Ico_Uxf_conf_appsound;

typedef struct _ico_uxf_conf_appinput {
    short inputdevId;
    short inputswId;
} Ico_Uxf_conf_appinput;

typedef struct _ico_uxf_conf_application {
    char *appid;
    char *name;
    char *group;
    char *exec;
    char *type;
    char *animation_visible;
    char *animation_resize;
    char *animation_move;
    int hostId;
    char *location;
    char *icon_key_name;
    char autostart;
    char noicon;
    char noconfigure;
    char res;
    unsigned short invisiblecpu;
    short kindId;
    short categoryId;
    short displayzoneNum;
    short soundzoneNum;
    short inputdevNum;
    Ico_Uxf_conf_appdisplay display[ICO_UXF_APPDISPLAY_MAX];
    Ico_Uxf_conf_appsound   sound[ICO_UXF_APPSOUND_MAX];
    Ico_Uxf_conf_appinput   input[ICO_UXF_APPINPUT_MAX];
    struct _ico_uxf_conf_application *nextidhash;
    struct _ico_uxf_conf_application *nextnamehash;
} Ico_Uxf_conf_application;

typedef struct _ico_uxf_conf_display_layer {
    int id;
    char *name;
} Ico_Uxf_conf_display_layer;

#define ZONE_MAX_OVERLAP    (ICO_UXF_DISPLAY_ZONE_MAX-1)
typedef struct _ico_uxf_conf_display_zone {
    int id;
    char *name;
    short x;
    short y;
    short width;
    short height;
    short zoneidx;
    short res;                                  /* (unused)         */
    struct _ico_uxf_conf_display *display;
    unsigned char overlapNum;
    unsigned char overlap[ZONE_MAX_OVERLAP];
} Ico_Uxf_conf_display_zone;

typedef struct _ico_uxf_conf_display {
    int id;
    char *name;
    short hostId;
    short displayno;
    short type;
    short width;
    short height;
    short inch;
    short layerNum;
    short zoneNum;
    char *wayland;
    Ico_Uxf_conf_display_layer *layer;
    Ico_Uxf_conf_display_zone *zone;
} Ico_Uxf_conf_display;

typedef struct _ico_uxf_conf_sound_zone {
    int id;
    char *name;
    short zoneidx;
    short res;                                  /* (unused)         */
    struct _ico_uxf_conf_sound *sound;
    unsigned char overlapNum;
    unsigned char overlap[ZONE_MAX_OVERLAP];
} Ico_Uxf_conf_sound_zone;

typedef struct _ico_uxf_conf_sound {
    int id;
    char *name;
    short hostId;
    short soundno;
    short type;
    short zoneNum;
    Ico_Uxf_conf_sound_zone *zone;
} Ico_Uxf_conf_sound;

typedef struct _ico_uxf_conf_inputsw {
    struct _ico_uxf_conf_inputdev   *inputdev;
    char *name;
    char *appid;
} Ico_Uxf_conf_inputsw;

typedef struct _ico_uxf_conf_inputdev {
    int id;
    char *name;
    short inputswNum;
    short res;                                  /* (unused)         */
    Ico_Uxf_conf_inputsw *inputsw;
} Ico_Uxf_conf_inputdev;

typedef struct _ico_uxf_sys_config {
    Ico_Uxf_conf_misc misc;
    short hostNum;
    short categoryNum;
    short kindNum;
    short displayNum;
    short soundNum;
    short inputdevNum;
    Ico_Uxf_conf_host *host;
    Ico_Uxf_conf_category *category;
    Ico_Uxf_conf_kind *kind;
    Ico_Uxf_conf_display *display;
    Ico_Uxf_conf_sound *sound;
    Ico_Uxf_conf_inputdev *inputdev;
} Ico_Uxf_Sys_Config;

typedef struct _ico_uxf_app_config {
    int applicationNum;
    int ailNum;
    Ico_Uxf_conf_application *application;
    Ico_Uxf_conf_application *hashidtable[ICO_UXF_MISC_HASHSIZE];
    Ico_Uxf_conf_application *hashnametable[ICO_UXF_MISC_HASHSIZE];
} Ico_Uxf_App_Config;

typedef void (*Ico_Uxf_AppUpdata_Cb)(const char *appid, int type);

const Ico_Uxf_Sys_Config* ico_uxf_getSysConfig(void);
const Ico_Uxf_Sys_Config* ico_uxf_ifGetSysConfig(void);
const Ico_Uxf_App_Config* ico_uxf_getAppConfig(void);

void ico_uxf_closeSysConfig(void);
void ico_uxf_closeAppConfig(void);

int ico_uxf_getSysKindByName(const char *skind);
int ico_uxf_getSysCategoryByName(const char *scategory);
int ico_uxf_getSysHostByName(const char *shost);
int ico_uxf_getSysHostTypeByName(const char *stype);
int ico_uxf_getSysDisplayByName(const char *sdisplay);
int ico_uxf_getSysDispTypeByName(const char *stype);
int ico_uxf_getSysLayerByName(const int display, const char *slayer);
int ico_uxf_getSysDispZoneByName(const int display, const char *sdispzone);
int ico_uxf_getSysSoundByName(const char *ssound);
int ico_uxf_getSysSoundZoneByName(const int sound, const char *ssoundzone);
int ico_uxf_getSysInputdevByName(const char *sinputdev);
int ico_uxf_getSysInputswByName(const int inputdev, const char *sinputsw);
int ico_uxf_getSysBoolByName(const char *sbool);
int ico_uxf_getAppDisplay(const Ico_Uxf_conf_application *app, const int idx,
                          int *x, int *y, int *width, int *height);

const Ico_Uxf_conf_host* ico_uxf_getSysHostById(const int hostId);
const Ico_Uxf_conf_application* ico_uxf_getAppByAppid(const char *appid);
const Ico_Uxf_conf_application* ico_uxf_getAppByName(const char *name);
const Ico_Uxf_conf_category* ico_uxf_getAppCategoryById(const int categoryId);
const Ico_Uxf_conf_display* ico_uxf_getSysDisplayById(const int displayId);
const char *ico_uxf_get_SysLocation( const int hostId );

int ico_uxf_conf_setAppUpdateCb(Ico_Uxf_AppUpdata_Cb func);

/* define log macros    */
#ifndef uifw_trace
#define uifw_debug      ICO_UXF_DEBUG
#define uifw_trace      ICO_UXF_DEBUG
#define uifw_critical   ICO_UXF_CRITICAL
#define uifw_info       ICO_UXF_INFO
#define uifw_warn       ICO_UXF_WARN
#define uifw_error      ICO_UXF_ERROR
#define uifw_logflush   ico_uxf_log_flush
#endif  /*uifw_trace*/

void ico_uxf_closeSysConfig(void);

#define ICO_UXF_CONF_EVENT_INSTALL      (0)
#define ICO_UXF_CONF_EVENT_UNINSTALL    (1)

#ifdef __cplusplus
}
#endif
#endif /* ICO_UXF_CONF_H_ */

