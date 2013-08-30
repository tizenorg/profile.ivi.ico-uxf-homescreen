/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   library to communicate with outer process
 *
 * @date    Feb-15-2013
 */

#include <sstream>
using namespace std;

#include "CicoSCServer.h"
#include "CicoSCCommandParser.h"
#include "CicoSCMessage.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"
#include "CicoSCUserManager.h"
#include "CicoSCUser.h"
#include "CicoSCResourceManager.h"

class CicoSCUwsHandle
{
public:
    CicoSCUwsHandle()
        : uwsContext(NULL), id(NULL), fd(-1), serviceFlag(false),
          ecoreFdHandler(NULL), appid("") {}
    void dump(void) const {
        ICO_DBG("uwsContext=%08X fd=%d service=%s ecoreFdHandler=%08X appid=%s",
                uwsContext, fd, serviceFlag ? "true" : "false",
                ecoreFdHandler, appid.c_str());
    }
    struct ico_uws_context *uwsContext;
    void* id;
    int fd;
    bool serviceFlag;
    Ecore_Fd_Handler *ecoreFdHandler;
    string appid;
};

CicoSCServer* CicoSCServer::ms_myInstance = NULL;

CicoSCServer::CicoSCServer()
    : m_uwsContext(NULL), m_windowCtrl(NULL),
      m_inputCtrl(NULL) , m_userMgr(NULL)
{
}

CicoSCServer::~CicoSCServer()
{
}

CicoSCServer*
CicoSCServer::getInstance(void)
{
	if (NULL == ms_myInstance) {
		ms_myInstance = new CicoSCServer();
	}

	return ms_myInstance;
}

void
CicoSCServer::setWindowCtrl(CicoSCWindowController *windowCtrl)
{
    m_windowCtrl = windowCtrl;
}

void
CicoSCServer::setInputCtrl(CicoSCInputController *inputCtrl)
{
    m_inputCtrl= inputCtrl;
}

void
CicoSCServer::setUserMgr(CicoSCUserManager *userMgr)
{
    m_userMgr = userMgr;
}

//--------------------------------------------------------------------------
/**
 *  @brief   startup server
 *
 *  @param [IN] port        websocket port
 *  @param [IN] protocol    websocket protocol name
 *  @return result
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ENOSYS  error(connection faile)
 */
//--------------------------------------------------------------------------
void
CicoSCServer::setResourceMgr(CicoSCResourceManager *resourceMgr)
{
    m_resourceMgr = resourceMgr;
}

//--------------------------------------------------------------------------
/**
 *  @brief   startup server
 *
 *  @param [IN] port        websocket port
 *  @param [IN] protocol    websocket protocol name
 *  @return result
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ENOSYS  error(connection faile)
 */
//--------------------------------------------------------------------------
int
CicoSCServer::startup(int port, const char *protocol)
{
    /* create uir string ":PORT" */
    stringstream uri;
    uri << ":" << port;

    /* create context */
    ICO_DBG("ico_uws_create_context(%s,%s) called.",
            uri.str().c_str(), protocol);
    m_uwsContext = ico_uws_create_context(uri.str().c_str(), protocol);
    if (NULL == m_uwsContext) {
        ICO_ERR("ico_uws_create_context() failed.");
        return ICO_SYC_ENOSYS;
    }
    ico_uws_service(m_uwsContext);

    /* set callback */
    int ret = ico_uws_set_event_cb(m_uwsContext, uwsCallback, (void *)this);
    if (ret != ICO_UWS_ERR_NONE) {
        ICO_ERR("ico_uws_set_event_cb() failed(%d).", ret);
        return ICO_SYC_ENOSYS;
    }
    ico_uws_service(m_uwsContext);

    return ICO_SYC_EOK;
}

CicoSCUwsHandle*
CicoSCServer::findUwsHandler(const struct ico_uws_context *context,
                             const void *id)
{
	list<CicoSCUwsHandle*>::iterator itr;
	itr = m_uwsHandlerList.begin();
	for (; itr != m_uwsHandlerList.end(); ++itr) {
        ICO_DBG("handle->context=%p handle->id=%p context=%p id=%p",
                (*itr)->uwsContext, (*itr)->id, context, id);
        if (((*itr)->uwsContext == context) &&
            ((*itr)->id == id)) {
            return *itr;
		}
    }
    return NULL;
}

CicoSCUwsHandle*
CicoSCServer::findUwsHandler(const Ecore_Fd_Handler *ecoreFdHandler)
{
	list<CicoSCUwsHandle*>::iterator itr;
	itr = m_uwsHandlerList.begin();
	for (; itr != m_uwsHandlerList.end(); ++itr) {
        if ((*itr)->ecoreFdHandler == ecoreFdHandler) {
            return *itr;
		}
    }
    return NULL;
}

CicoSCUwsHandle*
CicoSCServer::findUwsHandler(const string & appid)
{
	list<CicoSCUwsHandle*>::iterator itr;
	itr = m_uwsHandlerList.begin();
	for (; itr != m_uwsHandlerList.end(); ++itr) {
        ICO_DBG("handle->id=%p handle->appid=%s appid=%s",
                (*itr)->id, (*itr)->appid.c_str(), appid.c_str());
        if ((*itr)->appid == appid) {
            return *itr;
		}
    }
    return NULL;
}

void
CicoSCServer::addPollFd(CicoSCUwsHandle *handle)
{
    ICO_DBG("CicoSCServer::addPollFd Enter(fd=%d)", handle->fd);
    Ecore_Fd_Handler_Flags flags = (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR);

    handle->ecoreFdHandler = ecore_main_fd_handler_add(handle->fd, flags,
                                                       ecoreFdCallback,
                                                       this, NULL, NULL);
    ICO_DBG("CicoSCServer::addPollFd Leave");
}

void
CicoSCServer::delPollFd(CicoSCUwsHandle *handle)
{
    ICO_DBG("CicoSCServer::delPollFd Enter");
    ecore_main_fd_handler_del(handle->ecoreFdHandler);
    handle->ecoreFdHandler = NULL;
    ICO_DBG("CicoSCServer::delPollFd Enter");
}

Eina_Bool
CicoSCServer::ecoreFdCallback(void *data, Ecore_Fd_Handler *handler)
{
    ICO_DBG("CicoSCServer::ecoreFdCallback Enter");

    int fdFlags = 0;
#if 0
    Eina_Bool flag = ecore_main_fd_handler_active_get(handler, ECORE_FD_READ);
    if (Eina_True == flag) {
        fdFlags |= ECORE_FD_READ;
    }

    Eina_Bool flag = ecore_main_fd_handler_active_get(handler, ECORE_FD_ERROR);
    if (Eina_True == flag) {
        fdFlags |= ECORE_FD_ERROR;
    }

    Eina_Bool flag = ecore_main_fd_handler_active_get(handler, ECORE_FD_WRITE);
    if (Eina_True == flag) {
        fdFlags |= ECORE_FD_WRITE;
    }
#endif

    CicoSCUwsHandle *handle = NULL;
    handle =  static_cast<CicoSCServer*>(data)->findUwsHandler(handler);
    if (NULL != handle) {
        static_cast<CicoSCServer*>(data)->dispatch(handle, fdFlags);
    }

    ICO_DBG("CicoSCServer::ecoreFdCallback Leave");
    return ECORE_CALLBACK_RENEW;
}

void
CicoSCServer::dispatch(const CicoSCUwsHandle *handle, int flags)
{
    ICO_DBG("CicoSCServer::dispatch Enter");

    if (NULL == handle) {
        ICO_WRN("handle is null");
        ICO_DBG("CicoSCServer::dispatch Leave");
        return;
    }
    handle->dump(); //TODO

    ico_uws_service(handle->uwsContext);

    list<CicoSCCommand*>::iterator itr;
    itr = m_recvCmdQueue.begin();
    while(itr != m_recvCmdQueue.end()) {
        ICO_DBG("Deque command(0x%08X)", (*itr)->cmdid);
        CicoSCCommand *cmd = *itr;
        itr = m_recvCmdQueue.erase(itr);
        switch (cmd->cmdid & MSG_CMD_TYPE_MASK) {
        case MSG_CMD_TYPE_WINCTRL:
            ICO_DBG("command : MSG_CMD_TYPE_WINCTRL");
            m_windowCtrl->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_INPUTCTRL:
            ICO_DBG("command : MSG_CMD_TYPE_INPUTCTRL");
            m_inputCtrl->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_USERMGR:
            ICO_DBG("command : MSG_CMD_TYPE_USERMGR");
            m_userMgr->handleCommand(cmd);
            break;
        case MSG_CMD_TYPE_RESOURCEMGR:
            ICO_DBG("command : MSG_CMD_TYPE_RESOURCEMGR");
            m_resourceMgr->handleCommand(*cmd);
            break;
        default:
            ICO_DBG("command : unknown");
            break;
        }
        delete cmd;
    }

    if (NULL == handle->ecoreFdHandler) {
        ICO_ERR("ecoreFdHandler is null");
        ICO_DBG("CicoSCServer::dispatch Leave");
        return;
    }

    Eina_Bool flag = ecore_main_fd_handler_active_get(handle->ecoreFdHandler,
												      ECORE_FD_WRITE);
    if (EINA_TRUE == flag) {
        ICO_DBG("start send message");
        list<CicoSCMessage*>::iterator send_itr;
        send_itr = m_sendMsgQueue.begin();
        while (send_itr != m_sendMsgQueue.end()) {
            ICO_DBG("m_sendMsgQueue.size=%d", m_sendMsgQueue.size());
            CicoSCMessage* msg = *send_itr;
            CicoSCUwsHandle *sendHandle = findUwsHandler(msg->getSendToAppid());
            if (handle != sendHandle) {
                ++send_itr;
                continue;
            }
            send_itr = m_sendMsgQueue.erase(send_itr);
            ICO_DBG("Deque Message(id=%d)", msg->getId());
            if ((NULL != sendHandle) && (true == sendHandle->serviceFlag)) {
                sendHandle->dump();
                ICO_DBG("<<<SEND id=%08X msg=%s",
                        sendHandle->id, msg->getData());
                ICO_DBG("ico_usw_send called.");
                ico_uws_send(sendHandle->uwsContext, sendHandle->id,
                             (unsigned char *)msg->getData(),
                             strlen(msg->getData()));

                delete msg;

                usleep(200);
            }
        }
        ecore_main_fd_handler_active_set(handle->ecoreFdHandler,
            (Ecore_Fd_Handler_Flags)(ECORE_FD_READ | ECORE_FD_ERROR));
    }

    ICO_DBG("CicoSCServer::dispatch Leave");
}

int
CicoSCServer::sendMessage(const string & appid, CicoSCMessage* msg)
{
    ICO_DBG("CicoSCServer::sendMessage Enter(appid=%s, msg=%s)",
            appid.c_str(), msg->getData());

    msg->setSendToAppid(appid);
    ICO_DBG("Enque Message(id=%d)", msg->getId());
    m_sendMsgQueue.push_back(msg);

    CicoSCUwsHandle *handle = findUwsHandler(appid);
    if (NULL != handle) {
		ecore_main_fd_handler_active_set(handle->ecoreFdHandler,
			(Ecore_Fd_Handler_Flags)(ECORE_FD_READ |
								     ECORE_FD_WRITE |
									 ECORE_FD_ERROR));
		dispatch(handle, 0);
    }

    ICO_DBG("CicoSCServer::sendMessage Leave(EOK)");
    return ICO_SYC_EOK;
}

int
CicoSCServer::sendMessageToHomeScreen(CicoSCMessage* msg)
{
    const CicoSCUser *loginUser = m_userMgr->getLoginUser();
    if (NULL == loginUser) {
        ICO_WRN("homescree unknow");
        return ICO_SYC_ENOENT;
    }
    return sendMessage(loginUser->homescreen, msg);
}

void
CicoSCServer::uwsCallbackImpl(const struct ico_uws_context *context,
                              const ico_uws_evt_e event,
                              const void *id,
                              const ico_uws_detail *detail,
                              void *user_data)
{
    ICO_DBG("CicoSCServer::uwsCallbackImpl Enter");

    // find hanle
    CicoSCUwsHandle *handle = findUwsHandler(context, id);
    // If not found handle, create new handle
    if (NULL == handle) {
        handle = new CicoSCUwsHandle();
        m_uwsHandlerList.push_back(handle);
        handle->uwsContext = (struct ico_uws_context*)context;
        handle->id = (void*)(id);
        handle->serviceFlag = false;
    }

    switch (event) {
    case ICO_UWS_EVT_OPEN:
    {
        ICO_DBG(">>>RECV ICO_UWS_EVT_OPEN(id=%08x)", (int)id); 
        break;
    }
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=%08x)", (int)id);
        m_uwsContext = NULL;
        break;
    case ICO_UWS_EVT_RECEIVE:
	{
        ICO_DBG(">>>RECV ICO_UWS_EVT_RECEIVE(id=%08x, msg=%s, len=%d)", 
                (int)id, (char *)detail->_ico_uws_message.recv_data,
                detail->_ico_uws_message.recv_len);

        // convert message to command
        CicoSCCommand *cmd = new CicoSCCommand();
        CicoSCCommandParser cmdParser;
        cmdParser.parse((const char*)detail->_ico_uws_message.recv_data, *cmd);

        // update handle appid
        if (cmd->cmdid == MSG_CMD_SEND_APPID) {
            if (0 == cmd->appid.length()) {
                ICO_WRN("command argument invalid appid null");
                break;
            }
            handle->appid = cmd->appid;
            handle->serviceFlag = true;
            ICO_DBG("handle.appid=%s", handle->appid.c_str());
    ecore_main_fd_handler_active_set(handle->ecoreFdHandler,
                                     (Ecore_Fd_Handler_Flags)(ECORE_FD_READ |
                                     ECORE_FD_WRITE |
                                     ECORE_FD_ERROR));
 

            break;
        }
        
        // Enqueue command
        ICO_DBG("Enque command(0x%08X)", cmd->cmdid);
        m_recvCmdQueue.push_back(cmd);
        break;
	}
    case ICO_UWS_EVT_ERROR:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ERROR(id=%08x, err=%d)", 
                (int)id, detail->_ico_uws_error.code);
        break;
    case ICO_UWS_EVT_ADD_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ADD_FD(id=%08x, fd=%d)",
                    (int)id, detail->_ico_uws_fd.fd);
        handle->fd = detail->_ico_uws_fd.fd;
        addPollFd(handle);

        break;
    case ICO_UWS_EVT_DEL_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_DEL_FD(id=%d, fd=%d)",
                    (int)id, detail->_ico_uws_fd.fd);
        delPollFd(handle);
		delete handle;
        break;
    default:
        break;
    }
    ICO_DBG("CicoSCServer::uwsCallbackImpl Leave");
}

/*--------------------------------------------------------------------------*/
/*
 * @brief   callback_uws
 *          callback function from UWS
 *
 * @param[in]   context             context * @param[in]   event               event kinds
 * @param[in]   id                  client id
 * @param[in]   detail              event detail
 * @param[in]   data                user data
 * @return      none
 */
/*--------------------------------------------------------------------------*/
void
CicoSCServer::uwsCallback(const struct ico_uws_context *context,
                           const ico_uws_evt_e event,
                           const void *id,
                           const ico_uws_detail *detail,
                           void *user_data)
{
    ICO_DBG("callback_uws: context=%08x id=%08x", (int)context, (int)id);

	CicoSCServer* server = static_cast<CicoSCServer*>(user_data);
    server->uwsCallbackImpl(context, event, id, detail, user_data);
}
// vim:set expandtab ts=4 sw=4:
