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
 *  @brief 
 */
//==========================================================================
#include "CicoSCResourceManager.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "ico_syc_mrp_resource_private.h"

#include "ico_syc_msg_cmd_def.h"
#include "CicoSCSystemConfig.h"
#include "CicoSCCommandParser.h"
#include "CicoSCPolicyManager.h"
#include "CicoSCLifeCycleController.h"
#include "CicoSCAilItems.h"
#include "CicoSCWindowController.h"
#include "CicoSCInputController.h"

CicoSCResourceManager::CicoSCResourceManager()
    : m_winCtrl(NULL), m_inputCtrl(NULL)
{
    m_policyMgr = new CicoSCPolicyManager(this);
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

    ret = ico_syc_mrp_init(enforceDisplay, enforceSound, enforceInput, this);
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
            "(cmdid=%08X)", cmd.cmdid);

    CicoSCCmdResCtrlOpt *opt = (CicoSCCmdResCtrlOpt*)cmd.opt;

    // request command from application or internal
    int reqtype = internal ? REQTYPE_AUTO : REQTYPE_APP;

    if (MSG_CMD_ACQUIRE_RES == cmd.cmdid) {
        
        if (true == opt->displayres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_DISPLAY,
                                                         reqtype,
                                                         cmd);
            acquireDisplayResource(req, 0);
        }
        if (true == opt->soundres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_SOUND,
                                                         reqtype,
                                                         cmd);
            acquireSoundResource(req, 0);
        }
        if (true == opt->inputres) {
            resource_request_t *req = newResourceRequest(RESID_KIND_INPUT,
                                                         reqtype,
                                                         cmd);
            acquireInputResource(req, 0);
        }
    }
    else if (MSG_CMD_RELEASE_RES == cmd.cmdid) {

        if (true == opt->displayres) {
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

bool
CicoSCResourceManager::acquireDisplayResource(resource_request_t *req,
                                              int addprio)
{
    bool state = m_policyMgr->acquireDisplayResource(req->dispzoneid,
                                                     req->category);
    if (true == state) {
        if (NULL != m_winCtrl) {
            //m_winCtrl->show(req->surfaceid, NULL, 0);
        }
    }
    return true;
}

bool
CicoSCResourceManager::releaseDisplayResource(resource_request_t *req)
                                                 
{
    return ico_syc_mrp_release_display_resource(req);
}

bool
CicoSCResourceManager::acquireSoundResource(resource_request_t *req,
                                            int addprio)
{
    return ico_syc_mrp_acquire_sound_resource(req, addprio);
}

bool
CicoSCResourceManager::releaseSoundResource(resource_request_t *req)
{
    return ico_syc_mrp_release_sound_resource(req);
}

bool
CicoSCResourceManager::acquireInputResource(resource_request_t *req,
                                            int addprio)
{
    return ico_syc_mrp_acquire_input_resource(req, addprio);
}

bool
CicoSCResourceManager::releaseInputResource(resource_request_t *req)
{
    return ico_syc_mrp_release_input_resource(req);
}

resource_request_t *
CicoSCResourceManager::newResourceRequest(int resid,
                                          int reqtype,
                                          const CicoSCCommand &cmd)
{
    resource_request_t *req = NULL;
    req = (resource_request_t*)calloc(1, sizeof(resource_request_t));
    CicoSCCmdResCtrlOpt *opt = (CicoSCCmdResCtrlOpt*)cmd.opt;
    CicoSCSystemConfig *systemConfig = CicoSCSystemConfig::getInstance();

    req->reqtype     = reqtype;

    CicoSCLifeCycleController *lifeCycle =
        CicoSCLifeCycleController::getInstance();
    const CicoSCAilItems* ailItem = lifeCycle->findAIL(cmd.appid);
    req->category = ailItem->m_categoryID;
    ICO_DBG("req->category=%d", req->category);

    // TODO
    const CicoSCCategoryConf *categoryConf =
        systemConfig->findCategoryConfbyId(req->category);
    if (NULL == categoryConf) {
        req->prio = 10;
    }
 
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

    /* set application infomartion */
    req->appid = strdup(cmd.appid.c_str());
    req->pid   = cmd.pid;
    req->state = ICO_APF_RESOURCE_STATE_WAITTING;

    if (resid == RESID_KIND_DISPLAY) {
        req->dispzone   = strdup(opt->displayZone.c_str());
        req->dispzoneid = systemConfig->getDizplayZoneIdbyFullName(req->dispzone);
        req->winname    = strdup(opt->windowName.c_str());
        req->surfaceid  = opt->surfaceid;
        req->id         = opt->surfaceid;;
    }
    else if (resid == RESID_KIND_SOUND) {
        req->soundzone   = strdup(opt->soundZone.c_str());
        req->soundzoneid = systemConfig->getSoundZoneIdbyFullName(req->soundzone);
        req->sooudname   = strdup(opt->soundName.c_str());
        req->soundid     = opt->soundid;
        req->soundadjust = opt->adjust;
        req->id          = opt->soundid;
    }
    else if (resid == RESID_KIND_INPUT) {
        req->device = strdup(opt->device.c_str());
        req->input  = opt->input;
        req->id     = opt->input;
    }

    return req;
}

void
CicoSCResourceManager::delResourceRequest(resource_request_t *req)
{
    if (NULL == req) return;

    if (NULL != req->appid)     free(req->appid);
    if (NULL != req->dispzone)  free(req->dispzone);
    if (NULL != req->winname)   free(req->winname);
    if (NULL != req->soundzone) free(req->soundzone);
    if (NULL != req->sooudname) free(req->sooudname);
    if (NULL != req->device)    free(req->device);
 
    free(req);
}

void
CicoSCResourceManager::enforceDisplay(unsigned short state,
                                      const char *appid,
                                      unsigned int id,
                                      void *user_data)
{
    ICO_DBG("CicoSCResourceManager::enforceDisplay Enter");
//    ico_uxf_window_control(appid, id, ICO_UXF_APPSCTL_INVISIBLE,
//            state == ICO_APF_RESOURCE_STATE_ACQUIRED ? 0 : 1);
    ICO_DBG("CicoSCResourceManager::enforceDisplay Leave");
}
                       
void
CicoSCResourceManager::enforceSound(unsigned short state,
                                    pid_t pid,
                                    void *user_data)
{
    ICO_DBG("CicoSCResourceManager::enforceSound Enter");
    /* NOP */
    ICO_DBG("CicoSCResourceManager::enforceSound Leave");
}
            
void
CicoSCResourceManager::enforceInput(unsigned short state,
                                    const char *appid,
                                    const char *device,
                                    void *user_data)
{
    ICO_DBG("CicoSCResourceManager::enforceInput Enter");
#if 0
    ico_apc_inputsw_t *czone;
    int i;

    for (i = 0; i < ninputsw; i++) {
        if (strcasecmp(inputsw[i].inputsw->swname, device) == 0)  break;
    }

    if (i >= ninputsw) {
        i = confsys->misc.default_inputswId;
    }
    czone = &inputsw[i];

    if (ico_uxf_input_control(state == ICO_APF_RESOURCE_STATE_ACQUIRED ? 1 : 0,
                              appid, czone->inputdev->device,
                              czone->inputsw->input) != ICO_UXF_EOK) {
        ICO_WRN("failed to enforce input control");
    }
#endif
    ICO_DBG("CicoSCResourceManager::enforceInput Leave");
}

// receive changed state
void
CicoSCResourceManager::receiveChangedState(int state)
{
    ICO_DBG("CicoSCResourceManager::receiveChanagedState Enter");
    ICO_DBG("CicoSCResourceManager::receiveChanagedState Leave");
}
// vim:set expandtab ts=4 sw=4:
