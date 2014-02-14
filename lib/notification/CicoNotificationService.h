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
 *  @file   CicoNotificationService.h
 *
 *  @brief  This file is definition of CicoNotificationService class
 */
//==========================================================================
#ifndef __CICO_NOTIFICATION_SERVICE_H__
#define __CICO_NOTIFICATION_SERVICE_H__

#include "CicoNotificationInterface.h"

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotificationService
 *
 *  @brief  This class provide interface of notification for service
 */
//--------------------------------------------------------------------------
class CicoNotificationService : public CicoNotificationAppInterface,
                                       CicoNotificationServiceInterface
{
public:
    // default constructor
    CicoNotificationService();

    // constructor
    ~CicoNotificationService();

    // add notification
    bool Add(CicoNotification noti);

    // update notification
    bool Update(CicoNotification noti);

    // delete notification
    bool Delete(CicoNotification noti);

    // set callback function
    bool SetCallback(void (*detailed_changed_cb)
                     (void *data,
                      notification_type_e type,
                      notification_op *op_list,
                      int num_op),
                     void *user_data);

    // unset callback function
    bool UnsetCallback(void (*detailed_changed_cb)
                       (void *data,
                        notification_type_e type,
                        notification_op *op_list,
                        int num_op));

private:
    // private id of notification
    int m_privateid;

    // user date for callback function
    void *m_user_data;
};
#endif // __CICO_NOTIFICATION_SERVICE_H__
// vim:set expandtab ts=4 sw=4:
