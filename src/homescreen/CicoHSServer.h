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
 *  @file   CicoHSServer.h
 *
 *  @brief  This file is definition of CicoHSServer class
 */
//==========================================================================
#ifndef __CICO_HS_SERVER_H__
#define __CICO_HS_SERVER_H__

#include <list>
#include <string>

#include <Ecore.h>
#include <Eina.h>

#include <ico_uws.h>

//==========================================================================
//  forward declaration
//==========================================================================
class CicoHSUwsHandler;
class CicoHSMessage;
class CicoHSCommand;
class CicoHSAppControl;

//==========================================================================
/**
 *  @brief  This class has function of interprocess message server
 */
//==========================================================================
class CicoHSServer
{
public:
    // get CicoHSServer instance
    static CicoHSServer* getInstance();

    // set application control instance
    void setAppCtrl(CicoHSAppControl* appCtrl);

    // startup server
    int startup(int port, const char *protocol);

    // startup server
    void teardown(void);

    // send message to application client
    int sendMessage(const std::string & appid, CicoHSMessage* msg);

    // websocket callback function
    void receiveEventCB(const struct ico_uws_context *context,
                        const ico_uws_evt_e          event,
                        const void                   *id,
                        const ico_uws_detail         *detail,
                        void                         *user_data);

    // clear receive command queue
    void clearRecvCmdQueue(const std::string & appid);

    // clear send message queue
    void clearSendMsgQueue(const std::string & appid);

private:
    // default constructor
    CicoHSServer();

    // destructor
    ~CicoHSServer();

    // assignment operator
    CicoHSServer& operator=(const CicoHSServer &object);

    // copy constructor
    CicoHSServer(const CicoHSServer &object);

    // websocket utility callback function
    static void uwsReceiveEventCB(const struct ico_uws_context *context,
                                  const ico_uws_evt_e event,
                                  const void *id,
                                  const ico_uws_detail *detail,
                                  void *user_data);

    // ecore file destructor callback fucntion
    static Eina_Bool ecoreFdCallback(void *data,
                                          Ecore_Fd_Handler *handler);

    // add poll websocket file destructor
    void addPollFd(CicoHSUwsHandler *handler);

    // delete poll websocket file destructor
    void delPollFd(CicoHSUwsHandler *handler);

    // dispatch receive message process and send message process
    void dispatch(const CicoHSUwsHandler *handler);

    // find websocket handle by context and id
    CicoHSUwsHandler* findUwsHandler(const struct ico_uws_context *context,
                                     const void *id);

    // find websocket handle by ecore file destructor handler
    CicoHSUwsHandler* findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler);

    // find websocket handle by appid
    CicoHSUwsHandler* findUwsHandler(const std::string & appid);

    // query whether the handler exists
    bool isExistUwsHandler(const CicoHSUwsHandler *handler);

private:
    static CicoHSServer*    ms_myInstance;   ///< this class instance

    struct ico_uws_context  *m_uwsContext;   ///< websocket utility context

    CicoHSAppControl        *m_appCtrl;      ///< application control instance

    bool m_dispatchProcessing;

    /// websocket handler list
    std::list<CicoHSUwsHandler*> m_uwsHandlerList;

    /// send message queue
    std::list<CicoHSMessage*> m_sendMsgQueue;

    /// recieve message queue
    std::list<CicoHSCommand*> m_recvCmdQueue;
};
#endif  // __CICO_HS_SERVER_H__
// vim:set expandtab ts=4 sw=4:
