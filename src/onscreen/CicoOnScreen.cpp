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
#include "CicoOnScreen.h"
#include "CicoOSPopWindow.h"
#include <Ecore.h>
#include <Ecore_Wayland.h>
#include "CicoOSClient.h"

using namespace std;

//==========================================================================
// static members
//==========================================================================
CicoOnScreen * CicoOnScreen::os_instance=NULL;
uint32_t    CicoOnScreen::surfaceid = 0;

//==========================================================================
// functions
//==========================================================================

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::CicoOnScreen
 *          Constractor
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
CicoOnScreen::CicoOnScreen(void)
{
    ICO_TRA("CicoOnScreen::CicoOnScreen Enter");
    m_request = NULL;
    m_del = false;
    m_reserve = NULL;
    ICO_TRA("CicoOnScreen::CicoOnScreen Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::~CicoOnScreen
 *          Destractor
 *
 * @param[in]   none
 * @return      none
 */
//--------------------------------------------------------------------------
CicoOnScreen::~CicoOnScreen(void)
{
//  ICO_TRA("CicoOnScreen::~CicoOnScreen Enter");
    list<CicoOSPopWindow*>::iterator p = m_mngWin.begin();
    while (p != m_mngWin.end()) {
        CicoOSPopWindow* pt = *p;
        pt->removeMainWindow();
        delete pt;
    }
    m_mngWin.clear();
    p = m_waitMngWin.begin();
    while (p != m_waitMngWin.end()) {
        CicoOSPopWindow* pt = *p;
        pt->removeMainWindow();
        delete pt;
    }
    m_waitMngWin.clear();

    delete m_request;
    m_request = NULL;
    m_del = false;
//  ICO_TRA("CicoOnScreen::~CicoOnScreen Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::StartOnScreen
 *          Start on screen
 *
 * @param[in]   none
 * @return      none
 * @return true on success, false on error
 */
//--------------------------------------------------------------------------
bool
CicoOnScreen::StartOnScreen(void)
{
    ICO_TRA("Enter");

    ico_syc_connect(EventCallBack, NULL);
    // save instance pointer
    os_instance = this;

    // Initialize
    ecore_evas_init();

    CicoOSClient* cosc = CicoOSClient::getInstance();
    cosc->connect();

    // create surface
    if (NULL == m_reserve) {
        m_reserve = new CicoOSPopWindow(NOTIFICATION_TYPE_NONE);
        m_reserve->createMainWindow();
    }

    // set notification callback function
    notiservice_.SetCallback(NotificationCallback, this);

    ICO_TRA("Leave(true)");
    return true;
}

bool CicoOnScreen::insertNoti(notification_h noti_h)
{
    ICO_TRA("Enter");
    CicoOSPopWindow* w = new CicoOSPopWindow(noti_h);
    if (NULL == w) {
        ICO_ERR("FAIL new class");
        ICO_TRA("Leave (false)");
        return false;
    }
    if (! w->Empty()) {
        if (NOTIFICATION_TYPE_NOTI == w->GetType()) {
            m_waitMngWin.push_back(w);
            w = NULL;
        }
    }
    if (NULL != w) {
        delete w;
        ICO_TRA("Leave (false)");
        return false;
    }
    ICO_TRA("Leave (true)");
    return true;
}

bool CicoOnScreen::deleteNoti(int priv_id)
{
    ICO_TRA("Enter");
    list<CicoOSPopWindow*>::iterator p = m_mngWin.begin();
    for (; p != m_mngWin.end(); ++p) {
        CicoOSPopWindow* w = *p;
        if (priv_id == w->GetPrivId()) {
            w->hidePopup();
        }
    }
    list<CicoOSPopWindow*> tmp;
    p = m_waitMngWin.begin();
    for (; p != m_waitMngWin.end(); ++p) {
        CicoOSPopWindow* w = *p;
        if (priv_id == w->GetPrivId()) {
            tmp.push_back(w);
        }
    }
    p = tmp.begin();
    for (; p != tmp.end(); ++p) {
        CicoOSPopWindow* w = *p;
        m_waitMngWin.remove(w);
        delete w;
    }
    if (NULL != m_request) {
        if (priv_id == m_request->GetPrivId()) {
            m_del = true;
        }
    }
    ICO_TRA("Leave (true)");
    return true;
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::NotificationCallback
 *          Notification callback function
 *
 * @param[in]  data        The user data passed from the callback
 *                         registration function
 * @param[in]  type        The type of notification
 * @return     none
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::NotificationCallback(void *data, notification_type_e type,
                                   notification_op *op_list, int num_op)
{
    ICO_TRA("Enter");
    bool bInsFlag = false;
    for (int i = 0; i < num_op; i++) {
        notification_op_type_e op_type = op_list[i].type;
        int priv_id = op_list[i].priv_id;
        notification_h noti_h = op_list[i].noti;
        ICO_DBG("RECV notification op_type=%d priv_id=%d noti=0x%08x",
                op_type, priv_id, noti_h);

        switch (op_type) {
        case NOTIFICATION_OP_INSERT :
        {
            ICO_DBG("NOTIFICATION_OP_INSERT");
            if (true == os_instance->insertNoti(noti_h)) {
                bInsFlag = true;
            }
            break;  // break of switch op_type
        }
        case NOTIFICATION_OP_UPDATE :
            ICO_DBG("NOTIFICATION_OP_UPDATE");
            break;  // break of switch op_type
        case NOTIFICATION_OP_DELETE:
        {
            ICO_DBG("NOTIFICATION_OP_DELETE");
            os_instance->deleteNoti(priv_id);
            break;  // break of switch op_type
        }
        case NOTIFICATION_OP_DELETE_ALL:
            ICO_DBG("NOTIFICATION_OP_DELETE_ALL");
            break;  // break of switch op_type
        case NOTIFICATION_OP_REFRESH:
            ICO_DBG("NOTIFICATION_OP_REFRESH");
            break;  // break of switch op_type
        case NOTIFICATION_OP_SERVICE_READY:
            ICO_DBG("NOTIFICATION_OP_SERVICE_READY");
            break;  // break of switch op_type
        default :
            ICO_DBG("UNKOWN OP_TYPE(%d)", (int)op_type);
            break;  // break of switch op_type
        }
    }

    if (true == bInsFlag) {
        os_instance->requestShowSC();
    }

    ICO_TRA("Leave");
}

//--------------------------------------------------------------------------
/**
 * @brief   CicoOnScreen::GetResourdeId
 *          Get resource id (sourface id)
 *
 * @param      nothing
 * @return     resource id
 */
//--------------------------------------------------------------------------
uint32_t
CicoOnScreen::GetResourdeId(void)
{
    return CicoOnScreen::surfaceid;
}

bool CicoOnScreen::requestShowSC()
{
    ICO_TRA("Enter");
    if (NULL != m_request) {
        ICO_TRA("Leave false(request now)");
        return false;
    }
    if (m_waitMngWin.empty()) {
        ICO_TRA("Leave false(nothing window)");
        return false;
    }

    list<CicoOSPopWindow*>::iterator p;
    bool r;
    while (1) {
        m_del = false;
        p = m_waitMngWin.begin();
        m_request = *p;
        m_waitMngWin.remove(m_request);
        r = m_request->showPopup();
        if (true == r) {
            r = m_request->acquireRes();
            if (true == r)  {
                if (true == m_del)  {
                    m_request->hidePopup();
                    m_del = false;
                }
            }
            else    {
                ICO_ERR("_____ Fail Acquire Resource(%d)", m_request->GetPrivId());
            }
        }
        else    {
            ICO_ERR("_____ Fail SHOW POP REQUEST(%d)", m_request->GetPrivId());
        }
        if (true == r)      break;
        delete m_request;
        m_request = NULL;
        if (m_waitMngWin.empty())   break;
    }
    ICO_TRA("Leave %s", r ? "true": "false");
    return r;
}

//--------------------------------------------------------------------------
/**
 *  @brief   callback for system controller
 *
 *  @param [in] event       kind of event
 *  @param [in] detail      detail
 *  @param [in] user_data   user data
 */
//--------------------------------------------------------------------------
void
CicoOnScreen::EventCallBack(const ico_syc_ev_e event,
                            const void *detail, void *user_data)
{
    ICO_TRA("Enter(event %d, %x, %x)", (int)event, detail, user_data);

    ico_syc_res_info_t *ri = (ico_syc_res_info_t*) detail;
    if (NULL == ri) {
        ICO_ERR("____ CALLBACK NG PARAM");
        return;
    }
    switch (event) {
    case ICO_SYC_EV_RES_ACQUIRE:
        ICO_TRA("_____ ICO_SYC_EV_RES_ACQUIRE");
        break;  // break of switch event
    case ICO_SYC_EV_RES_DEPRIVE:
        ICO_TRA("_____ ICO_SYC_EV_RES_DEPRIVE");
        break;  // break of switch event
    case ICO_SYC_EV_RES_WAITING:
        ICO_TRA("_____ ICO_SYC_EV_RES_WAITING");
        break;  // break of switch event
    case ICO_SYC_EV_RES_REVERT:
        ICO_TRA("_____ ICO_SYC_EV_RES_REVERT");
        break;  // break of switch event
    case ICO_SYC_EV_RES_RELEASE:
    {
        ICO_TRA("_____ ICO_SYC_EV_RES_RELEASE");
        if (NULL == ri->window) {
            ICO_TRA("_____ no WINDOW");
            break;  // break of switch event
        }
        os_instance->releaseWindow(ri->window->resourceId);
        break;  // break of switch event
    }
    case ICO_SYC_EV_RES_WINDOW_ID:
    {
        ICO_TRA("_____ ICO_SYC_EV_RES_WINDOW_ID");
        if (NULL == ri->window) {
            ICO_TRA("_____ no WINDOW");
            break;  // break of switch event
        }
        CicoOnScreen::surfaceid = ri->window->resourceId;
        ICO_TRA("_____ surfaceid=%08x", CicoOnScreen::surfaceid);
        break;  // break of switch event
    }
    default:
        ICO_TRA("_____ UNKNOWN event(%d)", (int)event);
        break;  // break of switch event
    }
    ICO_TRA("Leave");
}

bool
CicoOnScreen::releaseWindow(uint32_t resourceId)
{
    ICO_TRA("Enter(%08x)", resourceId);

    bool bR = false;
    list<CicoOSPopWindow*> tmp;

    list<CicoOSPopWindow*>::iterator p = m_mngWin.begin();
    for (; p != m_mngWin.end(); ++p) {
        CicoOSPopWindow* w = *p;
        if (resourceId == w->m_resourceId) {
            tmp.push_back(w);
        }
    }
    p = tmp.begin();
    for (; p != tmp.end(); ++p) {
        CicoOSPopWindow* w = *p;
        if (w == m_request) {
            m_request = NULL;
        }
        m_mngWin.remove(w);
        delete w;
        bR = true;
    }
    tmp.clear();

    if (m_request)  {
        delete m_request;
        m_request = NULL;
        m_del = false;

        (void) requestShowSC();
    }
    ICO_TRA("Leave %s", bR? "true": "false");
    return bR;
}
// vim: set expandtab ts=4 sw=4:
