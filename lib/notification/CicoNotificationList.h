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
 *  @file   CicoNotificationList.h
 *
 *  @brief  This file is definition of CicoNotificationList class
 */
//==========================================================================
#ifndef __CICO_NOTIFICATION_LIST_H__
#define __CICO_NOTIFICATION_LIST_H__

#include <notification.h>

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotificationList
 *
 *  @brief  This class provide the functions of operate notification list
 */
//--------------------------------------------------------------------------
class CicoNotificationList {
public:
    // constructor
    CicoNotificationList(notification_type_e type, int count = -1);

    // destructor
    ~CicoNotificationList();

    // get notification data number
    int size(void);

    // get head notification handle and
    // reset cuurent index to head notification list
    notification_h head(void);

    // get tail notification handle and
    // reset cuurent index to tail notification list
    notification_h tail(void);

    // get prev notification handle
    notification_h next(void);

    // get next notification handle
    notification_h prev(void);

protected:
    // default constructor
    CicoNotificationList();

    // assignment operator
    CicoNotificationList& operator=(const CicoNotificationList & object);

    // copy constructor
    CicoNotificationList(const CicoNotificationList & object);

private:
    /// the notification list handle
    notification_list_h m_notiList;

    /// current position of notification list pointer
    notification_list_h m_current;

    /// the number of notification data
    int m_size;

    /// this flag is type of list sort(lastest or oldest)
    bool m_oldest;
};
#endif // __CICO_NOTIFICATION_LIST_H__
// vim:set expandtab ts=4 sw=4:
