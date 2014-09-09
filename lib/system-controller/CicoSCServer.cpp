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
 *  @file   CicoSCServer.cpp
 *
 *  @brief  This file implementation of CicoSCServer class
 */
//==========================================================================

#include <sstream>
using namespace std;

#include "CicoSCServer.h"
#include "CicoSCCommand.h"
#include "CicoSCMessage.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"
#include "CicoSCUserManager.h"
#include "CicoSCUser.h"
#include "CicoSCResourceManager.h"
#include "CicoSCPolicyManager.h"

class CicoSCUwsHandler
{
public:
    CicoSCUwsHandler()
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
CicoSCServer* CicoSCServer::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCServer::CicoSCServer()
    : m_uwsContext(NULL), m_windowCtrl(NULL),
      m_inputCtrl(NULL) , m_userMgr(NULL), m_resourceMgr(NULL),
      m_policyMgr(NULL), m_dispatchProcessing(false)
{
    CicoSCServer::ms_myInstance = this;
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCServer::~CicoSCServer()
{
    CicoSCServer::ms_myInstance = NULL;
    if (NULL != m_uwsContext) {
        ico_uws_close(m_uwsContext);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   get CicoSCServer instance
 *
 *  @return CicoSCServer instance
 */
//--------------------------------------------------------------------------
CicoSCServer*
CicoSCServer::getInstance(void)
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoSCServer();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set window controller instance
 *
 *  @param [in] windowCtrl  controller instance
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setWindowCtrl(CicoSCWindowController *windowCtrl)
{
    m_windowCtrl = windowCtrl;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set input controller instance
 *
 *  @param [in] inputCtrl   controller instance
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setInputCtrl(CicoSCInputController *inputCtrl)
{
    m_inputCtrl= inputCtrl;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set user manager
 *
 *  @param [in] userMgr     user manager instance
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setUserMgr(CicoSCUserManager *userMgr)
{
    m_userMgr = userMgr;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set resource manager instance
 *
 *  @param [in] resourceMgr resouce manager instance
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setResourceMgr(CicoSCResourceManager *resourceMgr)
{
    m_resourceMgr = resourceMgr;
}

//--------------------------------------------------------------------------
/**
 *  @brief   set policy manager instance
 *
 *  @param [in] policyMgr policy manager instance
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setPolicyMgr(CicoSCPolicyManager *policyMgr)
{
    m_policyMgr = policyMgr;
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
CicoSCServer::startup(int port, const char *protocol)
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
CicoSCServer::teardown(void)
{
    ICO_TRA("CicoSCServer::teardown Enter");
    {
        std::list<CicoSCUwsHandler*>::iterator itr;
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
        std::list<CicoSCMessage*>::iterator itr;
        itr = m_sendMsgQueue.begin();
        for (; itr != m_sendMsgQueue.end(); ++itr) {
            delete(*itr);
        }
        m_sendMsgQueue.clear();
    }

    {
        std::list<CicoSCCommand*>::iterator itr;
        itr = m_recvCmdQueue.begin();
        for (; itr != m_recvCmdQueue.end(); ++itr) {
            delete(*itr);
        }
    }

    if (NULL != m_uwsContext) {
        ico_uws_close(m_uwsContext);
        m_uwsContext = NULL;
    }
    ICO_TRA("CicoSCServer::teardown Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   add poll websocket file destructor
 *
 *  @param [in] handler  websocket handler
 */
//--------------------------------------------------------------------------
void
CicoSCServer::addPollFd(CicoSCUwsHandler *handler)
{
    ICO_TRA("CicoSCServer::addPollFd Enter(fd=%d)", handler->fd);
    Ecore_Fd_Handler_Flags flags;
    flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR);

    handler->ecoreFdHandler = ecore_main_fd_handler_add(handler->fd, flags,
                                                       ecoreFdCallback,
                                                       this, NULL, NULL);

    ICO_DBG("Enqueue uwsHandler(0x%08x)", handler);
    m_uwsHandlerList.push_back(handler);

    ICO_TRA("CicoSCServer::addPollFd Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   delete poll websocket file destructor
 *
 *  @param [in] handler  websocket handler
 */
//--------------------------------------------------------------------------
void
CicoSCServer::delPollFd(CicoSCUwsHandler *handler)
{
    ICO_TRA("CicoSCServer::delPollFd Enter");

    if (NULL == handler) {
        ICO_WRN("handler is null");
        ICO_TRA("CicoSCServer::delPollFd Leave");
        return;
    }

    if (NULL != handler->ecoreFdHandler) {
        ecore_main_fd_handler_del(handler->ecoreFdHandler);
        handler->ecoreFdHandler = NULL;
    }

    list<CicoSCUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
        if (*itr == handler) {
            ICO_DBG("Dequeue uwsHandler(0x%08x)", *itr);
            m_uwsHandlerList.erase(itr);
            break;
        }
    }
    delete handler;

    ICO_TRA("CicoSCServer::delPollFd Leave");
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
CicoSCServer::dispatch(const CicoSCUwsHandler *handler)
{
//    ICO_TRA("CicoSCServer::dispatch Enter(handler=0x%08x)", handler);

    if (NULL == handler) {
        ICO_WRN("handler is null");
        ICO_TRA("CicoSCServer::dispatch Leave");
        return;
    }

    ico_uws_service(handler->uwsContext);

    // There is a possibility that after calling ico_uws_service function,
    // the file is deleted.  Check whether handler not the disabled.
    if (false == isExistUwsHandler(handler)) {
        ICO_TRA("CicoSCServer::dispatch Leave");
        return;
    }

    if (true == m_dispatchProcessing) {
        ICO_TRA("CicoSCServer::dispatch Leave(disptch processing)");
        return;
    }

    m_dispatchProcessing = true;
    list<CicoSCCommand*>::iterator itr;
    itr = m_recvCmdQueue.begin();
    while(itr != m_recvCmdQueue.end()) {
        ICO_DBG("Dequeue command(0x%08x)", (*itr)->cmdid);
        CicoSCCommand *cmd = *itr;
        itr = m_recvCmdQueue.erase(itr);
        switch (cmd->cmdid & MSG_CMD_TYPE_MASK) {
        case MSG_CMD_TYPE_WINCTRL:
            //ICO_DBG("command : MSG_CMD_TYPE_WINCTRL");
            m_windowCtrl->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_INPUTCTRL:
            //ICO_DBG("command : MSG_CMD_TYPE_INPUTCTRL");
            m_inputCtrl->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_USERMGR:
            //ICO_DBG("command : MSG_CMD_TYPE_USERMGR");
            m_userMgr->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_RESOURCEMGR:
            //ICO_DBG("command : MSG_CMD_TYPE_RESOURCEMGR");
            m_resourceMgr->handleCommand(*cmd);
            break;
        case MSG_CMD_TYPE_INPUTDEVSETTING:
            //ICO_DBG("command : MSG_CMD_TYPE_INPUTDEVSETTING");
            m_inputCtrl->handleCommand(cmd);
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
        ICO_TRA("CicoSCServer::dispatch Leave");
        return;
    }

    Eina_Bool flag = ecore_main_fd_handler_active_get(handler->ecoreFdHandler,
                                                      ECORE_FD_WRITE);
    if (EINA_TRUE == flag) {
//        ICO_DBG("start send message");
        list<CicoSCMessage*>::iterator send_itr;
        send_itr = m_sendMsgQueue.begin();
        while (send_itr != m_sendMsgQueue.end()) {
//            ICO_DBG("m_sendMsgQueue.size=%d", m_sendMsgQueue.size());
            CicoSCMessage* msg = *send_itr;
            CicoSCUwsHandler *sendHandler = findUwsHandler(msg->getSendToAppid());
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

//    ICO_TRA("CicoSCServer::dispatch Leave");
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
CicoSCServer::sendMessage(const string & appid, CicoSCMessage* msg)
{
    ICO_TRA("CicoSCServer::sendMessage Enter(appid=%s, msg=%s)",
            appid.c_str(), msg->getData());

    msg->setSendToAppid(appid);
    ICO_DBG("Enqueue Message(id=%d)", msg->getId());
    m_sendMsgQueue.push_back(msg);

    CicoSCUwsHandler *handler = findUwsHandler(appid);
    if (NULL != handler) {
        Ecore_Fd_Handler_Flags flags;
        flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ  |
                                         ECORE_FD_WRITE |
                                         ECORE_FD_ERROR);

        ecore_main_fd_handler_active_set(handler->ecoreFdHandler, flags);

        dispatch(handler);
    }

    ICO_TRA("CicoSCServer::sendMessage Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief   send message to homescreen
 *
 *  @param [in] msg     message
 *
 *  @return ICO_SYC_EOK on success, other on error
 */
//--------------------------------------------------------------------------
int
CicoSCServer::sendMessageToHomeScreen(CicoSCMessage* msg)
{
    // if user change processing(homescree is not running)
    if (true == m_userMgr->isStoppingNow()) {
        ICO_DBG("homescreen not running");
        delete msg;
        return ICO_SYC_ENOENT;
    }

    const CicoSCUser *loginUser = m_userMgr->getLoginUser();
    if (NULL == loginUser) {
        ICO_WRN("homescreen unknown");
        delete msg;
        return ICO_SYC_ENOENT;
    }
    return sendMessage(loginUser->homescreen, msg);
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
CicoSCServer::uwsReceiveEventCB(const struct ico_uws_context *context,
                                const ico_uws_evt_e event,
                                const void *id,
                                const ico_uws_detail *detail,
                                void *user_data)
{
    if (NULL == user_data) {
        ICO_ERR("user_data is NULL");
        return;
    }

    CicoSCServer* server = static_cast<CicoSCServer*>(user_data);
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
CicoSCServer::ecoreFdCallback(void *data, Ecore_Fd_Handler *ecoreFdhandler)
{
//    ICO_TRA("CicoSCServer::ecoreFdCallback Enter");

    CicoSCUwsHandler *handler = NULL;
    handler =  static_cast<CicoSCServer*>(data)->findUwsHandler(ecoreFdhandler);
    if (NULL != handler) {
        static_cast<CicoSCServer*>(data)->dispatch(handler);
    }

//    ICO_TRA("CicoSCServer::ecoreFdCallback Leave");
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
CicoSCServer::receiveEventCB(const struct ico_uws_context *context,
                             const ico_uws_evt_e          event,
                             const void                   *id,
                             const ico_uws_detail         *detail,
                             void                         *user_data)
{
//    ICO_TRA("CicoSCServer::receiveEventCB Enter");

    switch (event) {
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=%p)", id);
//        ICO_TRA("CicoSCServer::receiveEventCB Leave");
        return;
    case ICO_UWS_EVT_ERROR:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ERROR(id=%p, err=%d)",
                id, detail->_ico_uws_error.code);
//        ICO_TRA("CicoSCServer::receiveEventCB Leave");
        return;
    default:
        break;
    }

    // find handler
    CicoSCUwsHandler *handler = findUwsHandler(context, id);
    // If not found handler, create new handler
    if (NULL == handler) {
        handler = new CicoSCUwsHandler();
        handler->uwsContext = (struct ico_uws_context*)context;
        handler->id = (void*)(id);
        handler->serviceFlag = false;
    }

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        ICO_DBG(">>>RECV ICO_UWS_EVT_OPEN(id=%p)", id);
        break;  // break of switch event
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=%p)", id);
        delete handler;
        break;  // break of switch event
    case ICO_UWS_EVT_RECEIVE:
    {
        ICO_DBG(">>>RECV ICO_UWS_EVT_RECEIVE(id=%p, msg=%s, len=%d)",
                id, (char *)detail->_ico_uws_message.recv_data,
                detail->_ico_uws_message.recv_len);

        // convert message to command
        CicoSCCommand *cmd = new CicoSCCommand();
        cmd->parseMessage((const char*)detail->_ico_uws_message.recv_data);

        // update handler appid
        if (cmd->cmdid == MSG_CMD_SEND_APPID) {
            if (0 == cmd->appid.length()) {
                ICO_WRN("command argument invalid appid null");
                delete cmd;
                break;  // break of switch event
            }
            handler->appid = cmd->appid;
            handler->serviceFlag = true;
            ICO_DBG("handler.appid=%s", handler->appid.c_str());

            Ecore_Fd_Handler_Flags flags;
            flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ  |
                                             ECORE_FD_WRITE |
                                             ECORE_FD_ERROR);

            ecore_main_fd_handler_active_set(handler->ecoreFdHandler, flags);

            notifyConnected(handler->appid);
            delete cmd;
            break;  // break of switch event
        }

        // Enqueue command
        ICO_DBG("Enqueue command(0x%08x)", cmd->cmdid);
        m_recvCmdQueue.push_back(cmd);
        break;  // break of switch event
    }
    case ICO_UWS_EVT_ADD_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ADD_FD(id=%p, fd=%d)",
                id, detail->_ico_uws_fd.fd);
        handler->fd = detail->_ico_uws_fd.fd;
        addPollFd(handler);
        break;  // break of switch event
    case ICO_UWS_EVT_DEL_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_DEL_FD(id=%p, fd=%d, appid=%s)",
                id, detail->_ico_uws_fd.fd, handler->appid.c_str());
        clearRecvCmdQueue(handler->appid);
        clearSendMsgQueue(handler->appid);
        delPollFd(handler);
        break;  // break of switch event
    default:
        break;  // break of switch event
    }
//    ICO_TRA("CicoSCServer::receiveEventCB Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   clear receive command queue
 *
 *  @param [in] appid   clear command application id
 */
//--------------------------------------------------------------------------
void
CicoSCServer::clearRecvCmdQueue(const std::string & appid)
{
    ICO_TRA("CicoSCServer::clearCmdQueue Enter(appid=%s)", appid.c_str());

    std::list<CicoSCCommand*>::iterator itr;
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

    ICO_TRA("CicoSCServer::clearCmdQueue Leave")
}

//--------------------------------------------------------------------------
/**
 *  @brief   clear send message queue
 *
 *  @param [in] appid   clear message application id
 */
//--------------------------------------------------------------------------
void
CicoSCServer::clearSendMsgQueue(const std::string & appid)
{
    ICO_TRA("CicoSCServer::clearMsgQueue Enter(appid=%s)", appid.c_str())

    std::list<CicoSCMessage*>::iterator itr;
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

    ICO_TRA("CicoSCServer::clearMsgQueue Leave")
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
CicoSCUwsHandler*
CicoSCServer::findUwsHandler(const struct ico_uws_context *context,
                             const void                   *id)
{
    list<CicoSCUwsHandler*>::iterator itr;
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
CicoSCUwsHandler*
CicoSCServer::findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler)
{
    list<CicoSCUwsHandler*>::iterator itr;
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
CicoSCUwsHandler*
CicoSCServer::findUwsHandler(const string & appid)
{
    list<CicoSCUwsHandler*>::iterator itr;
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
CicoSCServer::isExistUwsHandler(const CicoSCUwsHandler *handler)
{
    list<CicoSCUwsHandler*>::iterator itr;
    itr = m_uwsHandlerList.begin();
    for (; itr != m_uwsHandlerList.end(); ++itr) {
        if (*itr == handler) {
            return true;
        }
    }
    return false;
}

//--------------------------------------------------------------------------
/**
 *  @brief  notify information to homescreen on connected
 *
 *  @param [in] appid   application id
 */
//--------------------------------------------------------------------------
void
CicoSCServer::notifyConnected(const std::string & appid)
{
    const CicoSCUser *loginUser = m_userMgr->getLoginUser();
    if (NULL == loginUser) {
        ICO_WRN("homescreen unknown");
        return;
    }

    // if appid equal homescreen
    if (0 == loginUser->homescreen.compare(appid)) {
        if (NULL != m_policyMgr) {
            m_policyMgr->notifyConnected(appid);
        }
    }
}
// vim:set expandtab ts=4 sw=4:
