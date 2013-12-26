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
 *  @file   CicoSCResourceManager.cpp
 *
 *  @brief  This file implementation of CicoSCResourceManager class
 */
//==========================================================================

#include <sstream>
using namespace std;

#include "CicoSCResourceManager.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_mrp_resource_private.h"

#include "ico_syc_msg_cmd_def.h"
#include "CicoSystemConfig.h"
#include "CicoConf.h"
#include "CicoSCCommand.h"
#include "CicoSCPolicyManager.h"
#include "CicoSCLifeCycleController.h"
#include "CicoAilItems.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"
#include "CicoSCPolicyDef.h"

struct CompDisplayResourceRequest
{
    CompDisplayResourceRequest(const resource_request_t *req)
        : m_req(req) {}

    bool operator() (const resource_request_t *req)
    {
        if (NULL == req) {
            return false;
        }
        ICO_DBG("appid=%s:%s", req->appid, m_req->appid);
        if ((0 == strcmp(m_req->appid, req->appid)) &&
            (m_req->pid == req->pid) &&
            ((m_req->resid & RESID_TYPE_MASK) == (req->resid & RESID_TYPE_MASK)) &&
            (m_req->surfaceid == req->surfaceid)) {
            return true;
        }
        return false;
    }

    const resource_request_t *m_req;
};

struct CompSoundResourceRequest
{
    CompSoundResourceRequest(const resource_request_t *req)
        : m_req(req) {}

    bool operator() (const resource_request_t *req)
    {
        if (NULL == req) {
            return false;
        }

        ICO_DBG("appid=%s:%s", req->appid, m_req->appid);
        if ((0 == strcmp(m_req->appid, req->appid)) &&
            (m_req->pid == req->pid) &&
            ((m_req->resid & RESID_TYPE_MASK) == (req->resid & RESID_TYPE_MASK)) &&
            (m_req->soundid == req->soundid)) {
            return true;
        }
        return false;
    }

    const resource_request_t *m_req;
};

struct CompInputResourceRequest
{
    CompInputResourceRequest(const resource_request_t *req)
        : m_req(req) {}

    bool operator() (const resource_request_t *req)
    {
        if (NULL == req) {
            return false;
        }

        ICO_DBG("appid=%s:%s", req->appid, m_req->appid);
        if ((0 == strcmp(m_req->appid, req->appid)) &&
            (0 == strcmp(m_req->device, req->device)) &&
            (m_req->pid == req->pid) &&
            ((m_req->resid & RESID_TYPE_MASK) == (req->resid & RESID_TYPE_MASK))) {
            return true;
        }
        return false;
    }

    const resource_request_t *m_req;
};


CicoSCResourceManager::CicoSCResourceManager()
    : m_policyMgr(NULL), m_winCtrl(NULL), m_inputCtrl(NULL),
      m_animaName("Fade"), m_animaTime(400)
{
    m_policyMgr = new CicoSCPolicyManager(this);

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    list<resource_request_t*> l;

    {
        // initialize display current request and waiting queue
        const vector<CicoSCDisplayConf*>& dispConfList
            = sysConf->getDisplayConfList();
        vector<CicoSCDisplayConf*>::const_iterator itr;
        itr = dispConfList.begin();
        for (; itr != dispConfList.end(); ++itr) {
            vector<CicoSCDisplayZoneConf*>::iterator itr2;
            itr2 = (*itr)->zoneConfList.begin();
            for (; itr2 != (*itr)->zoneConfList.end(); ++itr2) {
                m_curDispResOwerReq[(*itr2)->id] = NULL;
            }
        }
    }

    {
        // initialize sound current request and waiting queue
        const vector<CicoSCSoundConf*>& soundConfList
            = sysConf->getSoundConfList();
        vector<CicoSCSoundConf*>::const_iterator itr;
        itr = soundConfList.begin();
        for (; itr != soundConfList.end(); ++itr) {
            vector<CicoSCSoundZoneConf*>::iterator itr2;
            itr2 = (*itr)->zoneConfList.begin();
            for (; itr2 != (*itr)->zoneConfList.end(); ++itr2) {
                m_curSoundResReq[(*itr2)->id] = NULL;
                m_soundReqQueue[(*itr2)->id]  = l;
            }
        }
    }

   {
        // initialize input current request and waiting queue
        const vector<CicoSCInputDevConf*>& inputDevConfList
            = sysConf->getInputDevConfList();
        vector<CicoSCInputDevConf*>::const_iterator itr;
        itr = inputDevConfList.begin();
        for (; itr != inputDevConfList.end(); ++itr) {
            vector<CicoSCSwitchConf*>::iterator itr2;
            itr2 = (*itr)->switchConfList.begin();
            for (; itr2 != (*itr)->switchConfList.end(); ++itr2) {
                m_curInputResReq[(*itr2)->id] = NULL;
                m_inputReqQueue[(*itr2)->id]  = l;
            }
        }
    }
}

CicoSCResourceManager::~CicoSCResourceManager()
{
    delete m_policyMgr;
}

int
CicoSCResourceManager::initialize(void)
{
    ICO_TRA("CicoSCResourceManager::initialize Enter");

    int ret = ICO_SYC_EOK;

    ret = ico_syc_mrp_init(enforceSound, this);
    if (ICO_SYC_EOK != ret) {
        ICO_TRA("CicoSCResourceManager::initialize Leave");
        return ret;
    }

    ret = m_policyMgr->initialize();
    if (ICO_SYC_EOK != ret) {
        ICO_TRA("CicoSCResourceManager::initialize Leave");
        return ret;
    }


    ICO_TRA("CicoSCResourceManager::initialize Leave");
    return ret;
}
  
void
CicoSCResourceManager::terminate(void)
{
    ICO_TRA("CicoSCResourceManager::terminate Enter");
    m_policyMgr->terminate();
    ico_syc_mrp_term();
    ICO_TRA("CicoSCResourceManager::terminate Leave");
}

void
CicoSCResourceManager::handleCommand(const CicoSCCommand &cmd,
                                     bool internal)
{
    ICO_TRA("CicoSCResourceManager::handleCommand Enter"
            "(cmdid=0x%08X internal=%s)",
            cmd.cmdid, internal ? "true" : "false");

    CicoSCCmdResCtrlOpt *opt = (CicoSCCmdResCtrlOpt*)cmd.opt;

    // request command from application or internal
    int reqtype = internal ? REQTYPE_AUTO : REQTYPE_APP;

    if ((MSG_CMD_CREATE_RES == cmd.cmdid) ||
        (MSG_CMD_ACQUIRE_RES == cmd.cmdid)) {
        
        if (true == opt->dispres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_DISPLAY,
                                                         reqtype,
                                                         cmd);
            if (MSG_CMD_CREATE_RES == cmd.cmdid) {
                acquireDisplayResource(req, false);
            }
            else {
                if ((true == m_policyMgr->getRegulation()) &&
                    (false == isMatchDisplayed())) {
                    updateDispResRegulationPreProc(req);
                    updateDisplayResourceRegulation(STID_DRVREGULATION_ON);
                }
                else {
                    acquireDisplayResource(req);
                }
            }

        }
        if (true == opt->soundres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_SOUND,
                                                         reqtype,
                                                         cmd);
            if ((true == m_policyMgr->getRegulation()) &&
                (false == isMatchDisplayed())) {
                updateSoundResRegulationPreProc(req);
                updateSoundResourceRegulation(STID_DRVREGULATION_ON);
            }
            else {
                acquireSoundResource(req);
            }

        }
        if (true == opt->inputres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_INPUT,
                                                         reqtype,
                                                         cmd);
            if ((true == m_policyMgr->getRegulation()) &&
                (false == isMatchDisplayed())) {
                updateInputResRegulationPreProc(req);
                updateInputResourceRegulation(STID_DRVREGULATION_ON);
            }
            else {
                acquireInputResource(req);
            }
        }
    }
    else if ((MSG_CMD_RELEASE_RES == cmd.cmdid) ||
             (MSG_CMD_DESTORY_RES == cmd.cmdid)) {

        if (true == opt->dispres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_DISPLAY,
                                                         reqtype,
                                                         cmd);
            releaseDisplayResource(req);
        }
        if (true == opt->soundres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_SOUND,
                                                         reqtype,
                                                         cmd);
            releaseSoundResource(req);
        }
        if (true == opt->inputres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_INPUT,
                                                         reqtype,
                                                         cmd);
            releaseInputResource(req);
        }
    }
    else {
        ICO_WRN("Unknown command");
    }

    ICO_TRA("CicoSCResourceManager::handleCommand Leave");
}

void
CicoSCResourceManager::setWindowController(CicoSCWindowController *winCtrl)
{
    m_winCtrl = winCtrl;
}

void
CicoSCResourceManager::setInputController(CicoSCInputController *inputCtrl)
{
    m_inputCtrl = inputCtrl;
}

#if 0
resource_request_t * 
CicoSCResourceManager::popDispResReq(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::popDispResReq Enter");

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_dispReqQueue.begin();
    CompDisplayResourceRequest comp(req);
    for (; itr != m_dispReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = find_if(itr->second.begin(), itr->second.end(), comp);
        if (itr->second.end() != itr2) {
            ICO_DBG("Dequeue waiting display request queue zone=%d req=0x%08x",
                    itr->first, *itr2);
            m_dispReqQueue[req->dispzoneid].erase(itr2);
            ICO_TRA("CicoSCResourceManager::popDispResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }

    ICO_TRA("CicoSCResourceManager::popDispResReq Leave(NULL)");
    return NULL;
}
#endif

resource_request_t *
CicoSCResourceManager::popSoundResReq(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::popSoundResReq Enter");

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_soundReqQueue.begin();
    CompSoundResourceRequest comp(req);
    for (; itr != m_soundReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = find_if(itr->second.begin(), itr->second.end(), comp);
        if (itr->second.end() != itr2) {
            ICO_DBG("Dequeue waiting sound request queue"
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    *itr2, itr->first, (*itr2)->soundzone, (*itr2)->appid);
            m_soundReqQueue[req->soundzoneid].erase(itr2);
            ICO_TRA("CicoSCResourceManager::popSoundResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }
    ICO_TRA("CicoSCResourceManager::popSoundResReq Leave(NULL)");
    return NULL;
}

resource_request_t * 
CicoSCResourceManager::popInputResReq(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::popInputResReq Enter");

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_inputReqQueue.begin();
    CompInputResourceRequest comp(req);
    for (; itr != m_inputReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = find_if(itr->second.begin(), itr->second.end(), comp);
        if (itr->second.end() != itr2) {
            ICO_DBG("Dequeue waiting input request queue"
                    "(req=0x%08x input:%d appid=%s)",
                    *itr2, (*itr2)->input, (*itr2)->appid);
            m_inputReqQueue[req->input].erase(itr2);
            ICO_TRA("CicoSCResourceManager::popInputResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }

    ICO_TRA("CicoSCResourceManager::popDispResReq Leave(NULL)");
    return NULL;
}


bool
CicoSCResourceManager::acquireDisplayResource(resource_request_t *newreq,
                                              bool control)
{
    ICO_TRA("CicoSCResourceManager::acquireDisplayResource Enter");

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(newreq->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_TRA("CicoSCResourceManager::acquireDisplayResource Leave(false)");
        return false;
    }

    if ((appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_ALMIGHTY) ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM)   ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM_VISIBLE)) {

        if ((NULL != m_winCtrl) && (true == control)) {
            m_winCtrl->show(newreq->surfaceid,
                            newreq->animation,
                            newreq->animationTime);
        }
        delResourceRequest(newreq);

        ICO_DBG("kind of system application");
        ICO_TRA("CicoSCResourceManager::acquireDisplayResource Leave(true)");
        return true;
    }

    // if exist current ower request, pop request
    resource_request_t *req = findCurDispResOwerReq(newreq);
    int chgzone = -1;
    if (NULL != req) {
        ICO_DBG("dispzoneid=%d/%d zone=%s/%s",
                req->dispzoneid, newreq->dispzoneid,
                req->dispzone, newreq->dispzone);
        if (req->dispzoneid != newreq->dispzoneid) {
            req = popCurDispResOwerReq(req);
            m_policyMgr->releaseDisplayResource(req->dispzoneid, req->prio);
            chgzone = newreq->dispzoneid;
        }
    }

    // if exist in wating request list, pop request
    if (NULL == req) {
        req = popWaitingDispResReq(newreq);
        if (NULL != req) {
            ICO_DBG("dispzoneid=%d/%d zone=%s/%s",
                    req->dispzoneid, newreq->dispzoneid,
                    req->dispzone, newreq->dispzone);
            if (req->dispzoneid != newreq->dispzoneid) {
                chgzone = newreq->dispzoneid;
            }
        }
        else {
            req = newreq;
        }
    }

    // 
    if (req != newreq) {
        // update request data
        req->dispzoneid = newreq->dispzoneid;
        if (NULL != req->dispzone) free(req->dispzone);
        req->dispzone = strdup(newreq->dispzone);
        req->layerid = newreq->layerid;
        if (NULL != req->animation) free(req->animation);
        req->animation = strdup(newreq->animation);
        req->animationTime = newreq->animationTime;

        // delete new request
        delResourceRequest(newreq);
    }
    
    if (false == control) {
        ICO_TRA("Enqueue waiting display resource request"
                "(req=0x%08x appid=%s)", req, req->appid);
        m_waitingDispResReq.push_front(req);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
        return true;
    }

    int type = req->resid & RESID_TYPE_MASK;
    bool state = m_policyMgr->acquireDisplayResource(type,
                                                     req->dispzoneid,
                                                     req->prio);
    if (true == state) {
        updateDisplayResource(req, chgzone);
    }
    else {
        if (-1 != chgzone) {
            // move request window
            m_winCtrl->setGeometry(req->surfaceid, req->dispzone, req->layerid,
                                   req->animation, req->animationTime,
                                   req->animation, req->animationTime);
        }
        else {
            // just in case, hide window
            if (NULL != m_winCtrl) {
                // hide request window
                m_winCtrl->hide(req->surfaceid, NULL, 0);
            }
        }
        ICO_DBG("Enqueue waiting display resource request"
                    "(req=0x%08x appid=%s)", req, req->appid);
        m_waitingDispResReq.push_front(req);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
    }

    ICO_TRA("CicoSCResourceManager::acquireDisplayResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseDisplayResource(resource_request_t *newreq)
{
    ICO_TRA("CicoSCResourceManager::releaseDisplayResource Enter"
            "(newreq=0x%08x)", newreq);

    // if exist in wating request list, pop request
    resource_request_t *req = popWaitingDispResReq(newreq);
    if (NULL != req) {
        delResourceRequest(req);
        delResourceRequest(newreq);
        ICO_TRA("CicoSCResourceManager::releaseDisplayResource Leave");
        return;
    }

    // if exist current ower request, pop request
    req = popCurDispResOwerReq(newreq);
    if (NULL != req) {
        m_policyMgr->releaseDisplayResource(req->dispzoneid, req->prio);
        delResourceRequest(req);
    }
    delResourceRequest(newreq);

    list<resource_request_t*>::iterator itr;
    itr = m_waitingDispResReq.begin();
    for (; itr != m_waitingDispResReq.end(); ++itr) {
        if (NULL != m_curDispResOwerReq[(*itr)->dispzoneid]) {
            continue;
        }
        int type = (*itr)->resid & RESID_TYPE_MASK;
        bool active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr)->dispzoneid,
                                                         (*itr)->prio);
        if (true == active) {
            resource_request_t* popreq = popWaitingDispResReq(*itr);
            updateDisplayResource(popreq);
            m_winCtrl->activeCB(NULL, NULL, popreq->surfaceid, -1);
            break;
        }
    }

    ICO_TRA("CicoSCResourceManager::releaseDisplayResource Leave");
}

bool
CicoSCResourceManager::acquireSoundResource(resource_request_t *newreq)
{
    ICO_TRA("CicoSCResourceManager::acquireSoundResource Enter");

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(newreq->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_TRA("CicoSCResourceManager::acquireSoundResource Leave(false)");
        return false;
    }

    if ((appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_ALMIGHTY) ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM)   ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM_AUDIO)) {

        delResourceRequest(newreq);

        ICO_DBG("kind of system application");
        ICO_TRA("CicoSCResourceManager::acquireSoundResource Leave(true)");
        return true;
    }

    resource_request_t *req = popSoundResReq(newreq);
    if (NULL == req) {
        req = newreq;
    }

    // is change zone?
    CompSoundResourceRequest comp(req);
    if (true == comp(m_curSoundResReq[req->soundzoneid])) {
        ICO_DBG("already acquired appid=%s pid=%d soundid=0x%08X",
                req->appid, req->pid, req->soundid);
        ICO_TRA("CicoSCResourceManager::acquireSoundResource Leave(true)");
        // free new request
        delResourceRequest(newreq);
        return true;
    }

    int type = req->resid & RESID_TYPE_MASK;
    bool state = m_policyMgr->acquireSoundResource(type,
                                                   req->soundzoneid,
                                                   req->prio);
    if (true == state) {
        updateSoundResource(req);
    }
    else {
        ICO_DBG("Enqueue waiting sound request queue"
                "(req=0x%08x zone:%02d:%s appid=%s)",
                req, req->soundzoneid, req->soundzone, req->appid);
        m_soundReqQueue[req->soundzoneid].push_front(req);
    }

    ICO_TRA("CicoSCResourceManager::acquireSoundResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseSoundResource(resource_request_t *newreq)
{
    ICO_TRA("CicoSCResourceManager::releaseSoundResource Enter");

    bool curchg = false;
    CompSoundResourceRequest comp(newreq);
    for (int i = 0; i < (int)m_curSoundResReq.size(); ++i) {
         if (NULL == m_curSoundResReq[i]) {
            continue;
         }

         if (true == comp(m_curSoundResReq[i])) {
            ICO_DBG("Dequeue current sound resource ower request"
                    "(req=0x%08x zoneid=%02d:%s appid=%s)",
                    m_curSoundResReq[i], m_curSoundResReq[i]->dispzoneid,
                    m_curSoundResReq[i]->dispzone,
                    m_curSoundResReq[i]->appid);
            ICO_PRF("CHG_GUI_RES sound   deprived zone=%02d:%s appid=%s",
                    m_curSoundResReq[i]->dispzoneid,
                    m_curSoundResReq[i]->dispzone,
                    m_curSoundResReq[i]->appid);
            ico_syc_mrp_release_sound_resource(m_curSoundResReq[i]);
            delResourceRequest(newreq);
            delResourceRequest(m_curSoundResReq[i]);
            m_curSoundResReq[i] = NULL;
            curchg = true;
            break;
         }
    }

    // If current request is not changed, 
    // remove the request from the waiting queue.
    if (false == curchg) {
       resource_request_t *req = popSoundResReq(newreq);
        if (NULL != req) {
            ico_syc_mrp_release_sound_resource(req);
            delResourceRequest(req);
            req = NULL;
        }
        delResourceRequest(newreq);
        ICO_TRA("CicoSCResourceManager::releaseSoundResource Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_soundReqQueue.begin();
    for (; itr != m_soundReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            if (NULL != m_curSoundResReq[(*itr2)->soundzoneid]) {
                continue;
            }

            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = m_policyMgr->acquireSoundResource(type,
                                                            (*itr2)->soundzoneid,
                                                            (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting sound request queue "
                        "(req=0x%08x zone:%02d:%s appid=%s)",
                        *itr2, itr->first, (*itr2)->soundzone, (*itr2)->appid);
                itr->second.erase(itr2);
                updateSoundResource(req);
                break;
            }
        }
    }

    ICO_TRA("CicoSCResourceManager::releaseSoundResource Leave");
}

bool
CicoSCResourceManager::acquireInputResource(resource_request_t *newreq)
{
    ICO_TRA("CicoSCResourceManager::acquireInputResource Enter");

    resource_request_t *req = popInputResReq(newreq);
    if (NULL == req) {
        req = newreq;
    }

    // is change zone?
    CompInputResourceRequest comp(req);
    if (true == comp(m_curInputResReq[req->input])) {
        ICO_DBG("already acquired appid=%s pid=%d input=0x%08X",
                req->appid, req->pid, req->input);
        ICO_TRA("CicoSCResourceManager::acquireInputResource Leave(true)");
        // free new request
        delResourceRequest(newreq);
        return true;
    }

    bool state = m_policyMgr->acquireInputResource(req->input, req->prio);
    if (true == state) {
        updateInputResource(req);
    }
    else {
        ICO_DBG("Enqueue waiting input request queue"
                "(req=0x%08x input:%d appid=%s)", req, req->input, req->appid);
        m_inputReqQueue[req->input].push_front(req);
    }

    ICO_TRA("CicoSCResourceManager::acquireInputResource Leave(true)");
    return true;
}

void
CicoSCResourceManager::releaseInputResource(resource_request_t *newreq)
{
    ICO_TRA("CicoSCResourceManager::releaseInputResource Enter");
    bool curchg = false;
    CompInputResourceRequest comp(newreq);
    for (int i = 0; i < (int)m_curInputResReq.size(); ++i) {
         if (NULL == m_curInputResReq[i]) {
            continue;
         }

         if (true == comp(m_curInputResReq[i])) {
            if (NULL != m_inputCtrl) {
                m_inputCtrl->delInputApp(m_curInputResReq[i]->appid,
                                         m_curInputResReq[i]->device,
                                         m_curInputResReq[i]->input);
            }
            ICO_DBG("Dequeue current input resource ower request"
                    "(req=0x%08x input=%d appid=%s)",
                    m_curInputResReq[i], m_curInputResReq[i]->input,
                    m_curInputResReq[i]->appid);
            ICO_PRF("CHG_GUI_RES input   deprived input=%d appid=%s",
                    m_curInputResReq[i]->input, m_curInputResReq[i]->appid);
            delResourceRequest(newreq);
            delResourceRequest(m_curInputResReq[i]);
            m_curInputResReq[i] = NULL;
            curchg = true;
            break;
         }
    }

    // If current request is not changed, 
    // remove the request from the waiting queue.
    if (false == curchg) {
       resource_request_t *req = popInputResReq(newreq);
        if (NULL != req) {
            delResourceRequest(req);
            req = NULL;
        }
        delResourceRequest(newreq);
        ICO_TRA("CicoSCResourceManager::releaseInputResource Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_inputReqQueue.begin();
    for (; itr != m_inputReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            if (NULL != m_curInputResReq[(*itr2)->input]) {
                continue;
            }
            bool active = m_policyMgr->acquireInputResource((*itr2)->input,
                                                            (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting input request queue "
                        "(req=0x%08x input:%d appid=%s)",
                        *itr2, (*itr2)->input, (*itr2)->appid);
                itr->second.erase(itr2);
                updateInputResource(req);
                break;
            }
        }
    }
    ICO_TRA("CicoSCResourceManager::releaseInputResource Leave");
}

resource_request_t *
CicoSCResourceManager::newResourceRequest(int resid,
                                          int reqtype,
                                          const CicoSCCommand &cmd)
{
    ICO_TRA("CicoSCResourceManager::newResourceRequest Enter");

    resource_request_t *req = NULL;
    req = (resource_request_t*)calloc(1, sizeof(resource_request_t));
    CicoSCCmdResCtrlOpt *opt = (CicoSCCmdResCtrlOpt*)cmd.opt;
    CicoSystemConfig *systemConfig = CicoSystemConfig::getInstance();

    req->reqtype = reqtype;

    CicoSCLifeCycleController *lifeCycle =
        CicoSCLifeCycleController::getInstance();
    const CicoAilItems* ailItem = lifeCycle->findAIL(cmd.appid);
    if (NULL != ailItem) {
        req->category = ailItem->m_categoryID;
        req->appkind = ailItem->m_kindID;
    }

    req->prio = req->category;
 
    req->released = 0;

    /* set resource id */
    req->resid = RESID_TYPE_BASIC;
    if (1 == opt->type) {
        req->resid = RESID_TYPE_INTERRUPT;
    }
    req->resid |= resid;

    req->resid |= RESID_CMD_RELEASE;
    if (MSG_CMD_ACQUIRE_RES == cmd.cmdid) {
        req->resid |= RESID_CMD_ACQUIRE;
    }

    /* set application information */
    req->appid = strdup(cmd.appid.c_str());
    req->pid   = cmd.pid;
    req->state = RES_STATE_WAITING;

    if (resid == RESID_KIND_DISPLAY) {
        req->dispzone   = strdup(opt->dispzone.c_str());
        req->dispzoneid = systemConfig->getDizplayZoneIdbyFullName(req->dispzone);
        req->layerid    = opt->layerid;
        req->winname    = strdup(opt->winname.c_str());
        req->surfaceid  = opt->surfaceid;
        req->id         = opt->surfaceid;;
        req->animation  = strdup(opt->animation.c_str());
        req->animationTime = opt->animationTime;
    }
    else if (resid == RESID_KIND_SOUND) {
        req->soundzone   = strdup(opt->soundzone.c_str());
        req->soundzoneid = systemConfig->getSoundZoneIdbyFullName(req->soundzone);
        ICO_DBG("req->soundzoneid=%d", req->soundzoneid);
        req->soundname   = strdup(opt->soundname.c_str());
        req->soundid     = opt->soundid;
        req->soundadjust = opt->adjust;
        req->id          = opt->soundid;
    }
    else if (resid == RESID_KIND_INPUT) {
        req->device = strdup(opt->device.c_str());
        req->input  = opt->input;
        req->id     = opt->input;
    }

    ICO_TRA("CicoSCResourceManager::newResourceRequest Leave"
            "(req=0x%08x appid=%s)", req, req->appid);
    return req;
}

void
CicoSCResourceManager::delResourceRequest(resource_request_t *req)
{
    if (NULL == req) return;

    if (NULL != req->appid)     free(req->appid);
    if (NULL != req->dispzone)  free(req->dispzone);
    if (NULL != req->winname)   free(req->winname);
    if (NULL != req->animation) free(req->animation);
    if (NULL != req->soundzone) free(req->soundzone);
    if (NULL != req->soundname) free(req->soundname);
    if (NULL != req->device)    free(req->device);
 
    free(req);
}

void
CicoSCResourceManager::enforceSound(unsigned short state,
                                    resource_request_t *req,
                                    void *user_data)
{
    ICO_TRA("CicoSCResourceManager::enforceSound Enter");
    /* NOP */
    ICO_TRA("CicoSCResourceManager::enforceSound Leave");
}
            
// receive changed state
void
CicoSCResourceManager::receiveChangedState(int state)
{
    ICO_TRA("CicoSCResourceManager::receiveChangedState Enter"
            "(state=%d)", state);

    if (STID_DRVREGULATION_ON == state) { 
        if (true == isMatchDisplayed()) {
            updateDisplayResourceRegulation(state);
            updateSoundResourceRegulation(state);
            updateInputResourceRegulation(state);
        }
    }
    else {
        updateDisplayResourceRegulation(state);
        updateSoundResourceRegulation(state);
        updateInputResourceRegulation(state);
    }

    ICO_TRA("CicoSCResourceManager::receiveChangedState Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  get policy manager instance
 *
 *  @return resource manager instace
 */
//--------------------------------------------------------------------------
CicoSCPolicyManager *
CicoSCResourceManager::getPolicyManager(void)
{
    return m_policyMgr;
}

void
CicoSCResourceManager::updateDisplayResource(resource_request_t *req,
                                             int chgzoneid)
{
    ICO_TRA("CicoSCResourceManager::updateDisplayResource Enter"
            "(req=0x%08x)", req);

    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.find(req->dispzoneid);
    if ((m_curDispResOwerReq.end() != itr) && (NULL != itr->second)) {
        // already ower
        if (itr->second == req) {
            ICO_DBG("already ower appid=%s pid=%d surfaceid=0x%08X",
                    req->appid, req->pid, req->surfaceid);
            // show request window
            m_winCtrl->show(req->surfaceid, req->animation, req->animationTime);
            m_winCtrl->activeCB(NULL, NULL, req->surfaceid, -1);
            ICO_TRA("CicoSCResourceManager::updateDisplayResource Leave");
            return;
        }
        resource_request_t *popreq = popCurDispResOwerReq(itr->second);
        if (NULL != popreq) {
            if (NULL != m_winCtrl) {
                // hide current window
                m_winCtrl->hide(popreq->surfaceid, NULL, 0);
            }

            // state change to waiting
            popreq->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting display resource request"
                    "(req=0x%08x zone=%02d:%s appid=%s",
                    popreq, popreq->dispzoneid,
                    popreq->dispzone, popreq->appid);
            m_waitingDispResReq.push_front(popreq);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
        }
    }

    std::map<unsigned int, resource_request_t*>::iterator itr2;
    itr2 = m_curDispResOwerReq.begin();
    for (; itr2 != m_curDispResOwerReq.end(); ++itr2) {
        resource_request_t *tmpreq = itr2->second;
        
        if (true == m_policyMgr->getDispZoneState(itr2->first)) {
            continue;
        }

        if (NULL == itr2->second) {
            continue;
        }

        ICO_DBG("Dequeue current display resource ower request"
                "(req=0x%08x zoneid=%02d:%s appid=%s)",
                tmpreq, tmpreq->dispzoneid, tmpreq->dispzone, tmpreq->appid);
        ICO_PRF("CHG_GUI_RES display deprived zone=%02d:%s appid=%s",
                tmpreq->dispzoneid, tmpreq->dispzone, tmpreq->appid);
        itr2->second = NULL;

        // hide current window
        m_winCtrl->hide(tmpreq->surfaceid, NULL, 0);

        ICO_DBG("Enqueue waiting display resource request"
                "(req=0x%08x zone=%02d:%s appid=%s",
                tmpreq, tmpreq->dispzoneid, tmpreq->dispzone, tmpreq->appid);
        m_waitingDispResReq.push_front(tmpreq);
    }

    if (NULL != m_winCtrl) {
        int ret = ICO_SYC_EOK;
        if (-1 != chgzoneid) {
            // move request window
            ret = m_winCtrl->setGeometry(req->surfaceid, req->dispzone,
                                         req->layerid,
                                         req->animation, req->animationTime,
                                         req->animation, req->animationTime);
            if (ICO_SYC_EOK != ret) {
                return;
            }
        }

        // show request window
        m_winCtrl->show(req->surfaceid, req->animation, req->animationTime);
    }
    // state change to acquired
    req->state = RES_STATE_ACQUIRED;
    // update current zone request
    ICO_DBG("Enqueue current display resource ower request"
            "(req=0x%08x zoneid=%02d:%s appid=%s)",
            req, req->dispzoneid, req->dispzone, req->appid);
    ICO_PRF("CHG_GUI_RES display acquired zone=%02d:%s appid=%s",
            req->dispzoneid, req->dispzone, req->appid);
    m_curDispResOwerReq[req->dispzoneid] = req;
#if 1   //DEBUG
    dumpCurDispResOwerReq();
#endif  //DEBUG

    // if current zone ower not exist after zone change
    if (-1 != chgzoneid) {
        list<resource_request_t*>::iterator itr;
        itr = m_waitingDispResReq.begin();
        for (; itr !=  m_waitingDispResReq.end(); ++itr) {
            if (m_curDispResOwerReq[(*itr)->dispzoneid] != NULL) {
                continue;
            }

            if (true == m_policyMgr->isExistDisplayZoneOwer((*itr)->dispzoneid)) {
                continue;
            }

            int type = (*itr)->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr)->dispzoneid,
                                                         (*itr)->prio);
            if (true == active) {
                resource_request_t* req = *itr;
                ICO_DBG("Dequeue waiting display resource request"
                        "(req=0x%08x zone=%02d:%s appid=%s)",
                        *itr, (*itr)->dispzoneid,
                        (*itr)->dispzone, (*itr)->appid);
                m_waitingDispResReq.erase(itr);
#if 1   //DEBUG
                dumpWaitingDispResReq();
#endif  //DEBUG
                updateDisplayResource(req);
#if 0
                m_winCtrl->active(req->surfaceid, -1);
#endif
                break;
            }
        }
    }

    ICO_TRA("CicoSCResourceManager::updateDisplayResource Leave");
}

void
CicoSCResourceManager::updateSoundResource(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::updateSoundResource Enter"
            "(req=0x%08x)", req);
    for (int i = 0; i < (int)m_curSoundResReq.size(); ++i) {
        ICO_DBG("zoneid=%d active=%d current=0x%08x",
                i, m_policyMgr->getSoundZoneState(i), m_curSoundResReq[i]);

        if (NULL == m_curSoundResReq[i]) {
            continue;
        }

        if ((i == req->soundzoneid) ||
            (false == m_policyMgr->getSoundZoneState(i))) {

            // state change to waiting
            m_curSoundResReq[i]->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting sound request queue "
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    m_curSoundResReq[i], i,
                    m_curSoundResReq[i]->soundzone, m_curSoundResReq[i]->appid);
            m_soundReqQueue[i].push_front(m_curSoundResReq[i]);

            // initialize current zone request
            ICO_DBG("Dequeue current sound request queue "
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    m_curSoundResReq[i], i,
                    m_curSoundResReq[i]->soundzone, m_curSoundResReq[i]->appid);
            ICO_PRF("CHG_GUI_RES sound   deprived zone=%02d:%s appid=%s",
                    i, m_curSoundResReq[i]->soundzone,
                    m_curSoundResReq[i]->appid);
            m_curSoundResReq[i] = NULL;
        }
    }

    if (req != m_curSoundResReq[req->soundzoneid]) {
        ico_syc_mrp_acquire_sound_resource(req);
        ico_syc_mrp_active_app(req->appid);

        // state change to acquired
        req->state = RES_STATE_ACQUIRED;
        // update current zone request
        ICO_DBG("Enqueue current sound request queue "
                "(req=0x%08x zone:%02d:%s appid=%s)",
                req, req->soundzoneid, req->soundzone, req->appid);
        ICO_PRF("CHG_GUI_RES sound   acquired zone=%02d:%s appid=%s",
                req->soundzoneid, req->soundzone, req->appid);
        m_curSoundResReq[req->soundzoneid] = req;
    }
    ICO_TRA("CicoSCResourceManager::updateSoundResource Leave");
}

void
CicoSCResourceManager::updateInputResource(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::updateInputResource Enter"
            "(req=0x%08x)", req);
    for (int i = 0; i < (int)m_curInputResReq.size(); ++i) {
        ICO_DBG("input=%d active=%d current=0x%08x",
                i, m_policyMgr->getInputState(i), m_curInputResReq[i]);

        if (NULL == m_curInputResReq[i]) {
            continue;
        }

        if ((i == req->input) ||
            (false == m_policyMgr->getInputState(i))) {

            if (NULL != m_inputCtrl) {
                // add input control
                m_inputCtrl->delInputApp(req->appid, req->device, req->input);
            }

            // state change to waiting
            m_curInputResReq[i]->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting input request queue "
                    "(req=0x%08x input:%d appid=%s)",
                    m_curInputResReq[i], i, m_curInputResReq[i]->appid);
            m_inputReqQueue[i].push_front(m_curInputResReq[i]);
            // initialize current zone request
            ICO_DBG("Dequeue current input request queue "
                    "(req=0x%08x input:%d appid=%s)",
                    m_curInputResReq[i], 
                    m_curInputResReq[i]->input, m_curInputResReq[i]->appid);
            ICO_PRF("CHG_GUI_RES input   deprived input=%d appid=%s",
                    m_curInputResReq[i]->input,
                    m_curInputResReq[i]->appid);
            m_curInputResReq[i] = NULL;
        }
    }

    if (req != m_curInputResReq[req->input]) {
        // delete input control
        m_inputCtrl->addInputApp(req->appid, req->device, req->input, 0, 0);
        // state change to acquired
        req->state = RES_STATE_ACQUIRED;
        // update current zone request
        ICO_DBG("Enqueue current input request queue "
                "(req=0x%08x input:%d appid=%s)", req, req->input, req->appid);
        ICO_PRF("CHG_GUI_RES input   acquired input=%d appid=%s",
                req->input, req->appid);
        m_curInputResReq[req->input] = req;
    }
    ICO_TRA("CicoSCResourceManager::updateInputResource Leave");
}

void
CicoSCResourceManager::updateDisplayResourceRegulation(int state)
{
    ICO_TRA("CicoSCResourceManager::updateDisplayResourceRegulation Enter"
            "(state=%d)", state);

    if (STID_DRVREGULATION_ON == state) {
        bool curchg = false;
        std::map<unsigned int, resource_request_t*>::iterator itr;
        itr = m_curDispResOwerReq.begin();
        for (; itr != m_curDispResOwerReq.end(); ++itr) {
            resource_request_t *current = itr->second;
            if (NULL == current) {
                continue;
            }

            int type = current->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         current->dispzoneid,
                                                         current->prio);
            if (false == active) {
                if (NULL != m_winCtrl) {
                    // hide current window
                    m_winCtrl->hide(current->surfaceid,
                                    m_animaName.c_str(), m_animaTime);
                }
                curchg = true;
            }
        }

        if (false == curchg) {
            ICO_TRA("CicoSCResourceManager::updateDisplayResourceRegulation "
                    "Leave");
            return;
        }

        list<resource_request_t*>::iterator itr2;
        itr2 = m_waitingDispResReq.begin();
        for (; itr2 !=  m_waitingDispResReq.end(); ++itr2) {
            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr2)->dispzoneid,
                                                         (*itr2)->prio);
            if (true == active) {
                if (NULL != m_winCtrl) {
                    // show current window
                    m_winCtrl->show((*itr2)->surfaceid,
                                    m_animaName.c_str(), m_animaTime);
                }
                break;
            }
        }
    }
    else if (STID_DRVREGULATION_OFF == state) {
        std::map<unsigned int, resource_request_t*>::iterator itr;
        itr = m_curDispResOwerReq.begin();
        for (; itr != m_curDispResOwerReq.end(); ++itr) {
            resource_request_t *current = itr->second;
            if (NULL == current) {
                if (true == m_policyMgr->isExistDisplayZoneOwer(itr->first)) {
                    if (NULL != m_winCtrl) {
                        int surfaceid = m_winCtrl->getDisplayedWindow(itr->first);

#if 0
                        if ((surfaceid > 0) && (NULL != itr->second) &&
                            (itr->second->surfaceid != surfaceid)) {
                            m_winCtrl->hide(surfaceid,
                                            m_animaName.c_str(),
                                            m_animaTime);
                        }
#else
                        if (surfaceid > 0) {
                            m_winCtrl->hide(surfaceid,
                                            m_animaName.c_str(),
                                            m_animaTime);
                        }

#endif
                    }
                    continue;
                }
                list<resource_request_t*>::iterator itr2;
                itr2 = m_waitingDispResReq.begin();
                for (; itr2 != m_waitingDispResReq.end(); ++itr2) {
                    resource_request_t *req = *itr2;
                    if (itr->first != (unsigned int)req->dispzoneid) {
                        continue;
                    }
                    int type = req->resid & RESID_TYPE_MASK;
                    bool active = false;
                    active = m_policyMgr->acquireDisplayResource(type,
                                                                 req->dispzoneid,
                                                                 req->prio);
                    if (true == active) {
                        ICO_DBG("Dequeue waiting display resource request"
                                "(req=0x%08x zone=%02d:%s appid=%s)",
                                req, req->dispzoneid,
                                req->dispzone, req->appid);
                        m_waitingDispResReq.erase(itr2);
                        ICO_DBG("Enqueue current display resource request"
                                "(req=0x%08x zone=%02d:%s appid=%s)",
                                req, req->dispzoneid, 
                                req->dispzone, req->appid);
                        ICO_PRF("CHG_GUI_RES display acquired zone=%02d:%s "
                                "appid=%s",
                                req->dispzoneid, req->dispzone, req->appid);
                        m_curDispResOwerReq[req->dispzoneid] = req;
#if 1   //DEBUG
                        dumpCurDispResOwerReq();
                        dumpWaitingDispResReq();
#endif  //DEBUG
                        m_winCtrl->show(req->surfaceid,
                                        m_animaName.c_str(), m_animaTime);
                        m_winCtrl->active(req->surfaceid, -1);
                        break;
                    }
                }
            }
            else {
                int type = current->resid & RESID_TYPE_MASK;
                bool active = false;
                active = m_policyMgr->acquireDisplayResource(type,
                                                             current->dispzoneid,
                                                             current->prio);
                if (true == active) {
                    if (NULL != m_winCtrl) {
                        int surfaceid = m_winCtrl->getDisplayedWindow(
                                                    itr->second->dispzoneid);
                        if ((itr->second->surfaceid != surfaceid) &&
                            (surfaceid > 0)) {
                            m_winCtrl->hide(surfaceid,
                                            m_animaName.c_str(), m_animaTime);
                        }
 
                        // show current window
                        m_winCtrl->show(current->surfaceid,
                                        m_animaName.c_str(), m_animaTime);
                    }
                }
#if 1   //DEBUG
                        dumpCurDispResOwerReq();
                        dumpWaitingDispResReq();
#endif  //DEBUG
            }
        }
    }

    ICO_TRA("CicoSCResourceManager::updateDisplayResourceRegulation Leave");
}

void
CicoSCResourceManager::updateSoundResourceRegulation(int state)
{
    ICO_TRA("CicoSCResourceManager::updateSoundResourceRegulation Enter"
            "(state=%d)", state);

    bool curchg = false;
    for (int i = 0; i < (int)m_curSoundResReq.size(); ++i) {
        resource_request_t *current = m_curSoundResReq[i];
        if (NULL == current) {
            continue;
        }

        int type = current->resid & RESID_TYPE_MASK;
        bool active = false;
        active = m_policyMgr->acquireSoundResource(type,
                                                   current->soundzoneid,
                                                   current->prio);
        if (false == active) {
            // state change to waiting
            current->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting sound request queue "
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    current,  i, current->soundzone, current->appid);
            m_soundReqQueue[i].push_front(current);
            // initialize current zone request
            m_curSoundResReq[i] = NULL;
            curchg = true;
         }
    }

    if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
        ICO_TRA("CicoSCResourceManager::updateSoundResourceRegulation Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_soundReqQueue.begin();
    for (; itr != m_soundReqQueue.end(); ++itr) {
        if (NULL != m_curSoundResReq[itr->first]) {
            continue;
        }

        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireSoundResource(type,
                                                       (*itr2)->soundzoneid,
                                                       (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting sound request queue "
                        "(req=0x%08x zone:%02d:%s appid=%s)",
                        *itr2, itr->first, (*itr2)->soundzone, (*itr2)->appid);
                itr->second.erase(itr2);
                updateSoundResource(req);
                break;
            }
        }
    }

    ICO_TRA("CicoSCResourceManager::updateSoundResourceRegulation Leave");
}

void
CicoSCResourceManager::updateInputResourceRegulation(int state)
{
    ICO_TRA("CicoSCResourceManager::updateInputResourceRegulation Enter"
            "(state=%d)", state);

    bool curchg = false;
    for (int i = 0; i < (int)m_curInputResReq.size(); ++i) {
        resource_request_t *current = m_curInputResReq[i];
        if (NULL == current) {
            continue;
        }

        bool active = false;
        active = m_policyMgr->acquireInputResource(current->input,
                                                   current->prio);
        if (false == active) {
            if (NULL != m_inputCtrl) {
                // hide current window
                m_inputCtrl->delInputApp(current->appid,
                                         current->device,
                                         current->input);
            }
            // state change to waiting
            current->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting input request queue "
                    "(req=0x%08x input:%d appid=%s)",
                    current, i, current->appid);
            m_inputReqQueue[i].push_front(current);
            // initialize current zone request
            m_curInputResReq[i] = NULL;
            curchg = true;
         }
    }

    if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
        ICO_TRA("CicoSCResourceManager::updateInputResourceRegulation Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_inputReqQueue.begin();
    for (; itr != m_inputReqQueue.end(); ++itr) {
        if (NULL != m_curInputResReq[itr->first]) {
            continue;
        }

        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            bool active = false;
            active = m_policyMgr->acquireInputResource((*itr2)->input,
                                                       (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting input request queue "
                        "(req=0x%08x input:%d appid=%s)",
                        *itr2, (*itr2)->input, (*itr2)->appid);
                itr->second.erase(itr2);
                updateInputResource(req);
                break;
            }
        }
    }

    ICO_TRA("CicoSCResourceManager::updateInputResourceRegulation Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief   find currnet dipalay resource ower request
 *
 *  @param [in] request  compare requset
 *  
 *  @return request object on same requeset found, NULL on not found
 */
//--------------------------------------------------------------------------
resource_request_t *
CicoSCResourceManager::findCurDispResOwerReq(resource_request_t *req)
{
    if (NULL == req) {
        ICO_WRN("invalid argument. req is null");
        return NULL;
    }
    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.begin();
    for (; itr != m_curDispResOwerReq.end(); ++itr) {
        CompDisplayResourceRequest comp(req);
        if (true == comp(itr->second)) {
            return itr->second;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  pop currnet dipalay resource ower request from list
 *
 *  @param [in] request  compare requset
 *  
 *  @return request object on same requeset found, NULL on not found
 */
//--------------------------------------------------------------------------
resource_request_t *
CicoSCResourceManager::popCurDispResOwerReq(resource_request_t *req)
{
    if (NULL == req) {
        ICO_WRN("invalid argument. req is null");
        return NULL;
    }
    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.begin();
    for (; itr != m_curDispResOwerReq.end(); ++itr) {
        CompDisplayResourceRequest comp(req);
        if (true == comp(itr->second)) {
            ICO_DBG("Dequeue current display resource ower request"
                    "(req=0x%08x zoneid=%02d:%s appid=%s)",
                    itr->second, itr->first,
                    itr->second->dispzone, itr->second->appid);
            ICO_PRF("CHG_GUI_RES display deprived zone=%02d:%s appid=%s",
                    itr->first, itr->second->dispzone, itr->second->appid);
            resource_request_t *findreq = itr->second;
            itr->second = NULL;
#if 1   //DEBUG
            dumpCurDispResOwerReq();
#endif  //DEBUG
            return findreq;
        }
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief   dump currnet dipalay resource ower request
 */
//--------------------------------------------------------------------------
void
CicoSCResourceManager::dumpCurDispResOwerReq(void)
{
    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.begin();
    for (; itr != m_curDispResOwerReq.end(); ++itr) {
        if (NULL == itr->second) {
            ICO_DBG("current req=0x%08x zone=%02d:(null) appid=(null)",
                    itr->second, itr->first);
        }
        else {
            ICO_DBG("current req=0x%08x zone=%02d:%s appid=%s",
                    itr->second, itr->first, itr->second->dispzone,
                    itr->second->appid);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  find waiting dipalay resource request
 *
 *  @param [in] request  compare requset
 *  
 *  @return request object on same requeset found, NULL on not found
 */
//--------------------------------------------------------------------------
resource_request_t *
CicoSCResourceManager::findWaitingDispResReq(resource_request_t *req)
{
    if (NULL == req) {
        ICO_WRN("invalid argument. req is null");
        return NULL;
    }
    CompDisplayResourceRequest comp(req);
    std::list<resource_request_t*>::iterator itr;
    itr = find_if(m_waitingDispResReq.begin(), m_waitingDispResReq.end(), comp);
    if (m_waitingDispResReq.end() != itr) {
        return *itr;
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  pop waiting dipalay resource request from waiting list
 *
 *  @param [in] request  compare requset
 *  
 *  @return request object on same requeset found, NULL on not found
 */
//--------------------------------------------------------------------------
resource_request_t *
CicoSCResourceManager::popWaitingDispResReq(resource_request_t *req)
{
    if (NULL == req) {
        ICO_WRN("invalid argument. req is null");
        return NULL;
    }
    CompDisplayResourceRequest comp(req);
    std::list<resource_request_t*>::iterator itr;
    itr = find_if(m_waitingDispResReq.begin(), m_waitingDispResReq.end(), comp);
    if (m_waitingDispResReq.end() != itr) {
        ICO_DBG("Dequeue waiting display resource request"
                "(req=0x%08x zone:%02d:%s appid=%s)",
                *itr, (*itr)->dispzoneid, (*itr)->dispzone, (*itr)->appid);
        resource_request_t *findreq = *itr;
        m_waitingDispResReq.erase(itr);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
        return findreq;
    }
    return NULL;
}

//--------------------------------------------------------------------------
/**
 *  @brief  dump waiting dipalay resource request
 */
//--------------------------------------------------------------------------
void
CicoSCResourceManager::dumpWaitingDispResReq(void)
{
    std::list<resource_request_t*>::iterator itr;
    itr = m_waitingDispResReq.begin();
    for (; itr != m_waitingDispResReq.end(); ++itr) {
        if (NULL == *itr) {
            ICO_DBG("waiting req=null");
        }
        else {
            ICO_DBG("waiting req=0x%08x zone=%02d:%s appid=%s",
                    *itr, (*itr)->dispzoneid, (*itr)->dispzone, (*itr)->appid);
        }
    }
}

//--------------------------------------------------------------------------
/**
 *  @brief  update display request for regulation pre-process
 */
//--------------------------------------------------------------------------
void
CicoSCResourceManager::updateDispResRegulationPreProc(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Enter");
#if 1   //DEBUG
    dumpCurDispResOwerReq();
    dumpWaitingDispResReq();
#endif  //DEBUG

    if (NULL == req) {
        ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Leave");
        return;
    }

    if (false == m_policyMgr->getRegulation()) {
        ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Leave");
        return;
    }

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(req->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Leave");
        return;
    }

    if ((appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_ALMIGHTY) ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM)   ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM_VISIBLE)) {

        if (NULL != m_winCtrl) {
            m_winCtrl->show(req->surfaceid,
                            req->animation,
                            req->animationTime);
        }
        delResourceRequest(req);

        ICO_DBG("kind of system application");
        ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Leave");
        return;
    }

    int surfaceid = req->surfaceid;
    if (NULL != m_winCtrl) {
        surfaceid = m_winCtrl->getDisplayedWindow(req->dispzoneid);
        if (-1 == surfaceid) {
            ICO_WRN("displayed surface id is invalid.");
            surfaceid = req->surfaceid;
        }
    }

    if (req->surfaceid != surfaceid) {
        ICO_WRN("req->surfaceid(0x%08X) != displayedsurfaceid(0x%08X)",
                req->surfaceid, surfaceid);
    }

    resource_request_t *curreq = NULL;
    int min = ICO_DISPLAY0_ZONEID_MIN;
    int max = ICO_DISPLAY0_ZONEID_MAX;
    if ((req->dispzoneid >= min) && (req->dispzoneid <= max)) {
        for (int i = min; i <= max; ++i) {
            std::map<unsigned int, resource_request_t*>::iterator itr;
            itr = m_curDispResOwerReq.find(i);
            if (itr != m_curDispResOwerReq.end()) {
                if (NULL != itr->second) {
                    curreq = itr->second;
                    break;
                }
            }
        }
    }
    
    min = ICO_DISPLAY1_ZONEID_MIN;
    max = ICO_DISPLAY1_ZONEID_MAX;
    if ((NULL == curreq) && 
        (req->dispzoneid >= min) && (req->dispzoneid <= max)) {
        for (int i = min; i <= max; ++i) {
            std::map<unsigned int, resource_request_t*>::iterator itr;
            itr = m_curDispResOwerReq.find(i);
            if (itr != m_curDispResOwerReq.end()) {
                if (NULL != itr->second) {
                    curreq = itr->second;
                    break;
                }
            }
        }
    }

#if 0
    resource_request_t *curreq = NULL;
    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.find(req->dispzoneid);
    if (itr != m_curDispResOwerReq.end()) {
        curreq = itr->second;
    }
#endif

    if (NULL != curreq) {
        ICO_DBG("Dequeue current display resource request"
                "(req=0x%08x zone:%02d:%s appid=%s)",
                curreq, curreq->dispzoneid, curreq->dispzone, curreq->appid);
        ICO_PRF("CHG_GUI_RES display deprived zone=%02d:%s appid=%s",
                curreq->dispzoneid, curreq->dispzone, curreq->appid);
        m_curDispResOwerReq[curreq->dispzoneid] = NULL;

        if (curreq->surfaceid != req->surfaceid) {
            resource_request_t *waitreq = popWaitingDispResReq(req);
            ICO_DBG("Enqueue waiting display resource request"
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    curreq, curreq->dispzoneid,
                    curreq->dispzone, curreq->appid);
            m_waitingDispResReq.push_front(curreq);

            if (NULL != waitreq) {
                ICO_DBG("Enqueue current display resource request"
                        "(req=0x%08x zone:%02d:%s appid=%s)",
                        waitreq, waitreq->dispzoneid,
                        waitreq->dispzone, waitreq->appid);
                ICO_PRF("CHG_GUI_RES display acquired zone=%02d:%s appid=%s",
                        waitreq->dispzoneid, waitreq->dispzone, waitreq->appid);
                m_curDispResOwerReq[waitreq->dispzoneid] = waitreq;
            }
        }
        else {
            ICO_DBG("Enqueue current display resource request"
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    curreq, curreq->dispzoneid,
                    curreq->dispzone, curreq->appid);
            ICO_PRF("CHG_GUI_RES display acquired zone=%02d:%s appid=%s",
                    curreq->dispzoneid, curreq->dispzone, curreq->appid);
            m_curDispResOwerReq[curreq->dispzoneid] = curreq;
        }
    }
    delResourceRequest(req);

#if 1   //DEBUG
    dumpCurDispResOwerReq();
    dumpWaitingDispResReq();
#endif  //DEBUG
    ICO_TRA("CicoSCResourceManager::updateDispResRegulationPreProc Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  update sound request for regulation pre-process
 */
//--------------------------------------------------------------------------
void
CicoSCResourceManager::updateSoundResRegulationPreProc(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::updateSoundResRegulationPreProc Enter");

    if (NULL == req) {
        return;
    }

    if (false == m_policyMgr->getRegulation()) {
        return;
    }

    CicoSystemConfig *sysConf = CicoSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(req->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_TRA("CicoSCResourceManager::updateSoundResRegulationPreProc Leave");
        return;
    }

    if ((appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_ALMIGHTY) ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM)   ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM_AUDIO)) {

        delResourceRequest(req);

        ICO_DBG("kind of system application");
        ICO_TRA("CicoSCResourceManager::updateSoundResRegulationPreProc Leave");
        return;
    }

    resource_request_t *curreq = NULL;
    std::map<int, resource_request_t*>::iterator itr;
    itr = m_curSoundResReq.find(req->soundzoneid);
    if (itr != m_curSoundResReq.end()) {
        curreq = itr->second;
    }

    if (NULL != curreq) {
        ICO_DBG("Dequeue current sound resource request"
                "(req=0x%08x zone:%02d:%s appid=%s)",
                curreq, curreq->soundzoneid, curreq->soundzone, curreq->appid);
        ICO_PRF("CHG_GUI_RES sound   deprived zone=%02d:%s appid=%s",
                curreq->soundzoneid, curreq->soundzone, curreq->appid);
        m_curSoundResReq[curreq->soundzoneid] = NULL;

        if (0 != strcmp(curreq->appid, req->appid)) {
            resource_request_t *waitreq = popSoundResReq(req);
            ICO_DBG("Enqueue waiting sound resource request"
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    curreq, curreq->soundzoneid,
                    curreq->soundzone, curreq->appid);
            m_soundReqQueue[curreq->soundzoneid].push_front(curreq);

            if (NULL != waitreq) {
                ICO_DBG("Enqueue current sound resource request"
                        "(req=0x%08x zone:%02d:%s appid=%s)",
                        waitreq, waitreq->soundzoneid,
                        waitreq->soundzone, waitreq->appid);
                ICO_PRF("CHG_GUI_RES sound   acquired zone=%02d:%s appid=%s",
                        waitreq->soundzoneid, waitreq->soundzoneid,
                        waitreq->appid);
                m_curSoundResReq[curreq->soundzoneid] = waitreq;
            }
        }
        else {
            ICO_DBG("Enqueue current sound resource request"
                    "(req=0x%08x zone:%02d:%s appid=%s)",
                    curreq, curreq->soundzoneid,
                    curreq->soundzone, curreq->appid);
            ICO_PRF("CHG_GUI_RES sound   acquired zone=%d:%s appid=%s",
                    curreq->soundzoneid, curreq->soundzone, curreq->appid);
            m_curSoundResReq[curreq->soundzoneid] = curreq;
        }
    }
    delResourceRequest(req);
    ICO_TRA("CicoSCResourceManager::updateSoundResRegulationPreProc Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  update input request for regulation pre-process
 */
//--------------------------------------------------------------------------
void
CicoSCResourceManager::updateInputResRegulationPreProc(resource_request_t *req)
{
    ICO_TRA("CicoSCResourceManager::updateInputResRegulationPreProc Enter");

    if (NULL == req) {
        return;
    }

    if (false == m_policyMgr->getRegulation()) {
        return;
    }

    resource_request_t *curreq = NULL;
    std::map<int, resource_request_t*>::iterator itr;
    itr = m_curInputResReq.find(req->input);
    if (itr != m_curInputResReq.end()) {
        curreq = itr->second;
    }

    if (NULL != curreq) {
        ICO_DBG("Dequeue current input resource request"
                "(req=0x%08x input:%d appid=%s)",
                curreq, curreq->input, curreq->appid);
        ICO_PRF("CHG_GUI_RES input   deprived input=%d appid=%s",
                curreq->input, curreq->appid);
        m_curInputResReq[curreq->input] = NULL;

        if (0 != strcmp(curreq->appid, req->appid)) {
            resource_request_t *waitreq = popInputResReq(req);
            ICO_DBG("Enqueue waiting input resource request"
                    "(req=0x%08x input:%d appid=%s)",
                    curreq, curreq->input, curreq->appid);
            m_inputReqQueue[curreq->input].push_front(curreq);

            if (NULL != waitreq) {
                ICO_DBG("Enqueue current input resource request"
                        "(req=0x%08x input:%d appid=%s)",
                        waitreq, waitreq->input, waitreq->appid);
                ICO_PRF("CHG_GUI_RES input   acquired input=%d appid=%s",
                        waitreq->input, waitreq->appid);
                m_curInputResReq[curreq->input] = waitreq;
            }
        }
        else {
            ICO_DBG("Enqueue current input resource request"
                    "(req=0x%08x input:%d appid=%s)",
                    curreq, curreq->input, curreq->appid);
            ICO_PRF("CHG_GUI_RES input   acquired input=%d appid=%s",
                    curreq->input, curreq->appid);
            m_curInputResReq[curreq->input] = curreq;
        }
    }
    delResourceRequest(req);
    ICO_TRA("CicoSCResourceManager::updateInputResRegulationPreProc Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief  compare displayed surface and ower surface
 */
//--------------------------------------------------------------------------
bool
CicoSCResourceManager::isMatchDisplayed(void)
{
    bool ret = false;
    std::map<unsigned int, resource_request_t*>::iterator itr;
    itr = m_curDispResOwerReq.begin();
    for (; itr != m_curDispResOwerReq.end(); ++itr) {
        if (NULL == itr->second) {
            continue;
        }
        if (NULL != m_winCtrl) {
            int surfaceid = m_winCtrl->getDisplayedWindow(
                                        itr->second->dispzoneid);
            std::map<unsigned int, resource_request_t*>::iterator itr2;
            itr2 = m_curDispResOwerReq.begin();
            for (; itr2 != m_curDispResOwerReq.end(); ++itr2) {
                if (NULL == itr2->second) {
                    continue;
                }
                if ((itr2->second->surfaceid == surfaceid)) {
                    ret = true;
                    break;
                }
            }
        }
    }
    return ret;
}
// vim:set expandtab ts=4 sw=4:
