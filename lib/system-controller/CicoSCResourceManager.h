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
 *  @file   CicoSCResourceManager.h
 *
 *  @brief  This file is definition of CicoSCResourceManager class
 */
//==========================================================================
#ifndef __CICO_SC_RESOURCE_MANAGER_H__
#define __CICO_SC_RESOURCE_MANAGER_H__

#include <list>
#include <vector>
#include <map>
using namespace std;

#include "ico_syc_mrp_resource_private.h"

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoSCCommand;
class CicoSCPolicyManager;
class CicoSCWindowController;
class CicoSCInputController;

//==========================================================================
/*
 *  @brief  This class manages resources
 */
//==========================================================================
class CicoSCResourceManager
{
public:
    // default constructor
    CicoSCResourceManager();

    // destructor
    ~CicoSCResourceManager();

    // initialize resource manager
    int initialize(void);

    // terminate resource manager
    void terminate(void);

    // handling command
    void handleCommand(const CicoSCCommand &cmd, bool internal=false);

    // set CicoSCWindowController instance
    void setWindowController(CicoSCWindowController *winCtrl);

    // set CicoSCInputController instance
    void setInputController(CicoSCInputController *inputCtrl);

    // receive changed state
    void receiveChangedState(int state);

    // get resource manager instance
    CicoSCPolicyManager * getPolicyManager(void);

private:
    // assignment operator
    CicoSCResourceManager& operator=(const CicoSCResourceManager &object);

    // copy constructor
    CicoSCResourceManager(const CicoSCResourceManager &object);

    bool acquireDisplayResource(resource_request_t *newreq,
                                bool control = true);
    void releaseDisplayResource(resource_request_t *newreq);

    bool acquireSoundResource(resource_request_t *newreq);
    void releaseSoundResource(resource_request_t *newreq);

    bool acquireInputResource(resource_request_t *newreq);
    void releaseInputResource(resource_request_t *newreq);

    resource_request_t* newResourceRequest(int resid,
                                           int reqtype,
                                           const CicoSCCommand &cmd);

    void delResourceRequest(resource_request_t *req);

    static void enforceSound(unsigned short state,
                             resource_request_t *req,
                             void *user_data);

    resource_request_t * popDispResReq(resource_request_t *req);

    resource_request_t * popSoundResReq(resource_request_t *req);

    resource_request_t * popInputResReq(resource_request_t *req);

    void updateDisplayResource(resource_request_t *req, int chgzoneid=-1);
    void updateSoundResource(resource_request_t *req);
    void updateInputResource(resource_request_t *req);

    void updateDisplayResourceRegulation(int state);
    void updateSoundResourceRegulation(int state);
    void updateInputResourceRegulation(int state);

    void updateDispResRegulationPreProc(resource_request_t *req);
    void updateSoundResRegulationPreProc(resource_request_t *req);
    void updateInputResRegulationPreProc(resource_request_t *req);

    resource_request_t * findCurDispResOwerReq(resource_request_t *req);
    resource_request_t * popCurDispResOwerReq(resource_request_t *req);
    void dumpCurDispResOwerReq(void);

    resource_request_t * findWaitingDispResReq(resource_request_t *req);
    resource_request_t * popWaitingDispResReq(resource_request_t *req);
    void dumpWaitingDispResReq(void);

    bool isMatchDisplayed(void);

private:
    CicoSCPolicyManager       *m_policyMgr;
    CicoSCWindowController    *m_winCtrl;
    CicoSCInputController     *m_inputCtrl;

    // pair zoneid and request
    std::map<unsigned int, resource_request_t*> m_curDispResOwerReq;
    std::map<int, resource_request_t*> m_curSoundResReq;
    std::map<int, resource_request_t*> m_curInputResReq;

    std::list<resource_request_t*> m_waitingDispResReq;
    map<int, list<resource_request_t*> > m_soundReqQueue;
    map<int, list<resource_request_t*> > m_inputReqQueue;

    // show/hide animation name at regulation on or off
    const std::string m_animaName;

    // show/hide animation time at regulation on or off
    int m_animaTime;
};
#endif  // __CICO_SC_RESOURCE_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
