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
    // default constructor
    CicoNotification();

    // constructor
    CicoNotification(notification_h noti);

    // destructor
    ~CicoNotification();

    bool Initialize(notification_type_e type);
    bool Terminate();
    notification_h Unpack(void) const;
    notification_h Clone(void);
    bool Empty(void) const;
    notification_type_e GetType(void) const;
    bool SetPkgname(const char *pkgname);
    const char *GetPkgname(void);
    bool SetTitle(const char *title);
    const char *GetTitle(void);
    bool SetContent(const char *content);
    const char *GetContent(void);
    bool SetIconPath(const char *path);
    const char *GetIconPath(void);

protected:
    // assignment operator
    CicoNotification& operator=(const CicoNotification & object);

    // copy constructor
    CicoNotification(const CicoNotification & object);

private:
    notification_h m_notification;
    notification_type_e m_type;
};
#endif // __CICO_NOTIFICATION_H__
// vim:set expandtab ts=4 sw=4:
