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
 *  @file   CicoNotification.h
 *
 *  @brief  This file is definition of CicoNotification class
 */
//==========================================================================
#ifndef __CICO_NOTIFICATION_H__
#define __CICO_NOTIFICATION_H__

#include <notification.h>

//--------------------------------------------------------------------------
/**
 *  @class  CicoNotification
 *
 *  @brief  This class provide notification functions
 */
//--------------------------------------------------------------------------
class CicoNotification {
public:
    // constructor
    CicoNotification(notification_type_e type);

    // constructor
    CicoNotification(notification_h noti);

    // destructor
    ~CicoNotification();

    // get notfication handle
    notification_h GetNotiHandle(void) const;

    // is empty
    bool Empty(void) const;

    // get notification type
    notification_type_e GetType(void) const;

    // get private id
    int GetPrivId(void);

    // set caller package name
    bool SetPkgname(const char *pkgname);

    // get caller package name
    const char *GetPkgname(void);

    // set title
    bool SetTitle(const char *title);

    // get title
    const char *GetTitle(void);

    // set content
    bool SetContent(const char *content);

    // get content
    const char *GetContent(void);

    // set icon path
    bool SetIconPath(const char *path);

    // get icon path
    const char *GetIconPath(void);

    // set execute option
    bool SetExecuteOption(notification_execute_type_e type,
                          const char *text,
                          const char *key,
                          bundle *service_handle);

    // get execute option
    bool GetExecuteOption(notification_execute_type_e type,
                          const char **text,
                          bundle **service_handle);

protected:
    // default constructor
    CicoNotification();

    // assignment operator
    CicoNotification& operator=(const CicoNotification & object);

    // copy constructor
    CicoNotification(const CicoNotification & object);

private:
    /// notification handle
    notification_h m_notification;

    /// notification type
    notification_type_e m_type;

    /// notification private id
    int m_privId;

    /// notification group id
    int m_groupId;
};
#endif // __CICO_NOTIFICATION_H__
// vim:set expandtab ts=4 sw=4:
