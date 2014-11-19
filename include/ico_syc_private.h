/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   header file of System Controller internal definition
 *
 * @date    Sep-4-2013
 */

#ifndef _ICO_SYC_PRIVATE_H_
#define _ICO_SYC_PRIVATE_H_

#include <stdio.h>
#include <stdlib.h>

#include <ico_uws.h>

#include "ico_syc_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*/
/* definition                                                                 */
/*============================================================================*/
/*
 * uri and protocol
 */
#define ICO_SYC_URI             (char *)"ws://127.0.0.1:18081"
#define ICO_SYC_PROTOCOL        (char *)"ico_syc_protocol"

#define ICO_SYC_MAX_LEN         128

/*
 * package name
 */
#define ICO_SYC_PACKAGE_SYSTEMCONTROLLER    "org.tizen.ico.system-controller"
#define ICO_SYC_PACKAGE_HOMESCREEN          "org.tizen.ico.homescreen"
#define ICO_SYC_PACKAGE_STATUSBAR           "org.tizen.ico.statusbar"
#define ICO_SYC_PACKAGE_ONSCREEN            "org.tizen.ico.onscreen"
#define ICO_SYC_PACKAGE_LOGIN               "org.tizen.ico.login"
#define ICO_SYC_PACKAGE_DIALER              "org.tizen.dialer"

/*
 * configuration file path
 */
#define ICO_SYC_CONFIGPATH_HOME             "ico"
#define ICO_SYC_CONFIGPATH_HOME_CONFIG      ICO_SYC_CONFIGPATH_HOME "/config"
#define ICO_SYC_CONFIGPATH_HOME_IMAGE       ICO_SYC_CONFIGPATH_HOME "/images"
#define ICO_SYC_CONFIGPATH_HOME_SOUND       ICO_SYC_CONFIGPATH_HOME "/sound"
#define ICO_SYC_CONFIGPATH_PACKAGE          "res"
#define ICO_SYC_CONFIGPATH_PACKAGE_CONFIG   ICO_SYC_CONFIGPATH_PACKAGE "/config"
#define ICO_SYC_CONFIGPATH_PACKAGE_IMAGE    ICO_SYC_CONFIGPATH_PACKAGE "/images"
#define ICO_SYC_CONFIGPATH_PACKAGE_SOUND    ICO_SYC_CONFIGPATH_PACKAGE "/sound"

/*============================================================================*/
/* variable & table                                                           */
/*============================================================================*/
struct ico_syc_res_context {
    char                    appid[ICO_SYC_MAX_LEN];
    /* window resource */
    ico_syc_res_window_t    *window;
    /* sound resource */
    ico_syc_res_sound_t     *sound;
    /* input resource */
    ico_syc_res_input_t     *input;

    /* window and sound resource type (basic or interruption) */
    int                     type;
};

/*============================================================================*/
/* internal function (common)                                                 */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_context
 *          Get the ico_uws context connected to System Controller.
 *
 * @return      uws_context
 * @retval      context                 success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
struct ico_uws_context *ico_syc_get_context(void);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_id
 *          Get the ico_uws id to send message to System Controller.
 *
 * @return      uws_id
 * @retval      id                      success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
void *ico_syc_get_id(void);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_get_appid
 *          Get the application id.
 *
 * @return      appid
 * @retval      appid                   success
 * @retval      NULL                    error
 */
/*--------------------------------------------------------------------------*/
char *ico_syc_get_appid(void);

/*============================================================================*/
/* internal function (winctl)                                                 */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_win
 *          Execute callback function. (ICO_SYC_EV_WIN_ACTIVE
 *                                      ICO_SYC_EV_WIN_CREATE
 *                                      ICO_SYC_EV_WIN_DESTROY)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_win(ico_syc_callback_t callback, void *user_data,
                    int event, const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_win_attr
 *          Execute callback function. (ICO_SYC_EV_WIN_ATTR_CHANGE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_win_attr(ico_syc_callback_t callback, void *user_data,
                         int event, const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_thumb
 *          Execute callback function. (ICO_SYC_EV_THUMB_ERROR
 *                                      ICO_SYC_EV_THUMB_CHANGE
 *                                      ICO_SYC_EV_THUMB_UNMAP)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_thumb(ico_syc_callback_t callback, void *user_data,
                      int event, const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_layer
 *          Execute callback function. (ICO_SYC_EV_LAYER_ATTR_CHANGE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_layer(ico_syc_callback_t callback, void *user_data,
                      int event, const void *data, size_t len);

/*============================================================================*/
/* internal function (userctl)                                                */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_auth
 *          Execute callback function. (ICO_SYC_EV_AUTH_FAIL)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_auth(ico_syc_callback_t callback, void *user_data, int event);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_userlist
 *          Execute callback function. (ICO_SYC_EV_USERLIST)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_userlist(ico_syc_callback_t callback, void *user_data,
                         int event, const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_lastinfo
 *          Execute callback function. (ICO_SYC_EV_LASTINFO)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_lastinfo(ico_syc_callback_t callback, void *user_data,
                         int event, const void *data, size_t len);

/*============================================================================*/
/* internal function (appresctl)                                              */
/*============================================================================*/
/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_res
 *          Execute callback function. (ICO_SYC_EV_RES_ACQUIRE
 *                                      ICO_SYC_EV_RES_DEPRIVE
 *                                      ICO_SYC_EV_RES_WAITING
 *                                      ICO_SYC_EV_RES_REVERT
 *                                      ICO_SYC_EV_RES_RELEASE)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_res(ico_syc_callback_t callback, void *user_data,
                    int event, const void *data, size_t len);

/*--------------------------------------------------------------------------*/
/**
 * @internal
 * @brief   ico_syc_cb_region
 *          Execute callback function. (ICO_SYC_EV_INPUT_SET
 *                                      ICO_SYC_EV_INPUT_UNSET)
 *
 * @param[in]   callback                callback function
 * @param[in]   user_data               passed data on called callback function
 * @param[in]   event                   event code
 * @param[in]   data                    message data
 * @param[in]   len                     length of data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void ico_syc_cb_region(ico_syc_callback_t callback, void *user_data,
                       int event, const void *data, size_t len);


/*============================================================================*/
/* NodeId <-> DisplayId macro                                                 */
/*============================================================================*/
#define ICO_SYC_ECUBASE 100
#define ICO_SYC_DISPLAYID(nodeid)           (nodeid % ICO_SYC_ECUBASE)
#define ICO_SYC_ECUID(nodeid)               (nodeid / ICO_SYC_ECUBASE)
#define ICO_SYC_NODEID(ecuid, displayid)    (ecuid * ICO_SYC_ECUBASE + \
                                             (displayid % ICO_SYC_ECUBASE))

/*============================================================================*/
/* log macro                                                                  */
/*============================================================================*/
#ifndef  _NO_USE_DLOG

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "ICO_SYC"
#include <dlog/dlog.h>

#define _ERR(fmt, arg...)                                       \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc E: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
        LOGE("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg);      \
    } while (0)

#define _WARN(fmt, arg...)                                      \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc W: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
        LOGW("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg);      \
    } while (0)

#define _INFO(fmt, arg...)                                      \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc I: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
        LOGI("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg);      \
    } while (0)

#define _DBG(fmt, arg...)                                       \
    do {                                                        \
        LOGD("%s:%d " fmt, __FUNCTION__, __LINE__, ##arg);      \
    } while (0)

#else

#define _ERR(fmt, arg...)                                       \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc E: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
    } while (0)

#define _WARN(fmt, arg...)                                      \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc W: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
    } while (0)


#define _INFO(fmt, arg...)                                      \
    do {                                                        \
        fprintf(stderr,                                         \
                "ico_syc I: %s:%d [ "fmt" ]\n",                 \
                __FUNCTION__,                                   \
                __LINE__,                                       \
                ##arg);                                         \
    } while (0)

#define _DBG(fmt, arg...)                                       \
    do {                                                        \
        if (getenv("ICO_SYC_DEBUG")) {                          \
            fprintf(stderr,                                     \
                    "ico_syc D: %s:%d [ "fmt" ]\n",             \
                    __FUNCTION__,                               \
                    __LINE__,                                   \
                    ##arg);                                     \
        }                                                       \
    } while (0)

#endif


#ifdef __cplusplus
}
#endif
#endif /*_ICO_SYC_PRIVATE_H_*/
/* vim:set expandtab ts=4 sw=4: */
