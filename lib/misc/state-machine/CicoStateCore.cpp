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
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateAction.h"
#include "CicoStateCore.h"
#include "CicoState.h"
#include "CicoStateInternal.h"

extern bool SttMdbg;

/**
 * It is the core definition of the state transition
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief CicoStateCore::CicoStateCore
 * @param ts type of state-object class
 * @param parent parent state-object
 */
CicoStateCore::CicoStateCore(E_TOSO ts, CicoStateCore* parent):
    m_eType(ts)
{
    m_activeState      = false;
    m_value            = 0;
    m_parent           = (CicoStateCore*)0;
    m_existHistoy      = false;
    m_stateTermination = (CicoStateCore*)0;
    // Set the parent-child relationship
    addStateF(parent, this);
    initTransition();
    initStartPosition();
    initSttAction();
}

/**
 * @brief CicoStateCore::CicoStateCore
 * @param ts type of state-object class
 * @param name Identification name
 * @param parent parent state-object
 */
CicoStateCore::CicoStateCore(E_TOSO ts, const std::string& name,
                             CicoStateCore* parent):
    m_name(name), m_eType(ts)
{
    m_activeState      = false;
    m_value            = 0;
    m_parent           = (CicoStateCore*)0;
    m_existHistoy      = false;
    m_stateTermination = (CicoStateCore*)0;
    // Set the parent-child relationship
    addStateF(parent, this);
    initTransition();
    initStartPosition();
    initSttAction();
}

/**
 * @brief CicoStateCore::CicoStateCore
 * @param ts type of state-object class
 * @param name Identification name
 * @param value Identification number
 * @param parent parent state-object
 */
CicoStateCore::CicoStateCore(E_TOSO ts, const std::string& name, int value,
                             CicoStateCore* parent):
    m_name(name), m_value(value), m_eType(ts)
{
    m_activeState      = false;
    m_parent           = (CicoStateCore*)0;
    m_existHistoy      = false;
    m_stateTermination = (CicoStateCore*)0;
    // Set the parent-child relationship
    addStateF(parent, this);
    initTransition();
    initStartPosition();
    initSttAction();
}

/**
 * @brief CicoStateCore::CicoStateCore
 * @param ts type of state-object class
 * @param value Identification number
 * @param parent parent state-object
 */
CicoStateCore::CicoStateCore(E_TOSO ts, int value, CicoStateCore* parent):
    m_value(value),  m_eType(ts)
{
    m_activeState      = false;
    m_parent           = (CicoStateCore*)0;
    m_existHistoy      = false;
    m_stateTermination = (CicoStateCore*)0;
    // Set the parent-child relationship
    addStateF(parent, this);
    initTransition();
    initStartPosition();
    initSttAction();
}

/**
 * @brief CicoStateCore::~CicoStateCore
 */
CicoStateCore::~CicoStateCore()
{
    // Release area childs state-object
    int sz = m_childs.size();
    for (int i = 0; i < sz; i++) {
        delete (m_childs[i]);
    }
    m_childs.clear();
    // transition area clear
    clearTransition();
    // start position area clear
    clearStartPosition();
    // entry/exit/do action are clear
    clearSttAction();
}

/**
 * state-object activate
 * @param ev trigger event
 * @return
 */
/**
 * @brief CicoStateCore::start
 * @param ev trigger event data
 * @param parent Operation in the direction of the parent
 * @return true:Activation success false:Activation fail
 */
bool CicoStateCore::start(const CicoEvent& ev, bool parent)
{
    if (true == m_activeState) {        // Already running state-object
        return false;                   // do not do anything
    }

    toActivate(ev);

    if (false == parent) {              // Has no parent(Avoid the initialization of the parent)
        return true;                    // do not do anything
    }

    int sz = m_childs.size();           // 
    if (0 == sz) {                      // 
        return true;                    // 
    }

    // operate starting position
    if (EPDir == m_startPosition.suk) {
        startF(m_startPosition.u.d, ev, true);
        return true;
    }
    else if (EPVec == m_startPosition.suk) {
        // Perform a plurality of start position
        int sz2 = m_startPosition.u.v->size();
        for (int i = 0; i < sz2; i++) {
            startF((*m_startPosition.u.v)[i], ev, true);
        }
        return true;
    }

    // Start position is not registered
#if 0
    {
        std::string s;
        getLogPartF(this, s);
        _INFO("START POSITION IS NOT REGISTERED:%s", s.c_str());
    }
#endif
    bool dup = false;                       // duplication
    CicoStateCore* chs = (CicoStateCore*)0;
    for (int i = 0; i < sz; i++) {
        if (true == m_childs[i]->isHistoryState()) {
            if ((CicoStateCore*)0 != chs) {
                dup = true;
            }
            else {
                chs = m_childs[i];
            }
        }
    }
    if (((CicoStateCore*)0 != chs) && (false == dup)) {
        startF(chs, ev, true);
        return true;
    }
    std::vector<CicoStateCore*> sto;
    if (1 != sz) {                      // 
        int cnt = 0;
        getRegisteredInit(sto, cnt);
        if (1 != cnt) {
            {
                std::string s;
                getLogPartF(this, s);
                _ERR("START POSITION IS UNKNOWN:%d:%s", cnt,s.c_str());
            }
            return false;
        }
    }
    else {
        sto.push_back(m_childs[0]);
    }
    sz = sto.size();
    for (int i = 0; i < sz; i++) {      // 
        startF(sto[i], ev, true);       // 
    }
    return true;
}

/**
 * @brief CicoStateCore::stop
 * @param ev trigger event data
 * @return true:Deactivation success false:Deactivation fail
 */
bool CicoStateCore::stop(const CicoEvent& ev)
{
    if (false == m_activeState) {
        return false;
    }
    toDeactivate(ev);
    return true;
}

/**
 * @brief CicoStateCore::setParent
 * @param parent registration parent data
 */
void CicoStateCore::setParent(CicoStateCore* parent)
{
    m_parent = parent;
}


/**
 * @brief setParentF
 * @param stt registration target
 * @param parent registration parent data
 */
void setParentF(CicoStateCore* stt, CicoStateCore* parent)
{
    stt->setParent(parent);
}

/**
 * @brief CicoStateCore::setName
 * @param name Identification name
 */
void CicoStateCore::setName(const std::string& name)
{
    m_name = name;
}

/**
 * @brief CicoStateCore::addState
 * @param state child registration data
 */
void CicoStateCore::addState(CicoStateCore* state)
{
    if ((CicoStateCore*)0 != state) {   // If the parameters are correct
        m_childs.push_back(state);      // Recognized as a child
        state->setParent(this);         // I'm a parent
        if (true == state->isHistoryState()) { // history state object
            m_existHistoy = true;       // flag on
        }
    }
}

/**
 * @brief CicoStateCore::addTransition
 * @param evInf event registration
 * @param state Transition destination data
 */
void CicoStateCore::addTransition(const CicoEventInfo& evInf,
                                  CicoStateCore* state)
{
    CicoEventInfo *ei = new CicoEventInfo(evInf); // copy object
    ei->setTransition(state);           // Set the destination state object

    if (EPUnused == m_eventInfo.suk) {  // unused
        m_eventInfo.u.d = ei;
        m_eventInfo.suk = EPDir;
    }
    else if (EPDir == m_eventInfo.suk) {
        CicoEventInfo* tmp = m_eventInfo.u.d;
        m_eventInfo.u.d = (CicoEventInfo*)0;
        m_eventInfo.u.v = new std::vector<CicoEventInfo*>;
        m_eventInfo.suk = EPVec;
        m_eventInfo.u.v->push_back(tmp);
        m_eventInfo.u.v->push_back(ei);
    }
    else if (EPVec == m_eventInfo.suk) {
        m_eventInfo.u.v->push_back(ei);
    }
}

/**
 * @brief CicoStateCore::addInitState
 * @param state start position data
 */
void CicoStateCore::addInitState(CicoStateCore* state)
{
    if (EPUnused == m_startPosition.suk) {
        m_startPosition.u.d = state;
        m_startPosition.suk = EPDir;
    }
    else if (EPDir == m_startPosition.suk) {
        CicoStateCore* tmp = m_startPosition.u.d;
        m_startPosition.u.d = (CicoStateCore*)0;

        m_startPosition.u.v = new std::vector<CicoStateCore*>;
        m_startPosition.u.v->push_back(tmp);
        m_startPosition.u.v->push_back(state);
        m_startPosition.suk = EPVec;
    }
    else if (EPVec == m_startPosition.suk) {
        m_startPosition.u.v->push_back(state);
    }
}

/**
 * @brief CicoStateCore::setFinishTransition
 * @param state Transition destination termination detection
 */
void CicoStateCore::setFinishTransition(CicoStateCore* state)
{
    m_stateTermination = state;
}

/**
 * @brief CicoStateCore::addEntryAction
 * @param action registration action
 * @param addval registration additional value number
 */
void CicoStateCore::addEntryAction(CicoStateAction* action, int addval)
{
    if ((CicoStateAction*)0 == action) {
        return;
    }
    addAction(m_entry, action, addval);
}

/**
 * @brief CicoStateCore::addExitAction
 * @param action registration action
 * @param addval registration additional value number
 */
void CicoStateCore::addExitAction(CicoStateAction* action, int addval)
{
    if ((CicoStateAction*)0 == action) {
        return;
    }
    addAction(m_exit, action, addval);
}

/**
 * @brief CicoStateCore::addDoAction
 * @param action registration action
 * @param addval registration additional value number
 */
void CicoStateCore::addDoAction(CicoStateAction* action, int addval)
{
    if ((CicoStateAction*)0 == action) {
        return;
    }
    addAction(m_do, action, addval);
}

/**
 * @brief CicoStateCore::addAction
 * @param ssa Registration destination
 * @param action registration action
 * @param addval registration additional value number
 */
void CicoStateCore::addAction(stSttAction& ssa, CicoStateAction* action, int addval)
{
    if (EPUnused == ssa.suk) {
        ssa.u.d.dcsa = action;
        ssa.u.d.dav  = addval;
        ssa.suk      = EPDir;
    }
    else if (EPDir == ssa.suk) {
        CicoStateAction* tmpActn = ssa.u.d.dcsa;
        int tmpAddval            = ssa.u.d.dav;
        ssa.u.d.dcsa             = (CicoStateAction*)0;
        ssa.u.d.dav              = 0;
        ssa.u.v.vcsa  = new std::vector<CicoStateAction*>; // entry action
        ssa.u.v.vav = new std::vector<int>; // entry action additional value
        ssa.suk = EPVec;
        ssa.u.v.vcsa->push_back(tmpActn);
        ssa.u.v.vav->push_back(tmpAddval);
        ssa.u.v.vcsa->push_back(action);
        ssa.u.v.vav->push_back(addval);
        
    }
    else if (EPVec == ssa.suk) {
        ssa.u.v.vcsa->push_back(action);
        ssa.u.v.vav->push_back(addval);
    }
}

/**
 * @brief CicoStateCore::toActivate
 * @param ev Trigger event data
 */
void CicoStateCore::toActivate(const CicoEvent& ev)
{

    if (true == m_activeState) {
        return;
    }

    if ((CicoStateCore*)0 != m_parent) {
        toActivateF(m_parent, ev);
    }

    m_activeState = true;               // activate

    onEntry(ev);                        // entry
    if (SttMdbg) {
        std::string sl;
        getLogPartF(this, sl);
        std::string el;
        getLogPartF(ev, el);
        _DBG("%s.toActivate(%s)", sl.c_str(), el.c_str());
    }
}

/**
 * @brief toActivateF
 * @param stt Activate target
 * @param ev Trigger event data
 */
void toActivateF(CicoStateCore* stt, const CicoEvent& ev)
{
    if (stt) {
        stt->toActivate(ev);
    }
}

/**
 * @brief CicoStateCore::toDeactivate
 * @param ev Trigger event data
 */
void CicoStateCore::toDeactivate(const CicoEvent& ev)
{
    if (false == m_activeState) {
        return;
    }
    int sz = m_childs.size();
    for (int i = 0; i < sz; i++) {
        toDeactivateF(m_childs[i], ev);
    }
    onExit(ev);                         // exit
    m_activeState = false;              // not active
    if (SttMdbg) {
        std::string sl;
        getLogPartF(this, sl);
        std::string el;
        getLogPartF(ev, el);
        _DBG("%s.toDeactivate(%s)", sl.c_str(), el.c_str());
    }
}

/**
 * @brief toDeactivateF
 * @param stt Deactivate target
 * @param ev Trigger event data
 */
void toDeactivateF(CicoStateCore* stt, const CicoEvent& ev)
{
    if (stt) {
        stt->toDeactivate(ev);
    }
}


/**
 * @brief CicoStateCore::onEntry
 * @param ev Trigger event data
 */
void CicoStateCore::onEntry(const CicoEvent& ev)
{
    if (EPDir == m_entry.suk) {
        onEntryRun(m_entry.u.d.dcsa, ev, m_entry.u.d.dav);
    }
    else if (EPVec == m_entry.suk) {
        int sz = m_entry.u.v.vcsa->size();
        for (int i = 0; i < sz; i++) {
            onEntryRun((*m_entry.u.v.vcsa)[i], ev, (*m_entry.u.v.vav)[i]);
        }
    }
}

/**
 * @brief CicoStateCore::onEntryRun
 * @param run target object
 * @param ev Trigger event data
 * @param val additional value number
 */
void CicoStateCore::onEntryRun(CicoStateAction* run, const CicoEvent& ev,
                               int val)
{
    if (run) {
        if (true == isState()) {
            run->onEntry(ev, (CicoState*)this, val);
        }
        else if (true == isFinalState()) {
            run->onEntry(ev, (CicoFinalState*)this, val);
        }
        else if (true == isHistoryState()) {
            run->onEntry(ev, (CicoHistoryState*)this, val);
        }
    }
}

/**
 * @brief CicoStateCore::onExit
 * @param ev Trigger event data
 */
void CicoStateCore::onExit(const CicoEvent& ev)
{
    if (EPDir == m_exit.suk) {
        onExitRun(m_exit.u.d.dcsa, ev, m_exit.u.d.dav);
    }
    else if (EPVec == m_exit.suk) {
        int sz = m_exit.u.v.vcsa->size();
        for (int i = 0; i < sz; i++) {
            onExitRun((*m_exit.u.v.vcsa)[i], ev, (*m_exit.u.v.vav)[i]);
        }
    }
}

/**
 * @brief CicoStateCore::onExitRun
 * @param run target object
 * @param ev Trigger event data
 * @param val additional value number
 */
void CicoStateCore::onExitRun(CicoStateAction* run, const CicoEvent& ev,
                               int val)
{
    if (run) {
        if (true == isState()) {
            run->onExit(ev, (CicoState*)this, val);
        }
        else if (true == isHistoryState()) {
            run->onExit(ev, (CicoHistoryState*)this, val);
        }
    }
}

/**
 * @brief CicoStateCore::onDo
 * @param ev Trigger event data
 */
void CicoStateCore::onDo(const CicoEvent& ev)
{
    if (EPDir == m_do.suk) {
        onDoRun(m_do.u.d.dcsa, ev, m_do.u.d.dav);
    }
    else if (EPVec == m_do.suk) {
        int sz = m_do.u.v.vcsa->size();
        for (int i = 0; i < sz; i++) {
            onDoRun((*m_do.u.v.vcsa)[i], ev, (*m_do.u.v.vav)[i]);
        }
    }
}

/**
 * @brief CicoStateCore::onDoRun
 * @param run target object
 * @param ev Trigger event data
 * @param val additional value number
 */
void CicoStateCore::onDoRun(CicoStateAction* run, const CicoEvent& ev, int val)
{
    if (run) {
        if (true == isState()) {
            run->onDo(ev, (CicoState*)this, val);
        }
    }
}

/**
 * @brief CicoStateCore::getActiveSonar
 * @param stt store objects
 * @param impact store impact data
 * @return true:get success fail:get fail
 */
bool CicoStateCore::getActiveSonar(std::vector<CicoState*>& stt,
                                   std::vector<E_ActiveImpact>& impact)
{
    if (false == m_activeState) {
        return false;
    }
    stt.push_back((CicoState*)this);
    impact.push_back(Single);
    int szbk = impact.size();

    bool bR = getActiveSonarChild(stt,  impact);
    if (true==bR) {
        impact[ szbk-1 ] = Follower;    // Single -> Follower
    }
    return true;
}

/**
 * @brief CicoStateCore::getActiveSonarChild
 * @param stt store objects
 * @param impact store impact data
 * @return true:get success fail:get fail
 */
bool CicoStateCore::getActiveSonarChild(std::vector<CicoState*>& stt,
                                        std::vector<E_ActiveImpact>& impact)
{
    bool bR = false;
    int sz = m_childs.size();
    for (int i = 0; i < sz; i++) {
        bR |= (m_childs[i])->getActiveSonar(stt, impact);
    }
    return bR;
}

/**
 * @brief CicoStateCore::eventTest
 * @param ev event data
 * @param sm my state-machine object
 * @return true:Transition occurs false:The transition has not occurred
 */
const CicoEventInfo* CicoStateCore::eventTest(const CicoEvent& ev,
                                              const CicoStateCore* sm) const
{
    const CicoEventInfo* r = (CicoEventInfo*)0;
    if (EPDir == m_eventInfo.suk) {
        if (true == m_eventInfo.u.d->Judge(ev, sm)) {
            r = m_eventInfo.u.d;
        }
    }
    else if (EPVec == m_eventInfo.suk) {
        int sz = m_eventInfo.u.v->size();
        for (int i = 0; i < sz; i++) {
            const CicoEventInfo* t = (*m_eventInfo.u.v)[i];
            if (true == t->Judge(ev, sm)) { // event match?
                r = t;
                break;                  // break of for i<sz
            }
        }
    }
    return r;
}

/**
 * @brief eventTestF
 * @param stt event test target
 * @param ev event data
 * @param sm stt state-machine object
 * @return true:Transition occurs false:The transition has not occurred
 */
const CicoEventInfo* eventTestF(const CicoStateCore* stt,
                                const CicoEvent& ev,
                                const CicoStateCore* sm)
{
    if (stt) {
        return stt->eventTest(ev, sm);
    }
    return (CicoEventInfo*)0;
}

/**
 * @brief addStateF
 * @param parent parent state-object
 * @param state child state-object
 */
void addStateF(CicoStateCore* parent, CicoStateCore* state)
{
    if (parent) {
        parent->addState(state);
    }
}

/**
 * @brief CicoStateCore::holdHistory
 */
void CicoStateCore::holdHistory()
{
    if (false == m_existHistoy) {       // have historyState under
        return;                         // no have is return
    }
    // Ihave history state-object  Stores a history
    int sz = m_childs.size();          // get child count
    for (int i = 0; i < sz; i++) {     // 
        CicoStateCore* t = (m_childs[i]); //
        if (true == t->isHistoryState()) { // is historyState-object?
            holdHistoryF(t);
        }
    }
}

/**
 * @brief holdHistoryF
 * @param s target state-object
 */
void holdHistoryF(CicoStateCore* s)
{
    if (s) {
        s->holdHistory();
    }
}

/**
 * @brief CicoStateCore::getCurrentState
 * @param states store active state-object data area
 * @param policy get policy
 * @return true:get success false:get fail
 */
bool CicoStateCore::getCurrentState(std::vector<const CicoState*>& states,
                                    E_GetPolicy policy)
{

    bool bR = false;

    if (ELvlTop == policy) {            // 
        int sz = m_childs.size();
        for (int i = 0; i < sz; i++) {
            CicoStateCore* t = m_childs[i];
            if (true == t->isActive()) { // active ?
                bR = true;
                states.push_back((CicoState*)t);
            }
        }
    }
    else {
        std::vector<CicoState*> stt;
        std::vector<E_ActiveImpact> impact;
        if (true == getActiveSonarChild(stt, impact)) {
            if (ELvlAll == policy) {    //
                int sz = impact.size(); //
                for (int i = 0; i < sz; i++) {
                    states.push_back(stt[i]);
                }
                bR = true;
            }
            else {
                int sz = impact.size(); //
                for (int i = 0; i < sz; i++) {
                    if (Single == impact[i]) {
                        bR = true;
                        states.push_back(stt[i]);
                    }
                }
            }
        }
    }
    return bR;
}

/**
 * @brief CicoStateCore::getCurrentState
 * @return not 0:active state-object 0:not have active state-object
 */
const CicoState* CicoStateCore::getCurrentState()
{
    CicoState* r = (CicoState*)0;
    int sz = m_childs.size();
    for (int i = 0; i < sz; i++) {      //
        CicoStateCore* t = m_childs[i];
        if (true == t->isActive()) {    // active ?
            r = (CicoState*)t;          //
            break;                      // break of for i
        }
    }
    return r;
}

/**
 * @brief CicoStateCore::isRegisteredInit
 * @return true:have start position  false:not have start position
 */
bool CicoStateCore::isRegisteredInit() const
{
    if (EPUnused == m_startPosition.suk) {
        return false;
    }
    return true;
}

/**
 * @brief CicoStateCore::getRegisteredInit
 * @param stt store start position objects
 * @param cnt store count
 */
void CicoStateCore::getRegisteredInit(std::vector<CicoStateCore*>& stt,
                                      int& cnt)
{
    // operate starting position
    if (EPDir == m_startPosition.suk) {
        stt.push_back(m_startPosition.u.d);
        cnt++;
    }
    else if (EPVec == m_startPosition.suk) {
        // Perform a plurality of start position
        int sz = m_startPosition.u.v->size();
        for (int i = 0; i < sz; i++) {
            stt.push_back((*m_startPosition.u.v)[i]);
        }
        cnt++;
    }
    int sz = m_childs.size();           //
    for (int i = 0; i < sz; i++) {      //
        m_childs[i]->getRegisteredInit(stt, cnt);
    }
}

/**
 * @brief CicoStateCore::stateEnd
 * @param ev trigger event
 */
void CicoStateCore::stateEnd(const CicoEvent& ev)
{
    int sz = m_childs.size();
    bool b = false;
    for (int i = 0; i < sz; i++) {
        if (true == (m_childs[i])->isActive()) {
            b = true;
            break;                      // break of for i
        }
    }
    if (true == b) {
        return;
    }
    if ((CicoStateCore*)0 != m_stateTermination) {
        CicoEventInfo ei("DetectTheEnd", CicoEvent::DetectTheEnd);
        onTransition(ev, this, m_stateTermination);
        return;
    }
    stop(ev);
    stateEndF(m_parent, ev);
}

/**
 * @brief stateEndF
 * @param stt target objects
 * @param ev trigger event
 */
void stateEndF(CicoStateCore* stt, const CicoEvent& ev)
{
    if (stt) {
        stt->stateEnd(ev);
    }
}

/**
 * @brief CicoStateCore::onTransition
 * @param ev trigger event
 * @param s Transition source
 * @param d Transition destination
 */
void CicoStateCore::onTransition(const CicoEvent& ev, CicoStateCore* s, CicoStateCore* d)
{
    std::vector<CicoStateCore*> stpstt;
    stpstt.push_back(s);

    std::vector<CicoStateCore*> rts;
    getRoots(s, rts);
    int szs = rts.size();
    if (0!=szs) {
        std::vector<CicoStateCore*> rtd;
        getRoots(d, rtd);
        int szd = rtd.size();
        for (int i = 0; i < szs; i++) {
            for (int j = 0; j < szd; j++) {
                if (rts[i] == rtd[j]) {
                    rts[i] = (CicoStateCore*)0;
                    break;              // break of for j
                }
            }
            if ((CicoStateCore*)0==rts[i]) {
                break;                  // break of for i
            }
        }
        for (int i = 0; i < szs; i++) {
            if ((CicoStateCore*)0==rts[i]) {
                break;                  // break of for i
            }
            stpstt.push_back(rts[i]);
        }
    }
    int sz = stpstt.size();
    // store history
    for (int i = 0; i < sz; i++) {
        holdHistoryF(stpstt[i]);
    }
    // stop Transition source
    for (int i = 0; i < sz; i++) {
        (stpstt[i])->stop(ev);
    }
    // start Transition destination
    startF(d, ev, true);
    {
        std::string sl;
        getLogPartF(s, sl);
        std::string el;
        getLogPartF(ev, el);
        std::string dl;
        getLogPartF(d, dl);
        _INFO("TRANSITION:%s-%s->%s", sl.c_str(), el.c_str(), dl.c_str());
    }
}

/**
 * @brief CicoStateCore::getRoots
 * @param stt base object
 * @param roots store roots object
 */
void CicoStateCore::getRoots(const CicoStateCore* stt,
                             std::vector<CicoStateCore*>& roots)
{
    const CicoStateCore* p = stt->getParent();
    while ((CicoStateCore*)0 != p) {
        if (true == p->isStateMachine()) { // stateMachine class Object
            break;                      // breack of while
        }
        roots.push_back((CicoStateCore*)p); // set parent
        p = p->getParent();             // get parent
    }
}

/**
 * @brief CicoStateCore::onDoExec
 * @param ev triger event
 * @return true:execute false:not execute
 */
bool CicoStateCore::onDoExec(const CicoEvent& ev)
{
    bool bR = false;
    if (EPUnused != m_do.suk) {
        onDo(ev);
        bR = true;
        {
            std::string sl;
            getLogPartF(this, sl);
            std::string el;
            getLogPartF(ev, el);
            //_INFO("%s.onDoExec(%s).onDo", sl.c_str(), el.c_str());
            _INFO("TRANSITION:%s-%s(onDo)", sl.c_str(), el.c_str());
        }
    }
    else if (EPUnused != m_entry.suk) {
        onEntry(ev);
        bR = true;
        {
            std::string sl;
            getLogPartF(this, sl);
            std::string el;
            getLogPartF(ev, el);
           // _INFO("%s.onDoExec(%s).onEntry", sl.c_str(), el.c_str());
            _INFO("TRANSITION:%s-%s(onEntry)", sl.c_str(), el.c_str(), sl.c_str());
        }
    }
    else {
        bR = false;
        {
            std::string sl;
            getLogPartF(this, sl);
            std::string el;
            getLogPartF(ev, el);
            _INFO("TRANSITION:%s-%s->%s(no tran.)", sl.c_str(), el.c_str(), sl.c_str());
        }
    }
    return bR;
}

/**
 * @brief onDoExecF
 * @param s target objects
 * @param ev trigger event
 * @return true:execute false:not execute
 */
bool onDoExecF(CicoStateCore* s, const CicoEvent& ev)
{
    if (s) {
        return s->onDoExec(ev);
    }
    return false;
}

/**
 * @brief CicoStateCore::getState
 * @param value Identification number
 * @return not 0:machi Identification number object
 *@        0:not find
 */
const CicoStateCore* CicoStateCore::getState(const int value) const
{
    if (m_value == value) {
        return this;
    }
    int sz = m_childs.size();
    if (0 != sz) {
        for (int i = 0; i < sz; i++) {
            const CicoStateCore* r;
            r = getStateF(m_childs[i], value);
            if ((CicoStateCore*)0 != r) {
                return r;
            }
        }
    }
    return (CicoStateCore*)0;
}

/**
 * @brief CicoStateCore::getObjects
 * @param obj store object area
 */
void CicoStateCore::getObjects(std::vector<CicoStateCore*>& obj)
{
    obj.push_back(this);                // store my state-object pointer
    int sz = m_childs.size();
    for (int i=0; i<sz; i++) {
        m_childs[i]->getObjects(obj);   // store childs state-object pointer
    }
}

/**
 * @brief CicoStateCore::getEventInfo
 * @param name event name
 * @param ev event number
 * @return not 0:machi event object  0:not find
 */
const CicoEventInfo* CicoStateCore::getEventInfo(const std::string& name,
                                                 unsigned short ev) const
{
    const CicoEventInfo tei(name, ev);
    const CicoEventInfo* r = (CicoEventInfo*)0;
    if (EPDir == m_eventInfo.suk) {
        if (tei == *m_eventInfo.u.d) {
            r = m_eventInfo.u.d;
        }
    }
    else if (EPVec == m_eventInfo.suk) {
        int sz = m_eventInfo.u.v->size();
        for (int i =0; i < sz; i++) {
            const CicoEventInfo* t = (*m_eventInfo.u.v)[i];
            if (tei == *t) {
                r = t;
                break;                  // break of for i<sz
            }
        }
    }
    return (CicoEventInfo*)r;           // not find
}

/**
 * @brief getStateF
 * @param s search object top
 * @param value find Identification number
 * @return not 0:match object 0:not find
 */
const CicoStateCore* getStateF(const CicoStateCore* s, const int value)
{
    if (s) {
        return s->CicoStateCore::getState(value);
    }
    return (CicoStateCore*)0;
}

/**
 * @brief CicoStateCore::getState
 * @param name find Identification name
 * @return not 0:match object 0:not find
 */
const CicoStateCore* CicoStateCore::getState(const std::string& name) const
{
    if (m_name == name) {
        return this;
    }
    int sz = m_childs.size();
    if (0 != sz) {
        for (int i = 0; i < sz; i++) {
            const CicoStateCore* r;
            r = getStateF(m_childs[i], name);
            if ((CicoStateCore*)0 != r) {
                return r;
            }
        }
    }
    return (CicoStateCore*)0;
}

/**
 * @brief getStateF
 * @param s search object top
 * @param name find Identification name
 * @return not 0:match object 0:not find
 */
const CicoStateCore* getStateF(const CicoStateCore* s, const std::string& name)
{
    if (s) {
        return s->CicoStateCore::getState(name);
    }
    return (CicoStateCore*)0;
}

/**
 * @brief startF
 * @param s start target object
 * @param e trigger event data
 * @param parent Operation in the direction of the parent
 * @return true:Activation success false:Activation fail
 */
bool startF(CicoStateCore* s, const CicoEvent& e, bool parent)
{
    bool bR = false;
    if (s) {
        bR = s->start(e, parent);
    }
    return bR;
}

/**
 * @brief getLogPartF
 * @param s log target
 * @param l store message area
 */
void getLogPartF(const CicoStateCore* s, std::string& l)
{
    std::stringstream stm;
    switch (s->m_eType) {               // type my class
    case CicoStateCore::EStateMachine:
        stm << "SM:{";                  // SM:{"name":999} image edit
        break;
    case CicoStateCore::EState:
        stm << "S:{";                   // S:{"name":999} image edit
        break;
    case CicoStateCore::EHistoryState:
        stm << "HS:{";                  // HS:{"name":999} image edit
        break;
    case CicoStateCore::EFinalState:
        stm << "FS:{";                  // FS:{"name":999} image edit
        break;
    default:
        stm << "??:{";                  // ??:{"name":999} image edit
        break;
    }
    if (!s->getName().empty()) {
        stm << "\"" << s->getName() << "\"";
    }
    stm << ":" << s->getValue() << "}";
    l = stm.str();
}

/**
 * @brief CicoStateCore::initTransition
 */
void CicoStateCore::initTransition()
{
    m_eventInfo.suk = EPUnused;         // unused
    // pointer clear
    m_eventInfo.u.d = (CicoEventInfo*)0;
    m_eventInfo.u.v = (std::vector<CicoEventInfo*>*)0;
}

/**
 * @brief CicoStateCore::initStartPosition
 */
void CicoStateCore::initStartPosition()
{
    m_startPosition.suk = EPUnused;     // unused
    // pointer clear
    m_startPosition.u.d = (CicoStateCore*)0;
    m_startPosition.u.v = (std::vector<CicoStateCore*>*)0;
}

/**
 * @brief CicoStateCore::initSttAction
 */
void CicoStateCore::initSttAction()
{
    // entry action
    m_entry.suk      = EPUnused;        // unused
    // entry action pointer clear
    m_entry.u.d.dcsa = (CicoStateAction*)0;
    m_entry.u.d.dav  = 0;
    m_entry.u.v.vcsa = (std::vector<CicoStateAction*>*)0;
    m_entry.u.v.vav  = (std::vector<int>*)0;

    // exit action
    m_exit.suk       = EPUnused;        // unused
    // exit action pointer clear
    m_exit.u.d.dcsa  = (CicoStateAction*)0;
    m_exit.u.d.dav   = 0;
    m_exit.u.v.vcsa  = (std::vector<CicoStateAction*>*)0;
    m_exit.u.v.vav   = (std::vector<int>*)0;

    // do action
    m_do.suk         = EPUnused;        // unused
    // do action pointer clear
    m_do.u.d.dcsa    = (CicoStateAction*)0;
    m_do.u.d.dav     = 0;
    m_do.u.v.vcsa    = (std::vector<CicoStateAction*>*)0;
    m_do.u.v.vav     = (std::vector<int>*)0;
}

/**
 * @brief CicoStateCore::clearTransition
 */
void CicoStateCore::clearTransition()
{
    if (EPVec == m_eventInfo.suk) {
        if ((std::vector<CicoEventInfo*>*)0 != m_eventInfo.u.v) {
            int sz = m_eventInfo.u.v->size();
            for (int i = 0; i < sz; i++) {
                delete (*m_eventInfo.u.v)[i];
            }
            m_eventInfo.u.v->clear();   // vector entry data clear
            delete m_eventInfo.u.v;
            m_eventInfo.u.v = (std::vector<CicoEventInfo*>*)0;
        }
    }
    else if (EPDir == m_eventInfo.suk) {
        delete m_eventInfo.u.d;
        m_eventInfo.u.d = (CicoEventInfo*)0;
    }
    m_eventInfo.suk = EPUnused;         // unused
}

/**
 * @brief CicoStateCore::clearStartPosition
 */
void CicoStateCore::clearStartPosition()
{
    if (EPVec == m_startPosition.suk) {
        if ((std::vector<CicoStateCore*>*)0 != m_startPosition.u.v) {
            m_startPosition.u.v->clear();
            delete m_startPosition.u.v;
            m_startPosition.u.v = (std::vector<CicoStateCore*>*)0;
        }
    }
    else if (EPDir == m_startPosition.suk) {
        m_startPosition.u.d = (CicoStateCore*)0;
    }
    m_startPosition.suk = EPUnused;     // unused
}

/**
 * @brief CicoStateCore::clearSttAction
 */
void CicoStateCore::clearSttAction()
{
    // entry action
    if (EPVec == m_entry.suk) {
        if ((std::vector<CicoStateAction*>*)0 != m_entry.u.v.vcsa) {
            m_entry.u.v.vcsa->clear();
            delete m_entry.u.v.vcsa;
            m_entry.u.v.vcsa = (std::vector<CicoStateAction*>*)0;
        }
        if ((std::vector<int>*)0 != m_entry.u.v.vav) {
            m_entry.u.v.vav->clear();
            delete m_entry.u.v.vav;
            m_entry.u.v.vav  = (std::vector<int>*)0;
        }
    }
    else if (EPDir == m_entry.suk) {
        m_entry.u.d.dcsa = (CicoStateAction*)0;
        m_entry.u.d.dav  = 0;
    }
    m_entry.suk      = EPUnused;        // unused

    // exit action
    if (EPVec == m_exit.suk) {
        if ((std::vector<CicoStateAction*>*)0 != m_exit.u.v.vcsa) {
            m_exit.u.v.vcsa->clear();
            delete m_exit.u.v.vcsa;
            m_exit.u.v.vcsa = (std::vector<CicoStateAction*>*)0;
        }
        if ((std::vector<int>*)0 != m_exit.u.v.vav) {
            m_exit.u.v.vav->clear();
            delete m_exit.u.v.vav;
            m_exit.u.v.vav  = (std::vector<int>*)0;
        }
    }
    else if (EPDir == m_exit.suk) {
        m_exit.u.d.dcsa = (CicoStateAction*)0;
        m_exit.u.d.dav  = 0;
    }
    m_exit.suk       = EPUnused;        // unused

    // do action
    if (EPVec == m_do.suk) {
        if ((std::vector<CicoStateAction*>*)0 != m_do.u.v.vcsa) {
            m_do.u.v.vcsa->clear();
            delete m_do.u.v.vcsa;
            m_do.u.v.vcsa = (std::vector<CicoStateAction*>*)0;
        }
        if ((std::vector<int>*)0 != m_do.u.v.vav) {
            m_do.u.v.vav->clear();
            delete m_do.u.v.vav;
            m_do.u.v.vav  = (std::vector<int>*)0;
        }
    }
    else if (EPDir == m_do.suk) {
        m_do.u.d.dcsa = (CicoStateAction*)0;
        m_do.u.d.dav  = 0;
    }
    m_do.suk         = EPUnused;        // unused
}
