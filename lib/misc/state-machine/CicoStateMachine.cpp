/*
 * Copyright (c) 2013 TOYOTA MOTOR CORPORATION.
 *
 * Contact: 
 *
 * Licensed under the Apache License, Version 2.0 (the License){}
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. 
 */
#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateCore.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoFinalState.h"
#include "CicoStateMachine.h"
#include "CicoStateInternal.h"

#if 1
bool SttMdbg = false;
#else
bool SttMdbg = true;
#endif

/**
 * state-machine object class definition
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief CicoStateMachine::CicoStateMachine
 * @param value Identification number
 */
CicoStateMachine::CicoStateMachine(int value):
    CicoStateCore(EStateMachine, value)
{
    m_eventExec = false;                // During event processing flag
}

/**
 * @brief CicoStateMachine::CicoStateMachine
 * @param name Identification name
 * @param value Identification number
 */
CicoStateMachine::CicoStateMachine(const std::string& name, int value):
    CicoStateCore(EStateMachine, name, value)
{
    m_eventExec = false;                // During event processing flag
}

/**
 * @brief CicoStateMachine::eventEntry
 * @param ev delivery of the event
 * @return true:Transition occurs false:The transition has not occurred
 */
bool CicoStateMachine::eventEntry(const CicoEvent& ev)
{
    if (SttMdbg) {
        std::string el;
        getLogPartF(ev, el);
        _DBG("CicoStateMachine::eventEntry(%s)", el.c_str());
    }
    if (true == m_eventExec) {          // Execution of the request will not be accepted during execution
        if (SttMdbg) {
            _DBG("CicoStateMachine::eventEntry() not be accepted");
        }
        return false;
    }
    m_eventExec = true;                 // During event processing flag

    std::vector<CicoState*> stt;        // store active state-objects
    std::vector<CicoStateCore::E_ActiveImpact> impact; // inpact flag area
    if (false == getActiveSonarChild(stt, impact)) { // get active objects and flags
                                        // get fail
        m_eventExec = false;            // During event processing flag off
        if (SttMdbg) {
            _DBG("false=CicoStateMachine::eventEntry() P1");
        }
        return false;
    }
    int sz = stt.size();
    if (0==sz) {
        m_eventExec = false;            // During event processing flag off
        if (SttMdbg) {
            _DBG("false=CicoStateMachine::eventEntry() P2");
        }
        return false;
    }
    bool bR = false;
    for (int i=0; i< sz; i++) {         // sz is get active state-objects count
        CicoStateCore* s = (CicoStateCore*)stt[i]; // get target
        if (false == s->isActive()) {   // inactive ?
            continue;                   // next state-object
        }
        const CicoEventInfo* evi = eventTestF(s, ev, this); //
        if ((CicoEventInfo*)0 == evi) { //
            continue;                   // next state-objects
        }
        CicoStateCore* ds = evi->getTransition(); // Gets the transition destination
        /*
        *   ev  transition event trigger
        *   s   transition source
        *   evi registration event data
        *   ds  transition destination
        */
        if (SttMdbg) {
            std::string eil;
            getLogPartF(*evi, eil);
            _DBG("CicoStateMachine::eventEntry match %s", eil.c_str());
        }
        if (((CicoStateCore*)0 != ds) && (s != ds)) {
            onTransition(ev, s, ds);    // transition request
            bR = true;
        }
        else {
            bR = onDoExecF(s, ev);
        }
    }
    m_eventExec = false;                // During event processing flag off
    if (SttMdbg) {
        _DBG("%s=CicoStateMachine::eventEntry() P3", bR?"true":"false");
    }
    return bR;
}

/**
 * @brief CicoStateMachine::eventTest
 * @param ev delivery of the event
 * @return true:Transition occurs false:The transition has not occurred
 */
bool CicoStateMachine::eventTest(const CicoEvent& ev)
{
    if (SttMdbg) {
        std::string el;
        getLogPartF(ev, el);
        _DBG("CicoStateMachine::eventTest(%s)", el.c_str());
    }
    if (true == m_eventExec) {          // Execution of the request will not be accepted during execution
        if (SttMdbg) {
            _DBG("CicoStateMachine::eventTest() not be accepted");
        }
        return false;
    }
    std::vector<CicoState*> stt;
    std::vector<CicoStateCore::E_ActiveImpact> impact;
    if (false == getActiveSonarChild(stt, impact)) {
                                        // get fail
        if (SttMdbg) {
            _DBG("false=CicoStateMachine::eventTest() P1");
        }
        return false;
    }
    int sz = stt.size();
    if (0==sz) {
        if (SttMdbg) {
            _DBG("false=CicoStateMachine::eventTest() P2");
        }
        return false;
    }
    bool bR = false;
    for (int i=0; i< sz; i++) {
        CicoStateCore* s = (CicoStateCore*)stt[i];
        if (false == s->isActive()) {
            continue;
        }
        const CicoEventInfo* evi = eventTestF(s, ev, this);
        if ((CicoEventInfo*)0 != evi) {
            bR = true;
            break;
        }
    }
    if (SttMdbg) {
        _DBG("%s=CicoStateMachine::() P3", bR?"true":"false");
    }
    return bR;
}


/**
 * @brief CicoStateMachine::start
 * @return true:start success false:start fail
 */
bool CicoStateMachine::start()
{
    CicoEvent e(CicoEvent::Start);
    return CicoStateCore::start(e);
}

/**
 * @brief CicoStateMachine::stop
 * @return true:stop success false:stop fail
 */
bool CicoStateMachine::stop()
{
    CicoEvent e(CicoEvent::Stop);
    return CicoStateCore::stop(e);
}
