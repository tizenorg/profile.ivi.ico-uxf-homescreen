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
 *  @file   CicoNotificationApp.h
 *
 *  @brief  This file is definition of CicoNotificationApp class
 */
//==========================================================================
#ifndef __CICO_NOTIFICATION_APP_H__
#define __CICO_NOTIFICATION_APP_H__

#include "CicoNotificationInterface.h"

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotificationApp
 *
 *  @brief  This class provide interface of notification for application
 */
//--------------------------------------------------------------------------
class CicoNotificationApp : public CicoNotificationAppInterface
{
public:
    // default constructor
    CicoNotificationApp();

    // constructor
    ~CicoNotificationApp();

    // add notification
    bool Add(CicoNotification noti);

    // update notification
    bool Update(CicoNotification noti);

    // delete notification
    bool Delete(CicoNotification noti);

private:
    // private id of notification
    int m_privateid;
};
#endif // __CICO_NOTIFICATION_APP_H__
// vim:set expandtab ts=4 sw=4:
