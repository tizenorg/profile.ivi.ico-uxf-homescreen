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
 *  @file   CicoNotificationInterface.h
 *
 *  @brief  This file is definition of CicoNotificationInterface class
 */
//==========================================================================
#ifndef __CICO_NOTIFICATION_INTERFACE_H__
#define __CICO_NOTIFICATION_INTERFACE_H__

#include "CicoNotification.h"

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotificationAppInterface
 *
 *  @brief  This class provide notification interface for application
 */
//--------------------------------------------------------------------------
class CicoNotificationAppInterface
{
public:
    // add notification
    virtual bool Add(CicoNotification noti) = 0;

    // update notification
    virtual bool Update(CicoNotification noti) = 0;

    // delete notification
    virtual bool Delete(CicoNotification noti) = 0;
};

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotificationServiceInterface
 *
 *  @brief  This class provide notification interface for service
 */
//--------------------------------------------------------------------------
class CicoNotificationServiceInterface
{
public:
    // set callback fucntion
    virtual bool SetCallback(void (*detailed_changed_cb)
                             (void *data, 
                              notification_type_e type, 
                              notification_op *op_list, 
                              int num_op),
                             void *user_data) = 0;

    // unset callback fucntion
    virtual bool UnsetCallback(void (*detailed_changed_cb)
                               (void *data, 
                                notification_type_e type, 
                                notification_op *op_list, 
                                int num_op)) = 0;
};
#endif // __CICO_NOTIFICATION_INTERFACE_H__
// vim:set expandtab ts=4 sw=4:
