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
 *  @file   CicoNotificationService.cpp
 *
 *  @brief  This file is implemetation of CicoNotificationService class
 */
//==========================================================================

#include <ico_log.h>

#include "CicoNotificationService.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoNotificationService::CicoNotificationService()
    : m_privateid(0),
      m_user_data(NULL)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoNotificationService::~CicoNotificationService()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  add notification
 *
 *  @param [in] ico notification handle
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification_insert() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotificationService::Add(CicoNotification noti)
{
    if (true == noti.Empty()) {
        ICO_WRN("notification handle is null.");
        return false;
    }

    int retid = 0;
    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_insert(noti.GetNotiHandle(), &retid);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_insert() failed(%d).", err);
        return false;
    }
    m_privateid = retid;
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  update notification
 *
 *  @param [in] ico notification handle
 */
//--------------------------------------------------------------------------
bool
CicoNotificationService::Update(CicoNotification noti)
{
    if ((true == noti.Empty()) || (0 == m_privateid)) {
        ICO_WRN("notification handle is null. or not add notification.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_update(noti.GetNotiHandle());
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_update() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  delete notification
 *
 *  @param [in] ico notification handle
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification is not add yet or
 *                      notification_update() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotificationService::Delete(CicoNotification noti)
{
    if ((true == noti.Empty()) || (0 == m_privateid)) {
        ICO_WRN("notification handle is null. or not add notification.");
        return false;
    }

    const char *pkgname = noti.GetPkgname();
    if (pkgname == NULL) {
        return false;
    }

    notification_error_e err = notification_delete_by_priv_id(pkgname,
                                                              noti.GetType(),
                                                              m_privateid);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_delete_by_priv_id() failed(%d).", err);
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  set callback function of notification
 *
 *  @param [in] detailed_changed_cb callback function
 *  @param [in] user_data           user data
 *
 *  @return result
 *  @retval true    success
 *  @retval false   callback function is already register or
 *                  notification_register_detailed_changed_cb failed
 */
//--------------------------------------------------------------------------
bool
CicoNotificationService::SetCallback(void (*detailed_changed_cb)
                                     (void *data,
                                      notification_type_e type,
                                      notification_op *op_list,
                                      int num_op),
                                     void *user_data)
{
    if (NULL != m_user_data) {
        ICO_WRN("callback function is already register.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_register_detailed_changed_cb(detailed_changed_cb,
                                                    user_data);
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_register_detailed_changed_cb() failed(%d).", err);
        return false;
    }

    m_user_data = user_data;
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  unset callback function of notification
 *
 *  @param [in] detailed_changed_cb callback function
 *  @param [in] user_data           user data
 *
 *  @return result
 *  @retval true    success
 *  @retval false   callback function is not register or
 *                  notification_unregister_detailed_changed_cb failed
 */
//--------------------------------------------------------------------------
bool
CicoNotificationService::UnsetCallback(void (*detailed_changed_cb)
                                       (void *data,
                                        notification_type_e type,
                                        notification_op *op_list,
                                        int num_op))
{
    if (NULL != m_user_data) {
        ICO_WRN("callback function is not register.");
        return false;
    }

    notification_error_e err = NOTIFICATION_ERROR_NONE;
    err = notification_unregister_detailed_changed_cb(detailed_changed_cb,
                                                      m_user_data);
    m_user_data = NULL;
    if (NOTIFICATION_ERROR_NONE != err) {
        ICO_ERR("notification_unregister_detailed_changed_cb() failed(%d).",
                err);
        return false;
    }

    return true;
}
// vim:set expandtab ts=4 sw=4:
