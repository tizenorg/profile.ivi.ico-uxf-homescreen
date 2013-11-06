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
#include <E_DBus.h>

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

    void recvAMBVehicleInfo(void);

    // get states
    const std::map<int, const CicoState*>& getPolicyStates(void);

    bool getDispZoneState(int zoneid);
    bool getSoundZoneState(int zoneid) const;
    bool getInputState(int input) const;
    bool getRegulation(void);

    bool isExistDisplayZoneOwer(int zoneid);

    // notify connected process
    void notifyConnected(const std::string & appid);

    static void AMBpropertyChanged(void *data, DBusMessage *msg);

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

    // timer callback function for ecore
    static Eina_Bool ecoreTimerCB(void *user_data);

    // initialize state machine
    int initStateMachine(void);

private:
    bool                  m_initialized;
    E_DBus_Connection     *m_dbusConnection;
    Ecore_Timer           *m_ecoreTimer;

    CicoStateMachine      *m_stateMachine;
    CicoSCResourceManager *m_resourceMgr;

    std::map<int, const CicoState*> m_policyStates;
    std::map<int, const CicoState*> m_dispZoneStates;
    std::vector<const CicoState*> m_soundZoneStates;
    std::vector<const CicoState*> m_inputStates;
};
#endif  // __CICO_SC_POLICY_MANAGER_H__
// vim:set expandtab ts=4 sw=4:
