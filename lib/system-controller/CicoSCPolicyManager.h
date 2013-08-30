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
 *  @file   CicoSCPolicyManager.h
 *
 *  @brief 
 */
//==========================================================================
#ifndef __CICO_SC_POLICY_MANAGER_H__
#define __CICO_SC_POLICY_MANAGER_H__

#include <vector>
using namespace std;

#include <Eina.h>
#include <dbus/dbus.h>

class CicoStateMachine;
class CicoSCResourceManager;

class CicoSCPolicyManager
{
public:
    // constructor
    CicoSCPolicyManager(CicoSCResourceManager* resourceMgr=NULL);

    // destructor
    ~CicoSCPolicyManager();

    // initialize resource manager
    int initialize(void);

    // terminate resource manager
    void terminate(void);

    bool acquireDisplayResource(int zoneid, int category);
    bool releaseDisplayResource(int zoneid, int category);
    bool acquireInputResource(int zoneid, int category);
    bool releaseInputResource(int zoneid, int category);

    bool testSMEvent(unsigned short event_id);
    bool testSMEvent(unsigned short event_id, int value);

    bool sendSMEvent(unsigned short event_id);
    bool sendSMEvent(unsigned short event_id, int value);

    void notifyChangedState(int state);

    void recvAMBVehicleInfo(void);

private:
    // default constructor
    CicoSCPolicyManager();

    // assignment operator
    CicoSCPolicyManager& operator=(const CicoSCPolicyManager &object);

    // copy constructor
    CicoSCPolicyManager(const CicoSCPolicyManager &object);

    // initialize amb connection
    int initAMB(void);

    // send request to amb
    int sendAMBRequest(void);

    // get vehicle information
    int getVehicleInfo(void);

    static Eina_Bool ecoreTimerCB(void *user_data);

    int initStateMachine(void);

private:
    bool m_initialized;

    CicoStateMachine *m_stateMachine;

    CicoSCResourceManager *m_resourceMgr;

    vector<int> m_acquireDispResEventTable;
    vector<int> m_releaseDispResEventTable;
};
#endif  // __CICO_SC_POLICY_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
