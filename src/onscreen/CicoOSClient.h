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
 *  @file   CicoOSClient.h
 *
 *  @brief  This file is definition of CicoOSClient class
 */
//==========================================================================
#ifndef __CICO_OS_CLIENT_H__
#define __CICO_OS_CLIENT_H__

#include <list>
#include <string>

#include <Ecore.h>
#include <Eina.h>

#include <ico_uws.h>

//==========================================================================
//  forward declaration
//==========================================================================
const int   seqMax = 10000;
const int   seqMin = 1;
const double timerCount = 0.1;

//==========================================================================
/**
 *  @brief  This class has function of interprocess message server
 */
//==========================================================================
class CicoOSClient
{
public:
    // get CicoOSClient instance
    static CicoOSClient* getInstance();

    // startup server
    bool    connect();

    // send message to application client
    bool    sendLaunchMessage(const std::string& appid);

    // websocket callback function
    void    receiveEventCB(const struct ico_uws_context* context,
                           const ico_uws_evt_e           event,
                           const void*                   id,
                           const ico_uws_detail*         detail,
                           void*                         user_data);
private:
    // default constructor
    CicoOSClient();

    // destructor
    ~CicoOSClient();

    // copy constructor
    CicoOSClient(const CicoOSClient &object);

    // websocket utility callback function
    static void uwsReceiveEventCB(const struct ico_uws_context* context,
                                  const ico_uws_evt_e           event,
                                  const void*                   id,
                                  const ico_uws_detail*         detail,
                                  void*                         user_data);

    static Eina_Bool ecoreTimerCB(void *data);

    void    updateSeqNum();
    int     getSeqNum() const;
    bool    refresh();

private:
    static CicoOSClient*    ms_myInstance;   ///< this class instance

    struct ico_uws_context* m_uwsContext;   ///< websocket utility context
    const void*             m_id;

    int                     m_seqNum;
};
#endif  // __CICO_OS_CLIENT_H__
// vim:set expandtab ts=4 sw=4:
