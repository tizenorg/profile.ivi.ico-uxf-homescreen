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
 *  @file   CicoStatusBar.h
 *
 *  @brief  This file is definition of CicoStatusBar class
 */
//==========================================================================
#ifndef __CICO_STATUSBAR_H__
#define __CICO_STATUSBAR_H__

#include <memory>
#include "CicoComponentImplementation.h"
#include "CicoCommonWindow.h"
#include "CicoNotification.h"
#include "CicoNotificationService.h"

//==========================================================================
/**
 *  @brief  This class provide the status bar window
 */
//==========================================================================
class CicoStatusBar : public CicoCommonWindow
{
public:

    // default constructor
    CicoStatusBar();

    // destructor
    ~CicoStatusBar();

    // initialize status bar 
    bool Initialize(void);

    // update clock
    bool UpdateTime(void);

    // update notification panel
    bool UpdateNotificationPanel(const char *msg, const char *icopath, 
                                 const char *soundpath);

    // notification callback
    static void NotificationCallback(void *data, notification_type_e type,
                                     notification_op *op_list, int num_op);

    // hide notification panel
    static Eina_Bool HideNotification(void *data);
private:
    // callback function of time update
    static Eina_Bool ecoreUpdateTimeCB(void *data);

protected:
    static const int STATUSBAR_POSX;    /// statusbar position x
    static const int STATUSBAR_POSY;    /// statusbar position y
    static const int STATUSBAR_HEIGHT;  /// statusbar height
    static const int CLOCK_WIDTH;       /// clock width
    static const int CLOCK_HEIGHT;      /// clock height

    Ecore_Timer *notitimer_;            /// notification timer
    std::shared_ptr<CicoStatusBarClockComponent> clockcomp_;    /// clock component
    std::shared_ptr<CicoNotificationPanelComponent> noticomp_;  /// notification component
    CicoNotificationService notiservice_;   /// notification service
};
#endif  // __CICO_STATUSBAR_H__
// vim: set expandtab ts=4 sw=4:
