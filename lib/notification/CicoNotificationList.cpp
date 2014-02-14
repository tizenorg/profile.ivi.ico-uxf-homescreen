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
 *  @file   CicoNotificationList.cpp
 *
 *  @brief  This file is implemetation of CicoNotificationList class
 */
//==========================================================================

#include <ico_log.h>

#include "CicoNotificationList.h"

//--------------------------------------------------------------------------
/**
 *  @brief  constructor
 *
 *  @param [in] type    notification type
 *      NOTIFICATION_TYPE_NONE is both notification type and ongoing type.
 *      NOTIFICATION_TYPE_NOTI is notification type only.
 *      NOTIFICATION_TYPE_ONGOING is ongoing type only.
 *  @param [in] count   count returned notification data number
 *      If count is -1, all of notification list is returned.(default)
 */
//--------------------------------------------------------------------------
CicoNotificationList::CicoNotificationList(notification_type_e type, int count)
    : m_notiList(NULL), m_size(0)
{
    notification_error_e err = NOTIFICATION_ERROR_NONE;

    err = notification_get_list(type, count, &m_notiList);
    if (NOTIFICATION_ERROR_NONE == err) {
        if (NULL != m_notiList) {
            notification_list_h getlist = NULL;
            getlist = notification_list_get_head(m_notiList);
            do {
                getlist = notification_list_get_next(m_notiList);
                m_size++;
            } while (getlist != NULL);
        }
        else {
            m_notiList = NULL;
            m_size = 0;
        }
    }
    else {
        ICO_ERR("notification_get_list() failed(%d).", err);
        m_notiList = NULL;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoNotificationList::~CicoNotificationList()
{
    if (m_notiList != NULL) {
        notification_error_e err = notification_free_list(m_notiList);
        if (NOTIFICATION_ERROR_NONE != err) {
            ICO_ERR("notification_free_list() failed.");
        }
        m_notiList = NULL;
        m_size = 0;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get notification data number
 */
//--------------------------------------------------------------------------
int
CicoNotificationList::size(void)
{
    return m_size;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get head notification handle and
 *          reset cuurent index to head notification list
 *
 *  @return notification handle
 */
//--------------------------------------------------------------------------
notification_h
CicoNotificationList::head(void)
{
    m_current = notification_list_get_head(m_notiList);
    if (NULL == m_current) {
        return NULL;
    }
    return notification_list_get_data(m_current);
}

//--------------------------------------------------------------------------
/**
 *  @brief  get tail notification handle and
 *          reset cuurent index to tail notification list
 *
 *  @return notification handle
 */
//--------------------------------------------------------------------------
notification_h
CicoNotificationList::tail(void)
{
    m_current = notification_list_get_tail(m_notiList);
    if (NULL == m_current) {
        return NULL;
    }
    return notification_list_get_data(m_current);
}

//--------------------------------------------------------------------------
/**
 *  @brief  get next notification handle
 *
 *  @return notification handle
 */
//--------------------------------------------------------------------------
notification_h
CicoNotificationList::next(void)
{
    m_current = notification_list_get_next(m_notiList);
    if (NULL == m_current) {
        return NULL;
    }
    return notification_list_get_data(m_current);
}

//--------------------------------------------------------------------------
/**
 *  @brief  get prev notification handle
 *
 *  @return notification handle
 */
//--------------------------------------------------------------------------
notification_h
CicoNotificationList::prev(void)
{
    m_current = notification_list_get_prev(m_notiList);
    if (NULL == m_current) {
        return NULL;
    }
    return notification_list_get_data(m_current);
}
// vim:set expandtab ts=4 sw=4:
