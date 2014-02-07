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
 *  @file   CicoHSServer.cpp
 *
 *  @brief  This file implementation of CicoHSServer class
 */
//==========================================================================

#include <sstream>
using namespace std;

#include "CicoHSServer.h"
#include "CicoHSCommand.h"
#include "CicoHSMessage.h"
#include "CicoHomeScreen.h"
#include "CicoHSAppControl.h"

#include <ico_log.h>
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

class CicoHSUwsHandler
{
public:
    CicoHSUwsHandler()
        : uwsContext(NULL), id(NULL), fd(-1), serviceFlag(false),
          ecoreFdHandler(NULL), appid("") {}
    void dump(void) const {
        ICO_DBG("uwsContext=0x%08x fd=%d service=%s "
                "ecoreFdHandler=0x%08x appid=%s",
                uwsContext, fd, serviceFlag ? "true" : "false",
                ecoreFdHandler, appid.c_str());
    }
    struct ico_uws_context *uwsContext;
    void*  id;
    int    fd;
    bool   serviceFlag;
    Ecore_Fd_Handler *ecoreFdHandler;
    string appid;
};

//==========================================================================    
//  private static variable
//==========================================================================    
CicoHSServer* CicoHSServer::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoHSServer::CicoHSServer()
    : m_uwsContext(NULL),  m_appCtrl(NULL),
      m_dispatchProcessing(false)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoHSServer::~CicoHSServer()
{
    if (NULL != m_uwsContext) {
        ico_uws_close(m_uwsContext);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   get CicoHSServer instance
 *
 *  @return CicoHSServer instance
 */
//--------------------------------------------------------------------------
CicoHSServer*
CicoHSServer::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoHSServer();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set application control instance
 *
 *  @param [in] appCtrl  application control instance
 */
//--------------------------------------------------------------------------
void
CicoHSServer::setAppCtrl(CicoHSAppControl *appCtrl)
{
    m_appCtrl = appCtrl;
}

//--------------------------------------------------------------------------
/**
 *  @brief   startup server
 *
 *  @param [in] port        websocket port
 *  @param [in] protocol    websocket protocol name
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ENOSYS  error(connection fail)
 */
//--------------------------------------------------------------------------
int
CicoHSServer::startup(int port, const char *protocol)
{
    /* create uir string ":PORT" */
    stringstream uri;
    uri << ":" << port;

    /* create context */
    ICO_DBG("called: ico_uws_create_context(port=%s protocol=%s)",
            uri.str().c_str(), protocol);
    m_uwsContext = ico_uws_create_context(uri.str().c_str(), protocol);
    if (NULL == m_uwsContext) {
        ICO_ERR("ico_uws_create_context() failed.");
        return ICO_SYC_ENOSYS;
    }
    ico_uws_service(m_uwsContext);

    /* set callback */
    int ret = ico_uws_set_event_cb(m_uwsContext, uwsReceiveEventCB,
                                   (void *)this);
    if (ret != ICO_UWS_ERR_NONE) {
        ICO_ERR("ico_uws_set_event_cb() failed(%d).", ret);
        return ICO_SYC_ENOSYS;
    }
    ico_uws_service(m_uwsContext);

    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  teardown server
 */
//--------------------------------------------------------------------------
void
CicoHSServer::teardown(void)
{
    ICO_TRA("CicoHSServer::teardown Enter");
    {
        std::list<CicoHSUwsHandler*>::iterator itr;
        itr = m_uwsHandlerList.begin();
        for (; itr !=  m_uwsHandlerList.end(); ++itr) {
            if (NULL != (*itr)->ecoreFdHandler) {
                ecore_main_fd_handler_del((*itr)->ecoreFdHandler);
                (*itr)->ecoreFdHandler = NULL;
            }
            delete(*itr);
        }
        m_uwsHandlerList.clear();
    }

    {
        std::list<CicoHSMessage*>::iterator itr;
        itr = m_sendMsgQueue.begin();
        for (; itr != m_sendMsgQueue.end(); ++itr) {
            delete(*itr);
        }
        m_sendMsgQueue.clear();
    }
    
    {
        std::list<CicoHSCommand*>::iterator itr;
        itr = m_recvCmdQueue.begin();
        for (; itr != m_recvCmdQueue.end(); ++itr) {
            delete(*itr);
        }
    }

    if (NULL != m_uwsContext) {
        ico_uws_close(m_uwsContext);
        m_uwsContext = NULL;
    }
    ICO_TRA("CicoHSServer::teardown Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   add poll websocket file destructor
 *
 *  @param [in] handler  websocket handler
 */
//--------------------------------------------------------------------------
void
CicoHSServer::addPollFd(CicoHSUwsHandler *handler)
{
    ICO_TRA("CicoHSServer::addPollFd Enter(fd=%d)", handler->fd);
    Ecore_Fd_Handler_Flags flags;
    flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR);

    handler->ecoreFdHandler = ecore_main_fd_handler_add(handler->fd, flags,
                                                       ecoreFdCallback,
                                                       this, NULL, NULL);

    ICO_DBG("Enqueue uwsHandler(0x%08x)", handler);
    m_uwsHandlerList.push_back(handler);

    ICO_TRA("CicoHSServer::addPollFd Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   delete poll websocket file destructor
 *
 *  @param [in] handler  websocket handler
 */
//--------------------------------------------------------------------------
void
CicoHSServer::delPollFd(CicoHSUwsHandler *handler)
{
    ICO_TRA("CicoHSServer::delPollFd Enter");

    if (NULL == handler) {
        ICO_WRN("handler is null");
        ICO_TRA("CicoHSServer::delPollFd Leave");
        return;
    }

    if (NULL != handler->ecoreFdHandler) {
        ecore_main_fd_handler_del(handler->ecoreFdHandler);
        handler->ecoreFdHandler = NULL;
    }

    list<CicoHSUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
        if (*itr == handler) {
            ICO_DBG("Dequeue uwsHandler(0x%08x)", *itr);
            m_uwsHandlerList.erase(itr);
            break;
        }
    }
    delete handler;

    ICO_TRA("CicoHSServer::delPollFd Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   dispatch receive message process and send message process
 *
 *  @param [in] handler  websocket handler
 *
 *  @return websocket handler instance on find, NULL on not found
 */
//--------------------------------------------------------------------------
void
CicoHSServer::dispatch(const CicoHSUwsHandler *handler)
{
//    ICO_TRA("CicoHSServer::dispatch Enter(handler=0x%08x)", handler);

    if (NULL == handler) {
        ICO_WRN("handler is null");
        ICO_TRA("CicoHSServer::dispatch Leave");
        return;
    }

    ico_uws_service(handler->uwsContext);

    // There is a possibility that after calling ico_uws_service function,
    // the file is deleted.  Check whether handler not the disabled.
    if (false == isExistUwsHandler(handler)) {
        ICO_TRA("CicoHSServer::dispatch Leave");
        return;
    }

    if (true == m_dispatchProcessing) {
        ICO_TRA("CicoHSServer::dispatch Leave(disptch processing)");
        return;
    }

    m_dispatchProcessing = true;
    list<CicoHSCommand*>::iterator itr;
    itr = m_recvCmdQueue.begin();
    while(itr != m_recvCmdQueue.end()) {
        ICO_DBG("Dequeue command(0x%08x)", (*itr)->cmdid);
        CicoHSCommand *cmd = *itr;
        itr = m_recvCmdQueue.erase(itr);
        switch (cmd->cmdid) {
        case MSG_CMD_APP_START:
        case MSG_CMD_APP_STOP:
        case MSG_CMD_WIN_CHANGE:
            m_appCtrl->handleCommand(cmd);
            break;
        default:
            ICO_WRN("command: Unknown type");
            break;
        }
        delete cmd;
    }
    m_dispatchProcessing = false;

    if (NULL == handler->ecoreFdHandler) {
        ICO_ERR("ecoreFdHandler is null");
        ICO_TRA("CicoHSServer::dispatch Leave");
        return;
    }

    Eina_Bool flag = ecore_main_fd_handler_active_get(handler->ecoreFdHandler,
                                                      ECORE_FD_WRITE);
    if (EINA_TRUE == flag) {
//        ICO_DBG("start send message");
        list<CicoHSMessage*>::iterator send_itr;
        send_itr = m_sendMsgQueue.begin();
        while (send_itr != m_sendMsgQueue.end()) {
//            ICO_DBG("m_sendMsgQueue.size=%d", m_sendMsgQueue.size());
            CicoHSMessage* msg = *send_itr;
            CicoHSUwsHandler *sendHandler = findUwsHandler(msg->getSendToAppid());
            if (handler != sendHandler) {
                ++send_itr;
                continue;
            }
            send_itr = m_sendMsgQueue.erase(send_itr);
            ICO_DBG("Dequeue Message(id=%d)", msg->getId());
            if ((NULL != sendHandler) && (true == sendHandler->serviceFlag)) {
                const char *data = msg->getData();
                ICO_DBG("<<<SEND appid=%s id=0x%08x msg=%s",
                        sendHandler->appid.c_str(), sendHandler->id, data);
//                ICO_DBG("called: ico_usw_send called(context=0x%08x id=0x%08x)",
//                        sendHandler->uwsContext, sendHandler->id);
                ico_uws_send(sendHandler->uwsContext, sendHandler->id,
                             (unsigned char *)data, strlen(data));

                delete msg;

                usleep(200);
            }
        }

        Ecore_Fd_Handler_Flags flags;;
        flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR);

        ecore_main_fd_handler_active_set(handler->ecoreFdHandler, flags);
    }

//    ICO_TRA("CicoHSServer::dispatch Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   send message to application client
 *
 *  @param [in] appid   application id of destination
 *  @param [in] msg     message
 *
 *  @return ICO_SYC_EOK on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoHSServer::sendMessage(const string & appid, CicoHSMessage* msg)
{
    ICO_TRA("CicoHSServer::sendMessage Enter(appid=%s, msg=%s)",
            appid.c_str(), msg->getData());

    msg->setSendToAppid(appid);
    ICO_DBG("Enqueue Message(id=%d)", msg->getId());
    m_sendMsgQueue.push_back(msg);

    CicoHSUwsHandler *handler = findUwsHandler(appid);
    if (NULL != handler) {
        Ecore_Fd_Handler_Flags flags;
        flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ  |
                                         ECORE_FD_WRITE |
                                         ECORE_FD_ERROR);

        ecore_main_fd_handler_active_set(handler->ecoreFdHandler, flags);

        dispatch(handler);
    }

    ICO_TRA("CicoHSServer::sendMessage Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/*
 *  @brief  websocket utility callback function
 *
 *  @param [in] context     context
 *  @param [in] event       event kinds
 *  @param [in] id          client id
 *  @param [in] detail      event detail
 *  @param [in] data        user data
 */
//--------------------------------------------------------------------------
void
CicoHSServer::uwsReceiveEventCB(const struct ico_uws_context *context,
                                const ico_uws_evt_e event,
                                const void *id,
                                const ico_uws_detail *detail,
                                void *user_data)
{
    if (NULL == user_data) {
        ICO_ERR("user_data is NULL");
        return;
    }

    CicoHSServer* server = static_cast<CicoHSServer*>(user_data);
    server->receiveEventCB(context, event, id, detail, user_data);
}

//--------------------------------------------------------------------------
/**
 *  @brief   ecore file destructor callback fucntion
 *
 *  @param [in] data        user data
 *  @param [in] handler     ecore file destructor handler
 *
 *  @return ECORE_CALLBACK_RENEW on retry , ECORE_CALLBACK_CANCEL on cancel
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoHSServer::ecoreFdCallback(void *data, Ecore_Fd_Handler *ecoreFdhandler)
{
//    ICO_TRA("CicoHSServer::ecoreFdCallback Enter");

    CicoHSUwsHandler *handler = NULL;
    handler =  static_cast<CicoHSServer*>(data)->findUwsHandler(ecoreFdhandler);
    if (NULL != handler) {
        static_cast<CicoHSServer*>(data)->dispatch(handler);
    }

//    ICO_TRA("CicoHSServer::ecoreFdCallback Leave");
    return ECORE_CALLBACK_RENEW;
}

//--------------------------------------------------------------------------
/**
 *  @brief   websocket callback function
 *
 *  @param [in] context     websocket context
 *  @param [in] event       changed event
 *  @param [in] id          source applicatin id
 *  @param [in] detail      event detail information
 *  @param [in] user_data   user data
 */
//--------------------------------------------------------------------------
void
CicoHSServer::receiveEventCB(const struct ico_uws_context *context,
                             const ico_uws_evt_e          event,
                             const void                   *id,
                             const ico_uws_detail         *detail,
                             void                         *user_data)
{
//    ICO_TRA("CicoHSServer::receiveEventCB Enter");

    switch (event) {
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=0x%08x)", (int)id);
//        ICO_TRA("CicoHSServer::receiveEventCB Leave");
        return;
    case ICO_UWS_EVT_ERROR:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ERROR(id=0x%08x, err=%d)", 
                (int)id, detail->_ico_uws_error.code);
//        ICO_TRA("CicoHSServer::receiveEventCB Leave");
        return;
    default:
        break;
    }

    // find handler
    CicoHSUwsHandler *handler = findUwsHandler(context, id);
    // If not found handler, create new handler
    if (NULL == handler) {
        handler = new CicoHSUwsHandler();
        handler->uwsContext = (struct ico_uws_context*)context;
        handler->id = (void*)(id);
        handler->serviceFlag = false;
    }

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        ICO_DBG(">>>RECV ICO_UWS_EVT_OPEN(id=0x%08x)", (int)id); 
        break;
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=0x%08x)", (int)id);
        delete handler;
        break;
    case ICO_UWS_EVT_RECEIVE:
    {
        ICO_DBG(">>>RECV ICO_UWS_EVT_RECEIVE(id=0x%08x, msg=%s, len=%d)", 
                (int)id, (char *)detail->_ico_uws_message.recv_data,
                detail->_ico_uws_message.recv_len);

        // convert message to command
        CicoHSCommand *cmd = new CicoHSCommand();
        cmd->parseMessage((const char*)detail->_ico_uws_message.recv_data);

        // Enqueue command
        ICO_DBG("Enqueue command(0x%08x)", cmd->cmdid);
        m_recvCmdQueue.push_back(cmd);
        break;
    }
    case ICO_UWS_EVT_ADD_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ADD_FD(id=0x%08x, fd=%d)",
                (int)id, detail->_ico_uws_fd.fd);
        handler->fd = detail->_ico_uws_fd.fd;
        addPollFd(handler);
        break;
    case ICO_UWS_EVT_DEL_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_DEL_FD(id=0x%08x, fd=%d, appid=%s)",
                (int)id, detail->_ico_uws_fd.fd, handler->appid.c_str());
        clearRecvCmdQueue(handler->appid);
        clearSendMsgQueue(handler->appid);
        delPollFd(handler);
        break;
    default:
        break;
    }
//    ICO_TRA("CicoHSServer::receiveEventCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   clear receive command queue
 *
 *  @param [in] appid   clear command application id
 */
//--------------------------------------------------------------------------
void
CicoHSServer::clearRecvCmdQueue(const std::string & appid)
{
    ICO_TRA("CicoHSServer::clearCmdQueue Enter(appid=%s)", appid.c_str());

    std::list<CicoHSCommand*>::iterator itr;
    itr = m_recvCmdQueue.begin();
    for (; itr != m_recvCmdQueue.end(); ) {
        if (0 == appid.compare((*itr)->appid)) {
            ICO_DBG("Dequeue command(0x%08x)", (*itr)->cmdid);
            delete *itr;
            itr = m_recvCmdQueue.erase(itr);
        }
        else {
            ++itr;
        }
    }

    ICO_TRA("CicoHSServer::clearCmdQueue Leave")
}

//--------------------------------------------------------------------------
/**
 *  @brief   clear send message queue
 *
 *  @param [in] appid   clear message application id
 */
//--------------------------------------------------------------------------
void
CicoHSServer::clearSendMsgQueue(const std::string & appid)
{
    ICO_TRA("CicoHSServer::clearMsgQueue Enter(appid=%s)", appid.c_str())

    std::list<CicoHSMessage*>::iterator itr;
    itr = m_sendMsgQueue.begin();
    while(itr != m_sendMsgQueue.end()) {
        if (0 == appid.compare((*itr)->getSendToAppid())) {
            ICO_DBG("Dequeue Message(id=%d)", (*itr)->getId());
            delete *itr;
            itr = m_sendMsgQueue.erase(itr);
        }
        else {
            ++itr;
        }
    }

    ICO_TRA("CicoHSServer::clearMsgQueue Leave")
}

//--------------------------------------------------------------------------
/**
 *  @brief   find websocket handler by context and id
 *
 *  @param [in] context     websocket context
 *  @param [in] id          id
 *
 *  @return websocket handler instance on find, NULL on not found
 */
//--------------------------------------------------------------------------
CicoHSUwsHandler*
CicoHSServer::findUwsHandler(const struct ico_uws_context *context,
                             const void                   *id)
{
    list<CicoHSUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
//        ICO_DBG("handler->context=%p handler->id=%p context=%p id=%p",
//                (*itr)->uwsContext, (*itr)->id, context, id);
        if (((*itr)->uwsContext == context) &&
            ((*itr)->id == id)) {
            return *itr;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   find websocket handler by ecore file destructor handler
 *
 *  @param [in] ecoreFdHandler  ecore file destructor handler
 *
 *  @return websocket handler instance on find, NULL on not found
 */
//--------------------------------------------------------------------------
CicoHSUwsHandler*
CicoHSServer::findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler)
{
    list<CicoHSUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
        if ((*itr)->ecoreFdHandler == ecoreFdHandler) {
            return *itr;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   find websocket handler by appid
 *
 *  @param [in] addid   application id
 *
 *  @return websocket handler instance on find, NULL on not found
 */
//--------------------------------------------------------------------------
CicoHSUwsHandler*
CicoHSServer::findUwsHandler(const string & appid)
{
    list<CicoHSUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
//        ICO_DBG("handler->id=%p handler->appid=%s appid=%s",
//                (*itr)->id, (*itr)->appid.c_str(), appid.c_str());
        if ((*itr)->appid == appid) {
            return *itr;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   find websocket handler by appid
 *
 *  @param [in] handler     websocket handler instance
 *
 *  @return true on exist, false on not exist
 */
//--------------------------------------------------------------------------
bool
CicoHSServer::isExistUwsHandler(const CicoHSUwsHandler *handler)
{   
    list<CicoHSUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
        if (*itr == handler) {
            return true;
        }
    }
    return false;
}
// vim:set expandtab ts=4 sw=4:
