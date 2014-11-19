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
 *  @file   CicoOSClient.cpp
 *
 *  @brief  This file implementation of CicoOSClient class
 */
//==========================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <sys/types.h>
#include <unistd.h>

using namespace boost::property_tree;
using namespace std;

#include "CicoOSClient.h"

#include <ico_log.h>
#include "ico_syc_error.h"
#include "ico_syc_msg_cmd_def.h"

//==========================================================================
//  private static variable
//==========================================================================
CicoOSClient* CicoOSClient::ms_myInstance = NULL;

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoOSClient::CicoOSClient()
    :m_uwsContext(NULL), m_id(NULL), m_seqNum(seqMin)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoOSClient::~CicoOSClient()
{
    if (NULL != m_uwsContext) {
        ico_uws_close(m_uwsContext);
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief   get CicoOSClient instance
 *
 *  @return CicoOSClient instance
 */
//--------------------------------------------------------------------------
CicoOSClient*
CicoOSClient::getInstance()
{
    if (NULL == ms_myInstance) {
        ms_myInstance = new CicoOSClient();
    }

    return ms_myInstance;
}

//--------------------------------------------------------------------------
/**
 *  @brief   connect
 *
 *  @param [in] port        websocket port
 *  @param [in] protocol    websocket protocol name
 *
 *  @return ICO_SYC_EOK on success, other on error
 *  @retval ICO_SYC_EOK     success
 *  @retval ICO_SYC_ENOSYS  error(connection fail)
 */
//--------------------------------------------------------------------------
bool
CicoOSClient::connect()
{
    ICO_TRA("CicoOSClient::connect Enter");

    /* create context */
    m_uwsContext = ico_uws_create_context(DEF_WS_CLI_HS_PORT,
                                          DEF_WS_CLI_HS_PROTOCOL);
    if (NULL == m_uwsContext) {
        ICO_ERR("ico_uws_create_context() failed.");
        ICO_TRA("CicoOSClient::connect Leave false");
        return false;
    }

    /* set callback */
    int ret = ico_uws_set_event_cb(m_uwsContext, uwsReceiveEventCB,
                                   (void *)this);
    if (ret != ICO_UWS_ERR_NONE) {
        ICO_ERR("ico_uws_set_event_cb() failed(%d).", ret);
        ICO_TRA("CicoOSClient::connect Leave false");
        return false;
    }

    ecore_timer_add(timerCount, CicoOSClient::ecoreTimerCB, this);

    ICO_TRA("CicoOSClient::connect Leave ture");

    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  control sequence number
 */
//--------------------------------------------------------------------------
void CicoOSClient::updateSeqNum()
{
    m_seqNum++;
    if (seqMax < m_seqNum) {
        m_seqNum = seqMin;
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  get sequence number
 */
//--------------------------------------------------------------------------
int CicoOSClient::getSeqNum() const
{
    return m_seqNum;
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
bool
CicoOSClient::sendLaunchMessage(const string& op_app)
{
    ICO_TRA("CicoOSClient::sendMessage Enter(%s)", op_app.c_str());
    if ((NULL == m_uwsContext) && (NULL == m_id)) {
        ICO_TRA("CicoOSClient::sendMessage Leave(false)");
        return false;
    }
    ico_uws_service(m_uwsContext);

    ptree pt;
    pt.put("command", MSG_CMD_WIN_CHANGE);
    pt.put("source.pid",    (int)getpid());
    pt.put("source.seq_no", getSeqNum());
    updateSeqNum();
    pt.put("arg.appid",   op_app);
    pt.put("arg.zone",    "Center");
    pt.put("arg.visible", 1);
    stringstream ss;
    write_json(ss, pt, false);
    string m = ss.str();

    size_t  l = m.length();
    char b[l+1];
    strcpy(b, m.c_str());
    ICO_DBG("msg:%d, %s", l, b);
    ico_uws_send(m_uwsContext, (void*)m_id, (unsigned char*)b, l);
    ico_uws_service(m_uwsContext);
#if 0
    usleep(200);
#endif

    ICO_TRA("CicoOSClient::sendMessage Leave(true)");
    return true;
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
CicoOSClient::uwsReceiveEventCB(const struct ico_uws_context* context,
                                const ico_uws_evt_e           event,
                                const void*                   id,
                                const ico_uws_detail*         detail,
                                void*                         user_data)
{
    if (NULL == user_data) {
        ICO_ERR("user_data is NULL");
        return;
    }

    CicoOSClient* cosc = static_cast<CicoOSClient*>(user_data);
    cosc->receiveEventCB(context, event, id, detail, user_data);
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
CicoOSClient::receiveEventCB(const struct ico_uws_context* context,
                             const ico_uws_evt_e           event,
                             const void*                   id,
                             const ico_uws_detail*         detail,
                             void*                         user_data)
{
//    ICO_TRA("CicoOSClient::receiveEventCB Enter");

    switch (event) {
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=%p)", id);
//        ICO_TRA("CicoOSClient::receiveEventCB Leave");
        return;
    case ICO_UWS_EVT_ERROR:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ERROR(id=%p, err=%d)",
                id, detail->_ico_uws_error.code);
//        ICO_TRA("CicoOSClient::receiveEventCB Leave");
        return;
    default:
        break;
    }

    if (id != m_id) {
        ICO_DBG(">>>CHANGE id %x -> %x", m_id, id);
        m_id = id;
    }

    switch (event) {
    case ICO_UWS_EVT_OPEN:
        ICO_DBG(">>>RECV ICO_UWS_EVT_OPEN(id=%p)", id);
        break;
    case ICO_UWS_EVT_CLOSE:
        ICO_DBG(">>>RECV ICO_UWS_EVT_CLOSE(id=%p)", id);
        break;
    case ICO_UWS_EVT_RECEIVE:
    {
        ICO_DBG(">>>RECV ICO_UWS_EVT_RECEIVE(id=%p, msg=%s, len=%d)",
                id, (char *)detail->_ico_uws_message.recv_data,
                detail->_ico_uws_message.recv_len);
        break;
    }
    case ICO_UWS_EVT_ADD_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_ADD_FD(id=%p, fd=%d)",
                id, detail->_ico_uws_fd.fd);
        break;
    case ICO_UWS_EVT_DEL_FD:
        ICO_DBG(">>>RECV ICO_UWS_EVT_DEL_FD(id=%p, fd=%d)",
                id, detail->_ico_uws_fd.fd);
        break;
    default:
        break;
    }
//    ICO_TRA("CicoOSClient::receiveEventCB Leave");
}

Eina_Bool
CicoOSClient::ecoreTimerCB(void *data)
{
    CicoOSClient* cosc = (CicoOSClient*)data;
    cosc->refresh();
    return ECORE_CALLBACK_RENEW;
}

bool
CicoOSClient::refresh()
{
    if (NULL == m_uwsContext) {
        ICO_TRA("NG refresh");
        return false;
    }
    ico_uws_service(m_uwsContext);
    return true;
}

// vim:set expandtab ts=4 sw=4:
