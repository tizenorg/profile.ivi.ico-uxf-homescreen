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
 *  @brief 
 */
//==========================================================================
#ifndef __CICO_SC_RESOURCE_MANAGER_H__
#define __CICO_SC_RESOURCE_MANAGER_H__

#include <list>
using namespace std;

#include "ico_syc_mrp_resource_private.h"

class CicoSCCommand;
class CicoSCPolicyManager;
class CicoSCWindowController;
class CicoSCInputController;

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

private:
    // assignment operator
    CicoSCResourceManager& operator=(const CicoSCResourceManager &object);

    // copy constructor
    CicoSCResourceManager(const CicoSCResourceManager &object);

    bool acquireDisplayResource(resource_request_t *req, int addprio);
    bool releaseDisplayResource(resource_request_t *req);

    bool acquireSoundResource(resource_request_t *req, int addprio);
    bool releaseSoundResource(resource_request_t *req);

    bool acquireInputResource(resource_request_t *req, int addprio);
    bool releaseInputResource(resource_request_t *req);


    resource_request_t* newResourceRequest(int resid,
                                           int reqtype,
                                           const CicoSCCommand &cmd);

    void delResourceRequest(resource_request_t *req);

    static void enforceDisplay(unsigned short state,
                               const char *appid,
                               unsigned int id,
                               void *user_data);

    static void enforceSound(unsigned short state,
                             pid_t pid,
                             void *user_data);

    static void enforceInput(unsigned short state,
                             const char *appid,
                             const char *device,
                             void *user_data);

private:
    CicoSCPolicyManager    *m_policyMgr;
    CicoSCWindowController *m_winCtrl;
    CicoSCInputController  *m_inputCtrl;

    list<list<resource_request_t*> > m_dispReqQueue;
    list<list<resource_request_t*> > m_soundReqQueue;
    list<list<resource_request_t*> > m_inputReqQueue;
};
#endif  // __CICO_SC_RESOURCE_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
