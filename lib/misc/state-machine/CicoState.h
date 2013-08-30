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
#ifndef CICOSTATE_H
#define CICOSTATE_H

#include <iostream>
#include <string>
#include <vector>
#include "CicoStateCore.h"

#ifndef CICOFINALSTATE_H
class CicoFinalState;
#endif

#ifndef CICOHISTORYSTATE_H
class CicoHistoryState;
#endif

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

/**
 * State object class definition
 * I represent the state. The state identified by the name or number
 * Depending on the event information of the member variable, perform -
 * the determination of the transition, the transition
 * I used during the transition or identification number, name identification.
 * Please be sure to set either.
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief The CicoState class
 */
class CicoState : public CicoStateCore
{
public:
    // Constructior
                CicoState(CicoState* parent=0);
                CicoState(const std::string& name, CicoState* parent=0);
                CicoState(int value, CicoState* parent=0);
                CicoState(const std::string& name, int value,
                          CicoState* parent=0);
                CicoState(CicoStateMachine* parent);
                CicoState(const std::string& name, CicoStateMachine* parent);
                CicoState(int value, CicoStateMachine* parent);
                CicoState(const std::string& name, int value,
                          CicoStateMachine* parent);
    // destructor
    virtual     ~CicoState();

    // registration of child
    void        addState(CicoState* state);
    void        addState(CicoFinalState* state);
    void        addState(CicoHistoryState* state);

    // registration of event and transition
    void        addTransition(const CicoEventInfo& evInf, CicoState* state);
    void        addTransition(const CicoEventInfo& evInf,
                              CicoFinalState* state);
    void        addTransition(const CicoEventInfo& evInf,
                              CicoHistoryState* state);

    // registration of finish transition
    void        setFinishTransition(CicoState* state);
    void        setFinishTransition(CicoFinalState* state);
    void        setFinishTransition(CicoHistoryState* state);

    // registration of start position
    void        addInitState(CicoState* state);
    void        addInitState(CicoHistoryState* state);

    bool        getCurrentState(std::vector<const CicoState*>& states,
                                CicoStateCore::E_GetPolicy policy=CicoStateCore::ELvlBttm);
    const CicoState* getCurrentState();


    void        addEntryAction(CicoStateAction* action, int addval = 0);

    void        addExitAction(CicoStateAction* action, int addval = 0);

    void        addDoAction(CicoStateAction* action, int addval = 0);

    void        setParent(CicoState* parent);
    void        setParent(CicoStateMachine* parent);

protected:
private:

};

/**
 * @brief CicoState::addEntryAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoState::addEntryAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addEntryAction(action, addval);
}

/**
 * @brief CicoState::addExitAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoState::addExitAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addExitAction(action, addval);
}

/**
 * @brief CicoState::addDoAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoState::addDoAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addDoAction(action, addval);
}

/**
 * @brief CicoState::getCurrentState
 * @param states active state-object pointers store area
 * @param policy get policy
 * @return true:get success / false:get fail
 */
inline bool CicoState::getCurrentState(std::vector<const CicoState*>& states,
                                       CicoStateCore::E_GetPolicy policy)
{
    return CicoStateCore::getCurrentState(states, policy);

}

/**
 * @brief CicoState::getCurrentState
 * @return active state-object pointer
 */
inline const CicoState* CicoState::getCurrentState()
{
    return (CicoState*)CicoStateCore::getCurrentState();

}

/**
 * @brief CicoState::setParent
 * @     set parent(state-object or stateMachine-object)
 * @param parent state-object pointer
 */
inline void CicoState::setParent(CicoState* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}

/**
 * @brief CicoState::setParent
 * @      set parent(state object or stateMachine object)
 * @param parent
 */
inline void CicoState::setParent(CicoStateMachine* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}

#endif // CICOSTATE_H
