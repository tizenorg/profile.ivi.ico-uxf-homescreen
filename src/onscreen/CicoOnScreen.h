/*
 * Copyright (c) 2014, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   On Screen
 *
 * @date    Jan-07-2014
 */
#ifndef __CICO_ON_SCREEN_H__
#define __CICO_ON_SCREEN_H__

#ifdef HAVE_CONFIG_H
    #include "config.h"
    #define __UNUSED__
#else
    #define __UNUSED__
#endif

#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <list>

#include <app.h>
#include <aul.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Edje.h>

#include <stdbool.h>
#include <CicoNotification.h>
#include <CicoNotificationService.h>

#include "ico_syc_common.h"
#include "ico_syc_privilege.h"

#include "ico_log.h"
#include "CicoOSPopWindow.h"

/*============================================================================*/
/* Define fixed parameters                                                    */
/*============================================================================*/

/*============================================================================*/
/* Define data types                                                          */
/*============================================================================*/

/*============================================================================*/
/* Define class                                                               */
/*============================================================================*/
class CicoOnScreen
{
public:
    CicoOnScreen(void);
    ~CicoOnScreen(void);
    bool StartOnScreen(void);

    static void NotificationCallback(void *data,
                                     notification_type_e type,
                                     notification_op *op_list,
                                     int num_op);

    static uint32_t GetResourdeId(void);

protected:
    bool requestShowSC();

    static void EventCallBack(const ico_syc_ev_e event,
                              const void *detail,
                              void *user_data);
    bool releaseWindow(uint32_t resourceId);
    bool insertNoti(notification_h noti_h);
    bool deleteNoti(int priv_id);

    static CicoOnScreen *os_instance;
    static uint32_t     surfaceid;

protected:
    CicoNotificationService notiservice_;
    std::list<CicoOSPopWindow*> m_mngWin;
    std::list<CicoOSPopWindow*> m_waitMngWin;
    CicoOSPopWindow*            m_request;
    bool                        m_del;
    CicoOSPopWindow*            m_reserve;
};
#endif  // __CICO_ON_SCREEN_H__
// vim:set expandtab ts=4 sw=4:
