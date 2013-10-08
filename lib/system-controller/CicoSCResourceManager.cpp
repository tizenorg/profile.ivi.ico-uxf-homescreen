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
#include "CicoSCSystemConfig.h"
#include "CicoSCConf.h"
#include "CicoSCCommand.h"
#include "CicoSCPolicyManager.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCAilItems.h"
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
    : m_winCtrl(NULL), m_inputCtrl(NULL)
{
    m_policyMgr = new CicoSCPolicyManager(this);

    CicoSCSystemConfig *sysConf = CicoSCSystemConfig::getInstance();
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
    ICO_DBG("CicoSCResourceManager::initialize Enter");

    int ret = ICO_SYC_EOK;

    ret = ico_syc_mrp_init(enforceSound, this);
    if (ICO_SYC_EOK != ret) {
        return ret;
    }

    ret = m_policyMgr->initialize();
    if (ICO_SYC_EOK != ret) {
        return ret;
    }


    ICO_DBG("CicoSCResourceManager::initialize Leave");
    return ret;
}
  
void
CicoSCResourceManager::terminate(void)
{
    ICO_DBG("CicoSCResourceManager::terminate Enter");
    m_policyMgr->terminate();
    ICO_DBG("CicoSCResourceManager::terminate Leave");
}

void
CicoSCResourceManager::handleCommand(const CicoSCCommand &cmd,
                                     bool internal)
{
    ICO_DBG("CicoSCResourceManager::handleCommand Enter"
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
                acquireDisplayResource(req);
            }

        }
        if (true == opt->soundres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_SOUND,
                                                         reqtype,
                                                         cmd);
            acquireSoundResource(req);
        }
        if (true == opt->inputres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_INPUT,
                                                         reqtype,
                                                         cmd);
            acquireInputResource(req);
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

    ICO_DBG("CicoSCResourceManager::handleCommand Leave");
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
    ICO_DBG("CicoSCResourceManager::popDispResReq Enter");

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
            ICO_DBG("CicoSCResourceManager::popDispResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }

    ICO_DBG("CicoSCResourceManager::popDispResReq Leave(NULL)");
    return NULL;
}
#endif

resource_request_t *
CicoSCResourceManager::popSoundResReq(resource_request_t *req)
{
    ICO_DBG("CicoSCResourceManager::popSoundResReq Enter");

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_soundReqQueue.begin();
    CompSoundResourceRequest comp(req);
    for (; itr != m_soundReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = find_if(itr->second.begin(), itr->second.end(), comp);
        if (itr->second.end() != itr2) {
            ICO_DBG("Dequeue waiting sound request queue zone=%d req=0x%08x",
                    itr->first, *itr2);
            m_soundReqQueue[req->soundzoneid].erase(itr2);
            ICO_DBG("CicoSCResourceManager::popSoundResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }
    ICO_DBG("CicoSCResourceManager::popSoundResReq Leave(NULL)");
    return NULL;
}

resource_request_t * 
CicoSCResourceManager::popInputResReq(resource_request_t *req)
{
    ICO_DBG("CicoSCResourceManager::popInputResReq Enter");

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_inputReqQueue.begin();
    CompInputResourceRequest comp(req);
    for (; itr != m_inputReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = find_if(itr->second.begin(), itr->second.end(), comp);
        if (itr->second.end() != itr2) {
            ICO_DBG("Dequeue waiting input request queue input=%d req=0x%08x",
                    itr->first, *itr2);
            m_inputReqQueue[req->input].erase(itr2);
            ICO_DBG("CicoSCResourceManager::popInputResReq Leave"
                    "(0x%08x)", *itr2);
            return *itr2;
        }
    }

    ICO_DBG("CicoSCResourceManager::popDispResReq Leave(NULL)");
    return NULL;
}


bool
CicoSCResourceManager::acquireDisplayResource(resource_request_t *newreq,
                                              bool control)
{
    ICO_DBG("CicoSCResourceManager::acquireDisplayResource Enter");

    CicoSCSystemConfig *sysConf = CicoSCSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(newreq->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_DBG("CicoSCResourceManager::acquireDisplayResource Leave(false)");
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
        ICO_DBG("CicoSCResourceManager::acquireDisplayResource Leave(true)");
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
        if (NULL != req->animation) free(req->animation);
        req->animation = strdup(newreq->animation);
        req->animationTime = newreq->animationTime;

        // delete new request
        delResourceRequest(newreq);
    }
    
    if (false == control) {
        ICO_DBG("Enqueue waiting display resource request"
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
        ICO_DBG("Enqueue waiting display resource request"
                "(req=0x%08x appid=%s)", req, req->appid);
        m_waitingDispResReq.push_front(req);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
    }


    ICO_DBG("CicoSCResourceManager::acquireDisplayResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseDisplayResource(resource_request_t *newreq)
{
    ICO_DBG("CicoSCResourceManager::releaseDisplayResource Enter"
            "(newreq=0x%08x)", newreq);

    // if exist in wating request list, pop request
    resource_request_t *req = popWaitingDispResReq(newreq);
    if (NULL != req) {
        delResourceRequest(req);
        delResourceRequest(newreq);
        ICO_DBG("CicoSCResourceManager::releaseDisplayResource Leave");
        return;
    }

    // if exist current ower request, pop request
    req = popCurDispResOwerReq(newreq);
    if (NULL != req) {
        delResourceRequest(req);
    }
    delResourceRequest(newreq);

    list<resource_request_t*>::iterator itr;
    itr = m_waitingDispResReq.begin();
    for (; itr != m_waitingDispResReq.end(); ++itr) {
        int type = (*itr)->resid & RESID_TYPE_MASK;
        bool active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr)->dispzoneid,
                                                         (*itr)->prio);
        if (true == active) {
            resource_request_t* popreq = popWaitingDispResReq(*itr);
            updateDisplayResource(popreq);
            m_winCtrl->active(popreq->surfaceid, -1);
            break;
        }
    }

    ICO_DBG("CicoSCResourceManager::releaseDisplayResource Leave");
}

bool
CicoSCResourceManager::acquireSoundResource(resource_request_t *newreq)
{
    ICO_DBG("CicoSCResourceManager::acquireSoundResource Enter");

    CicoSCSystemConfig *sysConf = CicoSCSystemConfig::getInstance();
    const CicoSCAppKindConf *appKindConf = NULL;
    appKindConf = sysConf->findAppKindConfbyId(newreq->appkind);
    if (NULL == appKindConf) {
        ICO_ERR("not found CicoSCAppKindConf instance");
        ICO_DBG("CicoSCResourceManager::acquireSoundResource Leave(false)");
        return false;
    }

    if ((appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_ALMIGHTY) ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM)   ||
        (appKindConf->privilege == CicoSCAppKindConf::PRIVILEGE_SYSTEM_AUDIO)) {

        delResourceRequest(newreq);

        ICO_DBG("kind of system application");
        ICO_DBG("CicoSCResourceManager::acquireSoundResource Leave(true)");
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
        ICO_DBG("CicoSCResourceManager::acquireSoundResource Leave(true)");
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
        ICO_DBG("Enqueue waiting sound request queue zone=%d req=0x%08x",
                req->soundzoneid, req);
        m_soundReqQueue[req->soundzoneid].push_front(req);
    }

    ICO_DBG("CicoSCResourceManager::acquireSoundResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseSoundResource(resource_request_t *newreq)
{
    ICO_DBG("CicoSCResourceManager::releaseSoundResource Enter");

    bool curchg = false;
    CompSoundResourceRequest comp(newreq);
    for (int i = 0; i < (int)m_curSoundResReq.size(); ++i) {
         if (NULL == m_curSoundResReq[i]) {
            continue;
         }

         if (true == comp(m_curSoundResReq[i])) {
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
        ICO_DBG("CicoSCResourceManager::releaseSoundResource Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_soundReqQueue.begin();
    for (; itr != m_soundReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = m_policyMgr->acquireSoundResource(type,
                                                            (*itr2)->soundzoneid,
                                                            (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting sound request queue "
                        "zone=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateSoundResource(req);
                break;
            }
        }
    }

    ICO_DBG("CicoSCResourceManager::releaseSoundResource Leave");
}

bool
CicoSCResourceManager::acquireInputResource(resource_request_t *newreq)
{
    ICO_DBG("CicoSCResourceManager::acquireInputResource Enter");

    resource_request_t *req = popInputResReq(newreq);
    if (NULL == req) {
        req = newreq;
    }

    // is change zone?
    CompInputResourceRequest comp(req);
    if (true == comp(m_curInputResReq[req->input])) {
        ICO_DBG("already acquired appid=%s pid=%d input=0x%08X",
                req->appid, req->pid, req->input);
        ICO_DBG("CicoSCResourceManager::acquireInputResource Leave(true)");
        // free new request
        delResourceRequest(newreq);
        return true;
    }

    bool state = m_policyMgr->acquireInputResource(req->input, req->prio);
    if (true == state) {
        updateInputResource(req);
    }
    else {
        ICO_DBG("Enqueue waiting input request queue input=%d req=0x%08x",
                req->input, req);
        m_inputReqQueue[req->input].push_front(req);
    }

    ICO_DBG("CicoSCResourceManager::acquireInputResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseInputResource(resource_request_t *newreq)
{
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
        ICO_DBG("CicoSCResourceManager::releaseInputResource Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_inputReqQueue.begin();
    for (; itr != m_inputReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            bool active = m_policyMgr->acquireInputResource((*itr2)->input,
                                                            (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting input request queue "
                        "input=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateInputResource(req);
                break;
            }
        }
    }
}

resource_request_t *
CicoSCResourceManager::newResourceRequest(int resid,
                                          int reqtype,
                                          const CicoSCCommand &cmd)
{
    ICO_DBG("CicoSCResourceManager::newResourceRequest Enter");

    resource_request_t *req = NULL;
    req = (resource_request_t*)calloc(1, sizeof(resource_request_t));
    CicoSCCmdResCtrlOpt *opt = (CicoSCCmdResCtrlOpt*)cmd.opt;
    CicoSCSystemConfig *systemConfig = CicoSCSystemConfig::getInstance();

    req->reqtype = reqtype;

    CicoSCLifeCycleController *lifeCycle =
        CicoSCLifeCycleController::getInstance();
    const CicoSCAilItems* ailItem = lifeCycle->findAIL(cmd.appid);
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

    ICO_DBG("CicoSCResourceManager::newResourceRequest Leave"
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
    ICO_DBG("CicoSCResourceManager::enforceSound Enter");
    /* NOP */
    ICO_DBG("CicoSCResourceManager::enforceSound Leave");
}
            
// receive changed state
void
CicoSCResourceManager::receiveChangedState(int state)
{
    ICO_DBG("CicoSCResourceManager::receiveChangedState Enter"
            "(state=%d)", state);

    if ((STID_DRVREGULATION_ON == state)||
        (STID_DRVREGULATION_OFF == state)) {

        updateDisplayResourceRegulation(state);
        updateSoundResourceRegulation(state);
        updateInputResourceRegulation(state);
    }

    ICO_DBG("CicoSCResourceManager::receiveChangedState Leave");
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
    ICO_DBG("CicoSCResourceManager::updateDisplayResource Enter"
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
            m_winCtrl->active(req->surfaceid, -1);
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
                    "(req=0x%08x appid=%s", popreq, popreq->appid);
            m_waitingDispResReq.push_front(popreq);
#if 1   //DEBUG
        dumpWaitingDispResReq();
#endif  //DEBUG
        }
    }
        
    if (NULL != m_winCtrl) {
        if (-1 != chgzoneid) {
            // move request window
            m_winCtrl->setGeometry(req->surfaceid, req->dispzone,
                                   req->animation, req->animationTime,
                                   req->animation, req->animationTime);
        }
        // show request window
        m_winCtrl->show(req->surfaceid, req->animation, req->animationTime);
    }
    // state change to acquired
    req->state = RES_STATE_ACQUIRED;
    // update current zone request
    ICO_DBG("Enqueue current display resource ower request"
            "(req=0x%08x zoneid=%d appid=%s)",
            req, req->dispzoneid, req->appid);
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
            int type = (*itr)->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr)->dispzoneid,
                                                         (*itr)->prio);
            if (true == active) {
                resource_request_t* req = *itr;
                ICO_DBG("Dequeue waiting display resource request"
                        "(req=0x%08x appid=%s)", *itr, (*itr)->appid);
                m_waitingDispResReq.erase(itr);
#if 1   //DEBUG
                dumpWaitingDispResReq();
#endif  //DEBUG
                updateDisplayResource(req);
                m_winCtrl->active(req->surfaceid, -1);
                break;
            }
        }
    }

    ICO_DBG("CicoSCResourceManager::updateDisplayResource Leave");
}

void
CicoSCResourceManager::updateSoundResource(resource_request_t *req)
{
    ICO_DBG("CicoSCResourceManager::updateSoundResource Enter"
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
                    "zone=%d req=0x%08x", i, m_curSoundResReq[i]);
            m_soundReqQueue[i].push_front(m_curSoundResReq[i]);
            // initialize current zone request
            m_curSoundResReq[i] = NULL;
        }
    }

    if (req != m_curSoundResReq[req->soundzoneid]) {
        ico_syc_mrp_acquire_sound_resource(req);
        ico_syc_mrp_active_app(req->appid);

        // state change to acquired
        req->state = RES_STATE_ACQUIRED;
        // update current zone request
        m_curSoundResReq[req->soundzoneid] = req;
    }
    ICO_DBG("CicoSCResourceManager::updateSoundResource Leave");
}

void
CicoSCResourceManager::updateInputResource(resource_request_t *req)
{
    ICO_DBG("CicoSCResourceManager::updateInputResource Enter"
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
                    "zone=%d req=0x%08x", i, m_curInputResReq[i]);
            m_inputReqQueue[i].push_front(m_curInputResReq[i]);
            // initialize current zone request
            m_curInputResReq[i] = NULL;
        }
    }

    if (req != m_curInputResReq[req->input]) {
        // delete input control
        m_inputCtrl->addInputApp(req->appid, req->device, req->input, 0, 0);
        // state change to acquired
        req->state = RES_STATE_ACQUIRED;
        // update current zone request
        m_curInputResReq[req->input] = req;
    }
    ICO_DBG("CicoSCResourceManager::updateInputResource Leave");
}

void
CicoSCResourceManager::updateDisplayResourceRegulation(int state)
{
    ICO_DBG("CicoSCResourceManager::updateDisplayResourceRegulation Enter"
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
                    // TODO animation?
                    m_winCtrl->hide(current->surfaceid, NULL, 0);
                }
                curchg = true;
            }
        }

        if (false == curchg) {
            ICO_DBG("CicoSCResourceManager::updateDisplayResourceRegulation "
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
                    // TODO animation?
                    m_winCtrl->show((*itr2)->surfaceid, NULL, 0);
                    m_winCtrl->active((*itr2)->surfaceid, -1);
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
                continue;
            }

            int type = current->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         current->dispzoneid,
                                                         current->prio);
            if (true == active) {
                if (NULL != m_winCtrl) {
                    // show current window
                    // TODO animation?
                    m_winCtrl->show(current->surfaceid, NULL, 0);
                    m_winCtrl->active(current->surfaceid, -1);
                }
            }
        }
    }

    ICO_DBG("CicoSCResourceManager::updateDisplayResourceRegulation Leave");
}

void
CicoSCResourceManager::updateSoundResourceRegulation(int state)
{
    ICO_DBG("CicoSCResourceManager::updateSoundResourceRegulation Enter"
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
                    "zone=%d req=0x%08x", i, current);
            m_soundReqQueue[i].push_front(current);
            // initialize current zone request
            m_curSoundResReq[i] = NULL;
            curchg = true;
         }
    }

    if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
        ICO_DBG("CicoSCResourceManager::updateSoundResourceRegulation Leave");
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
                        "zone=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateSoundResource(req);
                break;
            }
        }
    }

    ICO_DBG("CicoSCResourceManager::updateSoundResourceRegulation Leave");
}

void
CicoSCResourceManager::updateInputResourceRegulation(int state)
{
    ICO_DBG("CicoSCResourceManager::updateInputResourceRegulation Enter"
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
                    "zone=%d req=0x%08x", i, current);
            m_inputReqQueue[i].push_front(current);
            // initialize current zone request
            m_curInputResReq[i] = NULL;
            curchg = true;
         }
    }

    if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
        ICO_DBG("CicoSCResourceManager::updateInputResourceRegulation Leave");
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
                        "zone=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateInputResource(req);
                break;
            }
        }
    }

    ICO_DBG("CicoSCResourceManager::updateDisplayResourceRegulation Leave");
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
            ICO_DBG("Dequeue current display reoursce ower request"
                    "(req=0x%08x zoneid=%d appid=%s)",
                    itr->second, itr->first, itr->second->appid);
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
// vim:set expandtab ts=4 sw=4:
