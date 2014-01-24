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
 *  @file   CicoNotificationApp.cpp
 *
 *  @brief  This file is implemetation of CicoNotificationApp class
 */
//==========================================================================

#include <ico_log.h>

#include "CicoNotificationApp.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoNotificationApp::CicoNotificationApp()
    : m_privateid(0)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoNotificationApp::~CicoNotificationApp()
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
CicoNotificationApp::Add(CicoNotification noti)
{
    if (true == noti.Empty()) {
        ICO_WRN("notification handle is null.");
        return false;
    }

    int retid = 0;
    notification_error_e err = notification_insert(noti.Unpack(), &retid);
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
 *
 *  @return result
 *  @retval true        success
 *  @retval false       notification handle is null or
 *                      notification is not add yet or
 *                      notification_update() failed
 */
//--------------------------------------------------------------------------
bool
CicoNotificationApp::Update(CicoNotification noti)
{
    if ((true == noti.Empty()) || (0 == m_privateid)) {
        ICO_WRN("notification handle is null. or not add notification.");
        return false;
    }

    notification_error_e err = notification_update(noti.Unpack());
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
CicoNotificationApp::Delete(CicoNotification noti)
{
    if ((true == noti.Empty()) || (0 == m_privateid)) {
        ICO_WRN("notification handle is null. or not add notification.");
        return false;
    }

    const char *pkgname = noti.GetPkgname();
    if (NULL == pkgname) {
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
// vim:set expandtab ts=4 sw=4:
