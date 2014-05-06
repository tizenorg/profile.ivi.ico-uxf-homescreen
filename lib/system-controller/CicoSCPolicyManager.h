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
 *  @brief  This file is definition of CicoSCPolicyManager class
 */
//==========================================================================
#ifndef __CICO_SC_POLICY_MANAGER_H__
#define __CICO_SC_POLICY_MANAGER_H__

#include <Ecore.h>
#include <Eina.h>
#include <dbus/dbus.h>

#include <map>
#include <vector>
#include <string>
using namespace std;

//==========================================================================
//  Forward declaration
//==========================================================================
class CicoStateMachine;
class CicoState;
class CicoSCResourceManager;

//==========================================================================
/*
 *  @brief  This class manages the policy of resource
 */
//==========================================================================
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

    bool acquireDisplayResource(int type, int zoneid, int priority);
    bool releaseDisplayResource(int zoneid, int category);

    bool acquireSoundResource(int type, int zoneid, int priority);
    bool releaseSoundResource(int input, int priority);

    bool acquireInputResource(int input, int priority);
    bool releaseInputResource(int input);

    bool testSMEvent(unsigned short event_id);
    bool testSMEvent(unsigned short event_id, int value);

    bool sendSMEvent(unsigned short event_id);
    bool sendSMEvent(unsigned short event_id, int value);

    void notifyChangedState(int state);

    // get states
    const std::map<int, const CicoState*>& getPolicyStates(void);

    bool getDispZoneState(int zoneid);
    bool getSoundZoneState(int zoneid) const;
    bool getInputState(int input) const;
    bool getRegulation(void);

    bool isExistDisplayZoneOwer(int zoneid);

    // notify connected process
    void notifyConnected(const std::string & appid);

private:
    // default constructor
    CicoSCPolicyManager();

    // assignment operator
    CicoSCPolicyManager& operator=(const CicoSCPolicyManager &object);

    // copy constructor
    CicoSCPolicyManager(const CicoSCPolicyManager &object);

    // initialize state machine
    int initStateMachine(void);

    // callback function for changed vehicle informantion
    void onChangedVehicleInfo(int type, void *event);

private:
    // callback function for changed vehicle informantion
    static Eina_Bool changedVehicleInfoCB(void *data, int type, void *event);

private:
    // initialized flag
    bool m_initialized;

    // ecore event handler
    Ecore_Event_Handler*  m_ecoreEvHandler;

    // CicoStateMachine instance
    CicoStateMachine      *m_stateMachine;

    // CicoSCResourceManager instance
    CicoSCResourceManager *m_resourceMgr;

    // state list
    std::map<int, const CicoState*> m_policyStates;

    // display zone state list
    std::map<int, const CicoState*> m_dispZoneStates;

    // sound zone state list
    std::vector<const CicoState*> m_soundZoneStates;

    // input state list
    std::vector<const CicoState*> m_inputStates;
};
#endif  // __CICO_SC_POLICY_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
