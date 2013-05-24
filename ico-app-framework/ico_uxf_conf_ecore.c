/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   user experiance library(config's callback to Ecore(EFL) application)
 *
 * @date    May-17-2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>

#include "ico_apf_log.h"
#include "ico_apf_apimacro.h"
#include "ico_uxf_conf_common.h"
#include "ico_uxf_conf_ecore.h"
#include "ico_uxf_private.h"

/*==============================================================================*/
/* define                                                                       */
/*==============================================================================*/
typedef struct _econf_pkgmgr_event econf_pkgmgr_event_t;
typedef struct _econf_cb_func_info econf_cb_func_info_t;

struct _econf_pkgmgr_event {
    char appid[ICO_UXF_MAX_PROCESS_NAME];
    int type;
};

struct _econf_cb_func_info {
    econf_cb_func_info_t *next;
    Ico_Uxf_AppUpdata_Cb func;
};
/*==============================================================================*/
/* define static function prototype                                             */
/*==============================================================================*/
static void ico_uxf_econf_updateEvent(const char *appid, int type);
static Eina_Bool ico_uxf_econf_ecoreEvent(void *data, int type, void *event);

/*==============================================================================*/
/* static tables                                                                */
/*==============================================================================*/
static int                  econf_event_type = -1;
static Ecore_Event_Handler   *econf_handle = NULL;

static econf_cb_func_info_t *econf_cb_func = NULL;

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_econf_setAppUpdateCb:
 *
 * @param       none
 * @return      result
 * @retval      ICO_UXF_EOK     success
 * @retval      ICO_UXF_ENOSYS  cannot regist callback
 */
/*--------------------------------------------------------------------------*/
ICO_APF_API int
ico_uxf_econf_setAppUpdateCb(Ico_Uxf_AppUpdata_Cb func)
{
    econf_cb_func_info_t *func_info;

    if (econf_handle == NULL) {
        econf_event_type = ecore_event_type_new();

        econf_handle = ecore_event_handler_add(econf_event_type,
                                ico_uxf_econf_ecoreEvent, NULL);
        if (econf_handle == NULL) {
            apfw_warn("ico_uxf_setAppUpdateCb: cannot add ecore event handler");
            return ICO_UXF_ENOSYS;
        }
        ico_uxf_conf_setAppUpdateCb(ico_uxf_econf_updateEvent);
    }
    if (func) {
        func_info = malloc(sizeof(econf_cb_func_info_t));
        if (! func_info) {
            apfw_warn("ico_uxf_setAppUpdateCb: cannot allocate memory");
            return ICO_UXF_ENOSYS;
        }
        memset(func_info, 0, sizeof(econf_cb_func_info_t));
        func_info->func = func;
        if (! econf_cb_func) {
            econf_cb_func = func_info;
        }
        else {
            func_info->next = econf_cb_func;
            econf_cb_func = func_info;
        }
    }

    return ICO_UXF_EOK;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_econf_updateEvent:
 *          pass configuration change event to ecore event.
 *
 * @param[in]   data            data
 * @param[in]   type            type
 * @return      none
 */
/*--------------------------------------------------------------------------*/
static void
ico_uxf_econf_updateEvent(const char *appid, int type)
{
    econf_pkgmgr_event_t *conf_event;

    apfw_trace("ico_uxf_econf_updateEvent: Enter(appid=%s, type=%d)", appid, type);

    conf_event = malloc(sizeof(econf_pkgmgr_event_t));
    if (!conf_event) {
        apfw_warn("ico_uxf_econf_updateEvent: cannot allocate memory");
        return;
    }

    strncpy(conf_event->appid, appid, ICO_UXF_MAX_PROCESS_NAME);
    conf_event->type = type;

    ecore_event_add(econf_event_type, conf_event, NULL, NULL);

    apfw_trace("ico_uxf_econf_updateEvent: Leave");

    return;
}

/*--------------------------------------------------------------------------*/
/**
 * @brief   ico_uxf_econf_ecoreEvent:
 *          callback function from ecore when change app configuration
 *
 * @param[in]   data            data
 * @param[in]   type            type
 * @param[in]   event           event
 * @return      always ECORE_CALLBACK_PASS_ON
 */
/*--------------------------------------------------------------------------*/
static Eina_Bool
ico_uxf_econf_ecoreEvent(void *data, int type, void *event)
{
    econf_cb_func_info_t *func_info;
    econf_pkgmgr_event_t *conf_event = (econf_pkgmgr_event_t *)event;
    apfw_trace("ico_uxf_econf_ecoreEvent: Enter(ev=%08x)", (unsigned int)event);

    if (conf_event) {
        ico_uxf_update_procwin(conf_event->appid, conf_event->type);
        func_info = econf_cb_func;
        while (func_info) {
            if (func_info->func) {
                apfw_trace("ico_uxf_econf_ecoreEvent: func=%08x", (unsigned int)func_info->func);
                func_info->func(conf_event->appid, conf_event->type);
            }
            func_info = func_info->next;
        }
    }

    apfw_trace("ico_uxf_econf_ecoreEvent: Leave");

    return ECORE_CALLBACK_DONE;
}

