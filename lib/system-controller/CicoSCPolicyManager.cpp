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
 *  @file   CicoSCPolicyManager.cpp
 *
 *  @brief  This file implementation of CicoSCPolicyManager class
 */
//==========================================================================

#include <string>

#include <Ecore.h>

#include "CicoSCPolicyManager.h"
#include "CicoStateMachine.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoStateMachineCreator.h"
#include "CicoSCPolicyDef.h"
#include "CicoSCResourceManager.h"
#include "CicoLog.h"
#include "ico_syc_error.h"
#include "CicoSCServer.h"
#include "CicoSCMessage.h"
#include "ico_syc_msg_cmd_def.h"
#include "ico_syc_type.h"
#include "CicoCommonDef.h"
#include "CicoSCVInfoManager.h"
#include "CicoSCVInfo.h"

//--------------------------------------------------------------------------
/**
 *  @brief  default constructor
 */
//--------------------------------------------------------------------------
CicoSCPolicyManager::CicoSCPolicyManager(CicoSCResourceManager* resourceMgr)
    : m_initialized(false),
      m_ecoreEvHandler(NULL),
      m_stateMachine(NULL),
      m_resourceMgr(resourceMgr)
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  destructor
 */
//--------------------------------------------------------------------------
CicoSCPolicyManager::~CicoSCPolicyManager()
{
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize policy manager
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::initialize(void)
{
    ICO_TRA("CicoSCPolicyManager::initialize Enter");

    int ret = initStateMachine();
    if (ICO_SYC_EOK != ret) {
        return ret;
    }

    CicoSCVInfoManager* vinfomgr = CicoSCVInfoManager::getInstance();
    m_ecoreEvHandler = ecore_event_handler_add(
                            vinfomgr->getEcoreEvType(),
                            CicoSCPolicyManager::changedVehicleInfoCB,
                            this);

    vinfomgr->getVehicleInfo(ICO_VINFO_NIGHT_MODE);
    vinfomgr->getVehicleInfo(ICO_VINFO_DRIVING_MODE);

    ICO_TRA("CicoSCPolicyManager::initialize Leave");
    return ret;
}
  
//--------------------------------------------------------------------------
/**
 *  @brief  terminate policy manager
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::terminate(void)
{
    ICO_TRA("CicoSCPolicyManager::terminate Enter");
    ICO_TRA("CicoSCPolicyManager::terminate Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::onChangedVehicleInfo(int type, void *event)
{
    ICO_TRA("CicoSCPolicyManager::onChangedVehicleInfo Enter");

    if (NULL == event) {
        ICO_WRN("data is null");
        return;
    }

    if (CicoSCVInfoManager::getInstance()->getEcoreEvType() != type) {
        ICO_WRN("unknown ecore event type(%d)", type);
        return;
    }

    bool chgCamera     = false;
    bool chgRegulation = false;
    bool chgNightMode  = false;

    CicoSCVInfo* vinfo = static_cast<CicoSCVInfo*>(event);
    switch (vinfo->getProperty()) {
    case ICO_VINFO_VEHICLE_SPEED:
        ICO_DBG("ICO_VINFO_VEHICLE_SPEED : %d", vinfo->getInt32Value());
//        (void)sendSMEvent(EVID_VELOCTY, (int)vinfo->getInt32Value());
//        if (true == sendSMEvent(EVID_DRVREGULATION)) {
//            chgRegulation = true;
//        }
        break;
    case ICO_VINFO_SHIFT_POSITION:
        ICO_DBG("ICO_VINFO_SHIFT_POSITION : %d", vinfo->getUint8Value());
        (void)sendSMEvent(EVID_SHIFTPOS, (int)vinfo->getUint8Value());
        if (true == sendSMEvent(EVID_CAMERA)) {
            chgCamera = true;
        }
        break;
    case ICO_VINFO_LIGHT_LEFT:
        ICO_DBG("ICO_VINFO_LIGHT_LEFT : %s",
                vinfo->getBooleanValue() ? "true" : "false");
        if (false == vinfo->getBooleanValue()) {
            (void)sendSMEvent(EVID_TURN_OFF);
        }
        else {
            (void)sendSMEvent(EVID_TURN_LEFT);
        }
        if (true == sendSMEvent(EVID_CAMERA)) {
            chgCamera = true;
        }
        break;
    case ICO_VINFO_LIGHT_RIGHT:
        ICO_DBG("ICO_VINFO_LIGHT_RIGHT: %s",
                vinfo->getBooleanValue() ? "true" : "false");
        if (false == vinfo->getBooleanValue()) {
            (void)sendSMEvent(EVID_TURN_OFF);
        }
        else {
            (void)sendSMEvent(EVID_TURN_RIGHT);
        }
        if (true == sendSMEvent(EVID_CAMERA)) {
            chgCamera = true;
        }
        break;
    case ICO_VINFO_NIGHT_MODE:
        ICO_DBG("ICO_VINFO_NIGHT_MODE : %d", vinfo->getUint32Value());
        chgNightMode = sendSMEvent(EVID_NIGHTMODE, (int)vinfo->getUint32Value());
        break;
    case ICO_VINFO_DRIVING_MODE:
        ICO_DBG("ICO_VINFO_DRIVING_MODE : %d", vinfo->getUint32Value());
        chgRegulation = sendSMEvent(EVID_DRVREGULATION, (int)vinfo->getUint32Value());
        break;
    default:
        ICO_WRN("not such key (%d)", vinfo->getProperty());
        break;
    }

    if (true == chgRegulation) {
        ICO_DBG("true == chgRegulation");
        // notify changed state to resource manager
        CicoState* state =
                (CicoState*)m_stateMachine->getState(STID_DRVREGULATION);
        if (NULL != state) {
            vector<const CicoState*> currents;
            state->getCurrentState(currents, CicoStateCore::ELvlTop);
            if (0 != currents.size()) {
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }

        // Notify regulation changed state
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
        message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_REGULATION);
        if (true == m_policyStates[STID_DRVREGULATION_ON]->isActive()) {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
        }
        else {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
        }
        CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
    }

    if (true == chgCamera) {
        ICO_DBG("true == chgCamera");
        // notify changed state to resource manager
        CicoState* state = (CicoState*)m_stateMachine->getState(STID_CAMERA);
        if (NULL != state) {
            vector<const CicoState*> currents;
            state->getCurrentState(currents, CicoStateCore::ELvlTop);
            if (0 != currents.size()) {
                ICO_DBG("current=%s", currents[0]->getName().c_str());
                notifyChangedState(currents[0]->getValue());
            }
        }
    }

    if (true == chgNightMode) {
        ICO_DBG("true == chgNightMode");
        // Notify NightMode changed state
        CicoSCMessage *message = new CicoSCMessage();
        message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
        message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_NIGHTMODE);
        if (true == m_policyStates[STID_NIGHTMODE_ON]->isActive()) {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
        }
        else {
            message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
        }
        CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
    }

    ICO_TRA("CicoSCPolicyManager::onChangedVehicleInfo Leave");
}

//--------------------------------------------------------------------------
/**
 *  @brief
 */
//--------------------------------------------------------------------------
Eina_Bool
CicoSCPolicyManager::changedVehicleInfoCB(void *data, int type, void *event)
{
    static_cast<CicoSCPolicyManager*>(data)->onChangedVehicleInfo(type, event);
    return EINA_TRUE;
}

//--------------------------------------------------------------------------
/**
 *  @brief  get policy states
 */
//--------------------------------------------------------------------------
const std::map<int, const CicoState*>&
CicoSCPolicyManager::getPolicyStates(void)
{
    return m_policyStates;
}

//--------------------------------------------------------------------------
/**
 *  @brief  initialize state machine
 */
//--------------------------------------------------------------------------
int
CicoSCPolicyManager::initStateMachine(void)
{
    ICO_TRA("CicoSCPolicyManager::initStateMachine Enter");

    CicoStateMachineCreator creator;

    //TODO
    m_stateMachine = creator.createFile("/usr/apps/org.tizen.ico.system-controller/res/config/policy.json");
    if (NULL == m_stateMachine) {
        ICO_ERR("CicoStateMachineCreator::createFile failed.(reason:%s)",
                creator.getError().c_str());
        return ICO_SYC_ENOSYS;
    }

    int ret = m_stateMachine->start();
    if (ret == 0) {
        ICO_ERR("CicoStateMachine::start failed.");
        ICO_TRA("CicoSCPolicyManager::initStateMachine Leave(ENOSYS)");
        return ICO_SYC_ENOSYS;
    }

    vector<CicoStateCore*> objects;
    m_stateMachine->getObjects(objects);
    vector<CicoStateCore*>::iterator itr;
    itr = objects.begin();
    for (; itr != objects.end(); ++itr) {
        const CicoState* state = static_cast<const CicoState*>(*itr);
        m_policyStates[state->getValue()] = state;
#if 1   //-- { debug dump
        ICO_DBG("State=[%-45s] Active=%s",
                state->getName().c_str(),
                state->isActive() ? "true" : "false");
#endif  //-- } debug dump
    }

    {
        std::map<int, const CicoState*>::iterator itr;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE1);
        m_dispZoneStates[1] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE2);
        m_dispZoneStates[2] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE3);
        m_dispZoneStates[3] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE4);
        m_dispZoneStates[4] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE5);
        m_dispZoneStates[5] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE6);
        m_dispZoneStates[6] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE7);
        m_dispZoneStates[7] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE8);
        m_dispZoneStates[8] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE9);
        m_dispZoneStates[9] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE10);
        m_dispZoneStates[10] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE11);
        m_dispZoneStates[11] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE12);
        m_dispZoneStates[12] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE13);
        m_dispZoneStates[13] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE14);
        m_dispZoneStates[14] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE15);
        m_dispZoneStates[15] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE16);
        m_dispZoneStates[16] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE17);
        m_dispZoneStates[17] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE18);
        m_dispZoneStates[18] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE19);
        m_dispZoneStates[19] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY0_ZONE20);
        m_dispZoneStates[20] = itr != m_policyStates.end() ? itr->second : NULL;

        itr = m_policyStates.find(STID_DISPLAY1_ZONE1);
        m_dispZoneStates[21] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE2);
        m_dispZoneStates[22] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE3);
        m_dispZoneStates[23] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE4);
        m_dispZoneStates[24] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE5);
        m_dispZoneStates[25] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE6);
        m_dispZoneStates[26] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE7);
        m_dispZoneStates[27] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE8);
        m_dispZoneStates[28] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE9);
        m_dispZoneStates[29] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE10);
        m_dispZoneStates[30] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE11);
        m_dispZoneStates[31] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE12);
        m_dispZoneStates[32] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE13);
        m_dispZoneStates[33] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE14);
        m_dispZoneStates[34] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE15);
        m_dispZoneStates[35] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE16);
        m_dispZoneStates[36] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE17);
        m_dispZoneStates[37] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE18);
        m_dispZoneStates[38] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE19);
        m_dispZoneStates[39] = itr != m_policyStates.end() ? itr->second : NULL;
        itr = m_policyStates.find(STID_DISPLAY1_ZONE20);
        m_dispZoneStates[40] = itr != m_policyStates.end() ? itr->second : NULL;
    }

    m_soundZoneStates.push_back(NULL);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE1]);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE2]);
    m_soundZoneStates.push_back(m_policyStates[STID_SOUND_ZONE3]);

    m_inputStates.push_back(NULL);
    m_inputStates.push_back(m_policyStates[STID_INPUT1_USING]);
    m_inputStates.push_back(m_policyStates[STID_INPUT2_USING]);

    ICO_TRA("CicoSCPolicyManager::initStateMachine Leave(EOK)");
    return ICO_SYC_EOK;
}

//--------------------------------------------------------------------------
/**
 *  @brief  query whether a state transition
 *
 *  @param [in] event_id    trigger event id
 *
 *  @return true on test success, false on test failed
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventTest(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  query whether a state transition
 *
 *  @param [in] event_id    trigger event id
 *  @param [in] value       trigger optional integer value
 *
 *  @return true on test success, false on test failed
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::testSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventTest(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  send tigger event
 *
 *  @param [in] event_id    trigger event id
 *
 *  @return true on state transition, false on not state transition
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id)
{
    CicoEvent event(event_id);
    return m_stateMachine->eventEntry(event);
}

//--------------------------------------------------------------------------
/**
 *  @brief  send tigger event
 *
 *  @param [in] event_id    trigger event id
 *  @param [in] value       trigger optional integer value
 *
 *  @return true on state transition, false on not state transition
 */
//--------------------------------------------------------------------------
bool
CicoSCPolicyManager::sendSMEvent(unsigned short event_id, int value)
{
    CicoEvent event(event_id, value);
    return m_stateMachine->eventEntry(event);
}

bool
CicoSCPolicyManager::acquireDisplayResource(int type, int zoneid, int priority)
{
    ICO_TRA("CicoSCPolicyManager::acquireDisplayResource Enter"
            "(type=0x%08X zoneid=%d priority=%d)", type, zoneid, priority);

    bool chg = false;

    if (RESID_TYPE_BASIC == type) {
#if 1  // MK20131223 _MK_MK_
        unsigned short category_ev = EVID_DISPLAY0_CATEGORY;
        if (ZONEID_CENTER_UPPER == zoneid) {
            category_ev = EVID_DISP0_ZONE2_CATEGORY;
            
        }
        else if (ZONEID_CENTER_LOWER == zoneid) {
            category_ev = EVID_DISP0_ZONE3_CATEGORY;
        }

        bool zoneChg = testSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
        bool cateChg = testSMEvent(category_ev, priority);
        ICO_DBG("zoneChg=%d cateChg=%d", zoneChg, cateChg);
        if ((true == zoneChg) && (true == cateChg)) {
            sendSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
            sendSMEvent(category_ev, priority);
            chg = true;
        }
#else  // MK20131223 _MK_MK_
        bool zoneChg = testSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
        bool cateChg = testSMEvent(EVID_DISPLAY0_CATEGORY, priority);
        ICO_DBG("zoneChg=%d cateChg=%d", zoneChg, cateChg);
        if ((true == zoneChg) && (true == cateChg)) {
            sendSMEvent(EVID_DISPLAY_ZONE_ACQUIRE, zoneid);
            sendSMEvent(EVID_DISPLAY0_CATEGORY, priority);
            chg = true;
        }
#endif  // MK20131223 _MK_MK_
#if 0   //-- { debug dump
        else {
            std::map<int, const CicoState*>::iterator itr;
            itr = m_policyStates.begin();
            for (; itr != m_policyStates.end(); ++itr) {
                ICO_DBG("State=[%-45s] Active=%s",
                        itr->second->getName().c_str(),
                        itr->second->isActive() ? "true" : "false");
            }
        }
#endif  //-- } debug dump
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z1, priority);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z2, priority);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_D0_Z3, priority);
        }
    }
    else if (RESID_TYPE_ONSCREEN == type) {
        chg = sendSMEvent(EVID_ONSCREEN, priority);
    }
    ICO_TRA("CicoSCPolicyManager::acquireDisplayResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseDisplayResource(int zoneid, int priority)
{
    return sendSMEvent(EVID_DISPLAY_ZONE_RELEASE, zoneid);
}

bool
CicoSCPolicyManager::acquireSoundResource(int type, int zoneid, int priority)
{
    ICO_TRA("CicoSCPolicyManager::acquireSoundResource Enter"
            "(type=0x%08X zoneid=%d priority=%d)", type, zoneid, priority);

    bool chg = false;

    if (RESID_TYPE_BASIC == type) {
        bool zoneChg = testSMEvent(EVID_SOUND_ZONE, zoneid);
        bool cateChg = testSMEvent(EVID_SOUND_CATEGORY, priority);
        ICO_DBG("zoneChg=%d cateChg=%d", zoneChg, cateChg);
        if ((true == zoneChg) && (true == cateChg)) {
            sendSMEvent(EVID_SOUND_ZONE, zoneid);
            sendSMEvent(EVID_SOUND_CATEGORY, priority);
            chg = true;
        }
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z1, priority);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z2, priority);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z3, priority);
        }
    }

    ICO_TRA("CicoSCPolicyManager::acquireSoundResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseSoundResource(int type, int zoneid)
{
    ICO_TRA("CicoSCPolicyManager::acquireSoundResource Enter"
            "(type=%d zoneid=%d)", type, zoneid);

    bool chg = false;
    if (RESID_TYPE_BASIC == type) {
        chg = sendSMEvent(EVID_SOUND_ZONE_NOUSE);
        chg = sendSMEvent(EVID_SOUND_CATEGORY_UNKNOWN);
    }
    else if (RESID_TYPE_INTERRUPT == type) {
        if (1 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z1_NOOUTPUT);
        }
        else if (2 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z2_NOOUTPUT);
        }
        else if (3 == zoneid) {
            chg = sendSMEvent(EVID_INTTERPUT_S_Z3_NOOUTPUT);
        }
    }

    ICO_TRA("CicoSCPolicyManager::acquireSoundResource Leave(%s)",
            chg ? "true" : "false");

    return true;
}

bool
CicoSCPolicyManager::acquireInputResource(int input, int priority)
{
    ICO_TRA("CicoSCPolicyManager::acquireInputResource Enter"
            "(input=%d priority=%d)", input, priority);

    bool chg = false;

    if (1 == input) {
        chg = testSMEvent(EVID_INPUT1_ACQUIRE, input);
        if (true == chg) {
            sendSMEvent(EVID_INPUT1_ACQUIRE, input);
        }
    }
    else if (2 == input) {
        chg = testSMEvent(EVID_INPUT2_ACQUIRE, input);
        if (true == chg) {
            sendSMEvent(EVID_INPUT2_ACQUIRE, input);
        }
    }

    ICO_TRA("CicoSCPolicyManager::acquireInputResource Leave(%s)",
            chg ? "true" : "false");
    return chg;
}

bool
CicoSCPolicyManager::releaseInputResource(int input)
{
    if (1 == input) {
        (void)sendSMEvent(EVID_INPUT1_RELEASE, input);
    }
    else if (2 == input) {
        (void)sendSMEvent(EVID_INPUT2_RELEASE, input);
    }
    return true;
}

void
CicoSCPolicyManager::notifyChangedState(int state)
{
    m_resourceMgr->receiveChangedState(state);
}

bool
CicoSCPolicyManager::getDispZoneState(int zoneid)
{
    if (0 >= zoneid) {
        return false;
    }

    // find state instance
    std::map<int, const CicoState*>::iterator itr;
    itr = m_dispZoneStates.find(zoneid);
    if (itr == m_dispZoneStates.end()) {
        return false;
    }

    // if state instance is null
    if (NULL == itr->second) {
        return false;
    }
        
    return itr->second->isActive();
}

bool
CicoSCPolicyManager::getSoundZoneState(int zoneid) const
{
    if ((0 < zoneid) && ((int)m_soundZoneStates.size()-1 > zoneid)) {
        return m_soundZoneStates[zoneid]->isActive();
    }
    return false;
}

bool
CicoSCPolicyManager::getInputState(int input) const
{
    if ((0 < input) && ((int)m_inputStates.size()-1 > input)) {
        return m_inputStates[input]->isActive();
    }
    return false;
}

bool
CicoSCPolicyManager::getRegulation(void)
{
    return m_policyStates[STID_DRVREGULATION_ON]->isActive();
}

bool
CicoSCPolicyManager::isExistDisplayZoneOwer(int zoneid)
{
    if ((zoneid >= ICO_DISPLAY0_ZONEID_MIN) &&
        (zoneid <= ICO_DISPLAY0_ZONEID_MAX)) {
        return !m_policyStates[STID_DISPLAY0_NOOWER]->isActive();
    }
    if ((zoneid >= ICO_DISPLAY1_ZONEID_MIN) &&
        (zoneid <= ICO_DISPLAY1_ZONEID_MAX)) {
        return !m_policyStates[STID_DISPLAY1_NOOWER]->isActive();
    }
    return true;
}

//--------------------------------------------------------------------------
/**
 *  @brief  notify connected
 */
//--------------------------------------------------------------------------
void
CicoSCPolicyManager::notifyConnected(const std::string & appid)
{
    // Notify regulation changed state
    CicoSCMessage *message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
    message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_REGULATION);
    if (true == m_policyStates[STID_DRVREGULATION_ON]->isActive()) {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
    }
    else {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
    }
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);

    // Notify NightMode changed state
    message = new CicoSCMessage();
    message->addRootObject("command", MSG_CMD_NOTIFY_CHANGED_STATE);
    message->addArgObject(MSG_PRMKEY_STATEID, ICO_SYC_STATE_NIGHTMODE);
    if (true == m_policyStates[STID_NIGHTMODE_ON]->isActive()) {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_ON);
    }
    else {
        message->addArgObject(MSG_PRMKEY_STATE, ICO_SYC_STATE_OFF);
    }
    CicoSCServer::getInstance()->sendMessageToHomeScreen(message);
}
// vim:set expandtab ts=4 sw=4:
