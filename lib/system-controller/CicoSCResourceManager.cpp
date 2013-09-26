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
                m_curDispResReq[(*itr2)->id] = NULL;
                m_dispReqQueue[(*itr2)->id]  = l;
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

    resource_request_t *req = NULL;
    // equal current request?
    CompDisplayResourceRequest comp(newreq);
    resource_request_t *curreq = m_curDispResReq[newreq->dispzoneid];
    if ((NULL != curreq) && (true == comp(curreq))) {
        ICO_DBG("already acquired appid=%s pid=%d surfaceid=0x%08X",
                curreq->appid, curreq->pid, curreq->surfaceid);
        ICO_DBG("CicoSCResourceManager::acquireDisplayResource Leave(true)");
        // is change zone?
        if (newreq->dispzoneid == curreq->dispzoneid) {
            // free new request
            delResourceRequest(newreq);
            return true;
        }
        req = curreq;
        curreq->dispzoneid = newreq->dispzoneid;
        if (NULL != curreq->animation) free(curreq->animation);
        curreq->animation = strdup(newreq->animation);
        curreq->animationTime = newreq->animationTime;
        delResourceRequest(newreq);
    }
    else {
        // watting request?
        req = popDispResReq(newreq);
        if (NULL == req) {
            req = newreq;
        }
        else {
            req->dispzoneid = newreq->dispzoneid;
            if (NULL != req->animation) free(req->animation);
            req->animation = strdup(newreq->animation);
            req->animationTime = newreq->animationTime;
            delResourceRequest(newreq);
        }
    }

    if (false == control) {
        ICO_DBG("Enqueue waiting display request queue zone=%d req=0x%08x",
                req->dispzoneid, req);
        m_dispReqQueue[req->dispzoneid].push_front(req);
        return true;
    }

    int type = req->resid & RESID_TYPE_MASK;
    bool state = m_policyMgr->acquireDisplayResource(type,
                                                     req->dispzoneid,
                                                     req->prio);
    if (true == state) {
        updateDisplayResource(req);
    }
    else {
        ICO_DBG("Enqueue waiting display request queue zone=%d req=0x%08x",
                req->dispzoneid, req);
        m_dispReqQueue[req->dispzoneid].push_front(req);
    }


    ICO_DBG("CicoSCResourceManager::acquireDisplayResource Leave");
    return true;
}

void
CicoSCResourceManager::releaseDisplayResource(resource_request_t *newreq)
{
    ICO_DBG("CicoSCResourceManager::releaseDisplayResource Enter");

    bool curchg = false;
    CompDisplayResourceRequest comp(newreq);
    for (int i = 0; i < (int)m_curDispResReq.size(); ++i) {
         if (NULL == m_curDispResReq[i]) {
            continue;
         }

         if (true == comp(m_curDispResReq[i])) {
            if (NULL != m_winCtrl) {
                // hide current window
                m_winCtrl->hide(m_curDispResReq[i]->surfaceid, NULL, 0);
            }
            delResourceRequest(newreq);
            delResourceRequest(m_curDispResReq[i]);
            m_curDispResReq[i] = NULL;
            curchg = true;
            break;
         }
    }

    // If current request is not changed, 
    // remove the request from the waiting queue.
    if (false == curchg) {
       resource_request_t *req = popDispResReq(newreq);
        if (NULL != req) {
            delResourceRequest(req);
            req = NULL;
        }
        delResourceRequest(newreq);
        ICO_DBG("CicoSCResourceManager::releaseDisplayResource Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_dispReqQueue.begin();
    for (; itr != m_dispReqQueue.end(); ++itr) {
        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr2)->dispzoneid,
                                                         (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting display request queue "
                        "zone=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateDisplayResource(req);
                break;
            }
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

    ICO_DBG("CicoSCResourceManager::newResourceRequest Leave");
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
#if 0
        bool curchg = false;
        for (int i = 0; i < (int)m_curDispResReq.size(); ++i) {
             if (NULL == m_curDispResReq[i]) {
                continue;
             }
             int type = m_curDispResReq[i]->resid & RESID_TYPE_MASK;
             bool active = m_policyMgr->acquireDisplayResource(type,
                                                             m_curDispResReq[i]->dispzoneid,
                                                             m_curDispResReq[i]->prio);
            if (false == active) {
                if (NULL != m_winCtrl) {
                    // hide current window
                    m_winCtrl->hide(m_curDispResReq[i]->surfaceid, NULL, 0);
                }
                // state change to waiting
                m_curDispResReq[i]->state = RES_STATE_WAITING;
                // enqueue request
                ICO_DBG("Enqueue waiting display request queue "
                        "zone=%d req=0x%08x", i, m_curDispResReq[i]);
                m_dispReqQueue[i].push_front(m_curDispResReq[i]);
                // initialize current zone request
                m_curDispResReq[i] = NULL;
                curchg = true;
             }
        }

        if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
            ICO_DBG("CicoSCResourceManager::receiveChangedState Leave");
            return;
        }

        map<int, list<resource_request_t*> >::iterator itr;
        itr = m_dispReqQueue.begin();
        for (; itr != m_dispReqQueue.end(); ++itr) {
            list<resource_request_t*>::iterator itr2;
            itr2 = itr->second.begin();
            for (; itr2 !=  itr->second.end(); ++itr2) {
                if (NULL != m_curDispResReq[itr->first]) {
                    continue;
                }
                int type = (*itr2)->resid & RESID_TYPE_MASK;
                bool active = m_policyMgr->acquireDisplayResource(type,
                                                             (*itr2)->dispzoneid,
                                                             (*itr2)->prio);
                if (true == active) {
                    resource_request_t* req = *itr2;
                    ICO_DBG("Dequeue waiting display request queue "
                            "zone=%d req=0x%08x", itr->first, *itr2);
                    itr->second.erase(itr2);
                    updateDisplayResource(req);
                    break;
                }
            }
        }
#endif
    }

    ICO_DBG("CicoSCResourceManager::receiveChangedState Leave");
}

void
CicoSCResourceManager::updateDisplayResource(resource_request_t *req)
{
    ICO_DBG("CicoSCResourceManager::updateDisplayResource Enter"
            "(req=0x%08x)", req);
    for (int i = 0; i < (int)m_curDispResReq.size(); ++i) {
        ICO_DBG("zoneid=%d active=%d current=0x%08x",
                i, m_policyMgr->getDispZoneState(i), m_curDispResReq[i]);

        if (NULL == m_curDispResReq[i]) {
            continue;
        }

        if ((i == req->dispzoneid) ||
            (false == m_policyMgr->getDispZoneState(i))) {

            if (NULL != m_winCtrl) {
                // hide current window
                m_winCtrl->hide(m_curDispResReq[i]->surfaceid, NULL, 0);
            }
            // state change to waiting
            m_curDispResReq[i]->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting display request queue "
                    "zone=%d req=0x%08x", i, m_curDispResReq[i]);
            m_dispReqQueue[i].push_front(m_curDispResReq[i]);
            // initialize current zone request
            m_curDispResReq[i] = NULL;
        }
    }

    if (req != m_curDispResReq[req->dispzoneid]) {
        if (NULL != m_winCtrl) {
            // show request window
            m_winCtrl->show(req->surfaceid, req->animation,req->animationTime);
        }
        // state change to acquired
        req->state = RES_STATE_ACQUIRED;
        // update current zone request
        m_curDispResReq[req->dispzoneid] = req;
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

    bool curchg = false;
    for (int i = 0; i < (int)m_curDispResReq.size(); ++i) {
        resource_request_t *current = m_curDispResReq[i];
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
                m_winCtrl->hide(current->surfaceid, NULL, 0);
            }
            // state change to waiting
            current->state = RES_STATE_WAITING;
            // enqueue request
            ICO_DBG("Enqueue waiting display request queue "
                    "zone=%d req=0x%08x", i, current);
            m_dispReqQueue[i].push_front(current);
            // initialize current zone request
            m_curDispResReq[i] = NULL;
            curchg = true;
         }
    }

    if ((false == curchg) && (STID_DRVREGULATION_ON == state)) {
        ICO_DBG("CicoSCResourceManager::updateDisplayResourceRegulation Leave");
        return;
    }

    map<int, list<resource_request_t*> >::iterator itr;
    itr = m_dispReqQueue.begin();
    for (; itr != m_dispReqQueue.end(); ++itr) {
        if (NULL != m_curDispResReq[itr->first]) {
            continue;
        }

        list<resource_request_t*>::iterator itr2;
        itr2 = itr->second.begin();
        for (; itr2 !=  itr->second.end(); ++itr2) {
            int type = (*itr2)->resid & RESID_TYPE_MASK;
            bool active = false;
            active = m_policyMgr->acquireDisplayResource(type,
                                                         (*itr2)->dispzoneid,
                                                         (*itr2)->prio);
            if (true == active) {
                resource_request_t* req = *itr2;
                ICO_DBG("Dequeue waiting display request queue "
                        "zone=%d req=0x%08x", itr->first, *itr2);
                itr->second.erase(itr2);
                updateDisplayResource(req);
                break;
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
// vim:set expandtab ts=4 sw=4:
