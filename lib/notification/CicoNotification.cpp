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
 *  @file   CicoNotification.cpp
 *
 *  @brief  This file is implemetation of CicoNotification class
 */
//==========================================================================

#include <ico_log.h>

#include "CicoNotification.h"

//--------------------------------------------------------------------------
/**
 *  @brief  constructor
 *
 *  @param [in] type    type of notification
 */
//--------------------------------------------------------------------------
CicoNotification::CicoNotification(notification_type_e type)
    : m_notification(NULL),
      m_type(type),
      m_privId(NOTIFICATION_PRIV_ID_NONE),
      m_groupId(NOTIFICATION_GROUP_ID_NONE)
{
    // create notfication handler
    m_notification = notification_create(type);
    if (NULL != m_notification) {
        // initialize group id and private id variables
        notification_error_e err = NOTIFICATION_ERROR_NONE;
        err = notification_get_id(m_notification, &m_groupId, &m_privId);
        if (NOTIFICATION_ERROR_NONE != err) {
            ICO_ERR("notification_get_id() failed(%d).", err);
            m_privId  = NOTIFICATION_PRIV_ID_NONE;
            m_groupId = NOTIFICATION_GROUP_ID_NONE;
        }
    }
    else {
        ICO_ERR("notification_create() failed.");
        m_notification = NULL;
        m_type = NOTIFICATION_TYPE_NONE;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  constructor
 *
 *  @param [in] noti    notification handle
 */
//--------------------------------------------------------------------------
CicoNotification::CicoNotification(notification_h noti)
    : m_notification(NULL),
      m_type(NOTIFICATION_TYPE_NONE),
      m_privId(NOTIFICATION_PRIV_ID_NONE),
      m_groupId(NOTIFICATION_GROUP_ID_NONE)
{
    // clone notfication handler
    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_clone(noti, &m_notification);
    if (NOTIFICATION_ERROR_NONE == err) {
        notification_error_e err = NOTIFICATION_ERROR_NONE;

        // initialize type variable
        err = notification_get_type(m_notification, &m_type);
        if (NOTIFICATION_ERROR_NONE != err) {
            ICO_ERR("notification_get_id() failed(%d).", err);
            m_type = NOTIFICATION_TYPE_NONE;
        }

        // initialize group id and private id variables
        err = notification_get_id(m_notification, &m_groupId, &m_privId);
        if (NOTIFICATION_ERROR_NONE != err) {
            ICO_ERR("notification_get_id() failed(%d).", err);
            m_privId  = NOTIFICATION_PRIV_ID_NONE;
            m_groupId = NOTIFICATION_GROUP_ID_NONE;
        }
    }
    else {
        ICO_ERR("notification_clone() failed(%d).", err);
        m_notification = NULL;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoNotification::~CicoNotification()
{
    if (NULL != m_notification) {
        notification_free(m_notification);
        m_notification = NULL;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get notification handle
 *
 *  @return notification handle
 */
//--------------------------------------------------------------------------
notification_h
CicoNotification::GetNotiHandle(void) const
{
    return m_notification;
}

//--------------------------------------------------------------------------
/**
 *  @brief  check empty notification handle
 *
 *  @return empty   state of notification handle
 *  @retval true    notification handle is null
 *  @retval false   notification handle not is null
 */
//--------------------------------------------------------------------------
bool
CicoNotification::Empty(void) const
{
    return (m_notification == NULL) ? true : false;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get type of notification
 *
 *  @return type of notification
 */
//--------------------------------------------------------------------------
notification_type_e
CicoNotification::GetType(void) const
{
    return m_type;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get private id
 *
 *  @return notification private id
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_pkgname() failed
 */
//--------------------------------------------------------------------------
int
CicoNotification::GetPrivId(void)
{
    return m_privId;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set package name
 *
 *  @param [in] pkgname package name
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_pkgname() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::SetPkgname(const char *pkgname)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_set_pkgname(m_notification, pkgname);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_pkgname() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get package name of notification
 *
 *  @return package name
 */
//--------------------------------------------------------------------------
const char *
CicoNotification::GetPkgname(void)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return NULL;
    }

    char *caller_pkgname = NULL;
    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_get_pkgname(m_notification, &caller_pkgname);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_pkgname() failed(%d).", err);
        return NULL;
    }

    return caller_pkgname;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set title of notification
 *
 *  @param [in] title   path of icon image
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_text() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::SetTitle(const char *title)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_set_text(m_notification,
                                NOTIFICATION_TEXT_TYPE_TITLE,
                                title,
                                NULL,
                                NOTIFICATION_VARIABLE_TYPE_NONE);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_set_text() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get title of notification
 *
 *  @return text of title
 */
//--------------------------------------------------------------------------
const char *
CicoNotification::GetTitle(void)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return NULL;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    char *title = NULL;
    err = notification_get_text(m_notification,
                                NOTIFICATION_TEXT_TYPE_TITLE,
                                &title);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_text() failed(%d).", err);
        return NULL;
    }

    return title;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set content of notification
 *
 *  @param [in] content path of icon image
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_text() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::SetContent(const char *content)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_set_text(m_notification,
                                NOTIFICATION_TEXT_TYPE_CONTENT,
                                content,
                                NULL,
                                NOTIFICATION_VARIABLE_TYPE_NONE);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_set_text() failed(%d).", err);
        return false;
    }
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get content of notification
 *
 *  @return text of content
 */
//--------------------------------------------------------------------------
const char *
CicoNotification::GetContent(void)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return NULL;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    char *content = NULL;
    err = notification_get_text(m_notification,
                                NOTIFICATION_TEXT_TYPE_CONTENT,
                                &content);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_text() failed(%d).", err);
        return NULL;
    }

    return content;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set icon path of notification
 *
 *  @param [in] path    path of icon image
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_icon() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::SetIconPath(const char *path)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_set_icon(m_notification, path);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_set_icon() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get icon path of notification
 *
 *  @return path of icon image
 */
//--------------------------------------------------------------------------
const char *
CicoNotification::GetIconPath(void)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null.");
        return NULL;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    char *iconpath = NULL;
    err = notification_get_icon(m_notification, &iconpath);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_icon() failed(%d).", err);
        return NULL;
    }

    return iconpath;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set execute option
 *
 *  @param [in] type            notification execute type
 *  @param [in] text            basic text for button
 *  @param [in] key             value for localizaed text
 *  @param [in] service_handle  appsvc bundle data
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_set_execute_option() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::SetExecuteOption(notification_execute_type_e type,
                                   const char *text,
                                   const char *key,
                                   bundle *service_handle)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_set_execute_option(m_notification, type,
                                          text, key, service_handle);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_execute_option() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get execute option
 *
 *  @param [in]  type           notification execute type
 *  @param [out] text           text for button
 *  @param [out] service_handle appsvc bundle data
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_get_execute_option() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotification::GetExecuteOption(notification_execute_type_e type,
                                   const char **text,
                                   bundle **service_handle)
{
    if (NULL == m_notification) {
        ICO_ERR("notification handle is null");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_get_execute_option(m_notification, type,
                                          text, service_handle);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_get_execute_option() failed(%d).", err);
        return false;
    }

    return true;
}
// vim:set expandtab ts=4 sw=4:
