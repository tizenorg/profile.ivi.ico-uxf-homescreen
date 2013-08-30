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
#ifndef CICOSTATEMACHINE_H
#define CICOSTATEMACHINE_H

#include <iostream>
#include <string>
#include <vector>

#include "CicoStateCore.h"

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOFINALSTATE_H
class CicoFinalState;
#endif

#ifndef CICOHISTORYSTATE_H
class CicoHistoryState;
#endif


/**
 * state-machine object class definition
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief The CicoStateMachine class
 *   state-machine object class definition
 */
class CicoStateMachine : public CicoStateCore
{
public:
    // Constructior
                CicoStateMachine(int value = 0);
                CicoStateMachine(const std::string& name, int value = 0);

    // registration state/historyState/fainalState objects
    void        addState(CicoState* state);
    void        addState(CicoFinalState* state);
    void        addState(CicoHistoryState* state);

    // registration start position
    void        addInitState(CicoState* state);
    void        addInitState(CicoHistoryState* state);

    // start state-machine
    bool        start();

    // Delivery of the event
    bool        eventEntry(const CicoEvent& ev);
    // Test event delivery
    bool        eventTest(const CicoEvent& ev);

    // stop state-machine
    bool        stop();

    // Acquisition of state-objects is registered
    const CicoStateCore* getState(const int value) const;
    const CicoStateCore* getState(const std::string& name) const;

    // get current active state-objects
    bool        getCurrentState(std::vector<const CicoState*>& states,
                                CicoStateCore::E_GetPolicy policy=CicoStateCore::ELvlBttm);
    const CicoState* getCurrentState();

protected:
private:
    bool        m_eventExec;            // During event processing flag
};

/**
 * @brief CicoStateMachine::addState
 * @param state register child state-object pointer
 */
inline void CicoStateMachine::addState(CicoState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoStateMachine::addState
 * @param state register child state-object pointer
 */
inline void CicoStateMachine::addState(CicoFinalState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoStateMachine::addState
 * @param state register child state-object pointer
 */
inline void CicoStateMachine::addState(CicoHistoryState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoStateMachine::addInitState
 * @param state
 * @param state state-object pointer is tart position
 */
inline void CicoStateMachine::addInitState(CicoState* state)
{
    CicoStateCore::addInitState((CicoStateCore*)state);
}

/**
 * @brief CicoStateMachine::addInitState
 * @param state state-object pointer is tart positions
 */
inline void CicoStateMachine::addInitState(CicoHistoryState* state)
{
    CicoStateCore::addInitState((CicoStateCore*)state);
}

/**
 * @brief CicoStateMachine::getCurrentState
 * @param states active state-object pointers store area
 * @param policy get policy
 * @return true:get success / false get fail
 */
inline bool CicoStateMachine::getCurrentState(std::vector<const CicoState*>& states,
                                              CicoStateCore::E_GetPolicy policy)
{
    return CicoStateCore::getCurrentState(states, policy);
}

/**
 * @brief CicoStateMachine::getCurrentState
 * @      get active state-object
 * @return active state-object pointer
 */
inline const CicoState* CicoStateMachine::getCurrentState()
{
    return CicoStateCore::getCurrentState();
}

/**
 * @brief CicoStateMachine::getState
 * @param value state-object identification number
 * @return not 0:state-object pointer/0 get fail
 */
inline const CicoStateCore* CicoStateMachine::getState(const int value) const
{
    return CicoStateCore::getState(value);
}

/**
 * @brief CicoStateMachine::getState
 * @param name state-object identification name
 * @return not 0:state-object pointer/0 get fail
 */
inline const CicoStateCore* CicoStateMachine::getState(const std::string& name) const
{
    return CicoStateCore::getState(name);
}

#endif // CICOSTATEMACHINE_H
