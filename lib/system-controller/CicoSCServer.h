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
 *  @file   CicoSCServer.h
 *
 *  @brief  This file is definition of CicoSCServer class
 */
//==========================================================================
#ifndef __CICO_SC_SERVER_H__
#define __CICO_SC_SERVER_H__

#include <list>
using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/epoll.h>

#include <getopt.h>

#include <Ecore.h>
#include <Eina.h>

#include <ico_uws.h>

//==========================================================================
//  forward declaration
//==========================================================================
class CicoSCUwsHandler;
class CicoSCMessage;
class CicoSCCommand;
class CicoSCWindowController;
class CicoSCInputController;
class CicoSCUserManager;
class CicoSCResourceManager;

//==========================================================================
/**
 *  @brief  This class has function of interprocess message server
 */
//==========================================================================
class CicoSCServer
{
public:
    // get CicoSCServer instance
    static CicoSCServer* getInstance();

    // set window controller instance
    void setWindowCtrl(CicoSCWindowController* windowCtrl);

    // set input controller instance
    void setInputCtrl(CicoSCInputController* inputCtrl);

    // set input controller instance
    void setUserMgr(CicoSCUserManager* userMgr);

    // set resource manager instance
    void setResourceMgr(CicoSCResourceManager* resourceMgr);

    // startup server
    int startup(int port, const char *protocol);

    // send message to application client
    int sendMessage(const string & appid, CicoSCMessage* msg);

    // send message to homescreen
    int sendMessageToHomeScreen(CicoSCMessage* msg);

    // websocket callback function
    void receiveEventCB(const struct ico_uws_context *context,
                        const ico_uws_evt_e          event,
                        const void                   *id,
                        const ico_uws_detail         *detail,
                        void                         *user_data);

private:
    // default constructor
    CicoSCServer();

    // destructor
    ~CicoSCServer();

    // assignment operator
    CicoSCServer& operator=(const CicoSCServer &object);

    // copy constructor
    CicoSCServer(const CicoSCServer &object);

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
    void addPollFd(CicoSCUwsHandler *handler);

    // delete poll websocket file destructor
    void delPollFd(CicoSCUwsHandler *handler);

    // dispatch receive message process and send message process
    void dispatch(const CicoSCUwsHandler *handler);

    // find websocket handle by context and id
    CicoSCUwsHandler* findUwsHandler(const struct ico_uws_context *context,
                                     const void *id);

    // find websocket handle by ecore file destructor handler
    CicoSCUwsHandler* findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler);

    // find websocket handle by appid
    CicoSCUwsHandler* findUwsHandler(const string & appid);

    // query whether the handler exists
    bool isExistUwsHandler(const CicoSCUwsHandler *handler);

private:
    static CicoSCServer*    ms_myInstance;   ///< this class instance

    struct ico_uws_context  *m_uwsContext;   ///< websocket utility context

    CicoSCWindowController  *m_windowCtrl;   ///< window controller instance
    CicoSCInputController   *m_inputCtrl;    ///< input controller instance
    CicoSCUserManager       *m_userMgr;      ///< user manager instance
    CicoSCResourceManager   *m_resourceMgr;  ///< resource manager instance

    list<CicoSCUwsHandler*> m_uwsHandlerList;///< websocket handler list
    list<CicoSCMessage*>    m_sendMsgQueue;  ///< send message queue
    list<CicoSCCommand*>    m_recvCmdQueue;  ///< recieve message queue
};
#endif  // __CICO_SC_SERVER_H__
// vim:set expandtab ts=4 sw=4:
