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
 *  @brief 
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
class CicoSCUwsHandle;
class CicoSCMessage;
class CicoSCCommand;
class CicoSCWindowController;
class CicoSCInputController;
class CicoSCUserManager;
class CicoSCResourceManager;

//==========================================================================
/**
 *  @brief  communication server
 */
//==========================================================================
class CicoSCServer
{
public:
    static CicoSCServer* getInstance();

    void setWindowCtrl(CicoSCWindowController* windowCtrl);
    void setInputCtrl(CicoSCInputController* inputCtrl);
    void setUserMgr(CicoSCUserManager* userMgr);
    void setResourceMgr(CicoSCResourceManager* resourceMgr);

	int startup(int port, const char *protocol);

    void addPollFd(CicoSCUwsHandle *handle);
    void delPollFd(CicoSCUwsHandle *handle);
    void dispatch(const CicoSCUwsHandle *handle, int flags);
    int sendMessage(const string & appid, CicoSCMessage* msg);
    int sendMessageToHomeScreen(CicoSCMessage* msg);

    void uwsCallbackImpl(const struct ico_uws_context *context,
                         const ico_uws_evt_e event,
                         const void *id,
                         const ico_uws_detail *detail,
                         void *user_data);


private:
    CicoSCServer();
    ~CicoSCServer();
    static void uwsCallback(const struct ico_uws_context *context,
                            const ico_uws_evt_e event,
                            const void *id,
                            const ico_uws_detail *detail,
                            void *user_data);

    static Eina_Bool ecoreFdCallback(void *data,
                                          Ecore_Fd_Handler *handler);

    CicoSCUwsHandle* findUwsHandler(const struct ico_uws_context *context,
                                    const void *id);
    CicoSCUwsHandle* findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler);
    CicoSCUwsHandle* findUwsHandler(const string & appid);
 
private:
    static CicoSCServer* ms_myInstance;

    struct ico_uws_context *m_uwsContext; // TODO init

    CicoSCWindowController *m_windowCtrl;
    CicoSCInputController  *m_inputCtrl;
    CicoSCUserManager      *m_userMgr;
    CicoSCResourceManager  *m_resourceMgr;

    list<CicoSCUwsHandle*> m_uwsHandlerList;
    list<CicoSCMessage*>   m_sendMsgQueue;
    list<CicoSCCommand*>   m_recvCmdQueue;
};
#endif  // __CICO_SC_SERVER_H__
// vim:set expandtab ts=4 sw=4:
