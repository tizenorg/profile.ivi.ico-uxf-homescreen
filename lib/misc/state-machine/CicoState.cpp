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
#include <string>
#include <vector>
#include "CicoStateCore.h"
#include "CicoState.h"

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
 * @brief CicoState::CicoState
 * @param parent parent object
 */
CicoState::CicoState(CicoState* parent):
    CicoStateCore(EState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param name Identification name
 * @param parent parent object
 */
CicoState::CicoState(const std::string& name, CicoState* parent):
    CicoStateCore(EState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param value Identification number
 * @param parent parent object
 */
CicoState::CicoState(int value, CicoState* parent):
    CicoStateCore(EState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param name Identification name
 * @param value Identification number
 * @param parent parent object
 */
CicoState::CicoState(const std::string& name, int value, CicoState* parent):
    CicoStateCore(EState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param parent parent object
 */
CicoState::CicoState(CicoStateMachine* parent):
    CicoStateCore(EState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param name Identification name
 * @param parent parent object
 */
CicoState::CicoState(const std::string& name, CicoStateMachine* parent):
    CicoStateCore(EState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param value Identification number
 * @param parent parent object
 */
CicoState::CicoState(int value, CicoStateMachine* parent):
    CicoStateCore(EState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::CicoState
 * @param name Identification name
 * @param value Identification number
 * @param parentparent object
 */
CicoState::CicoState(const std::string& name, int value,
                     CicoStateMachine* parent):
    CicoStateCore(EState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoState::~CicoState
 */
CicoState::~CicoState()
{
}

/**
 * @brief CicoState::addState
 * @param state child object
 */
void CicoState::addState(CicoState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoState::addState
 * @param state child object
 */
void CicoState::addState(CicoFinalState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoState::addState
 * @param state child object
 */
void CicoState::addState(CicoHistoryState* state)
{
    CicoStateCore::addState((CicoStateCore*)state);
}

/**
 * @brief CicoState::addTransition
 * @param evInf transition trigger data
 * @param state Transition destination
 */
void CicoState::addTransition(const CicoEventInfo& evInf, CicoState* state)
{
    CicoStateCore::addTransition(evInf, (CicoStateCore*)state);
}

/**
 * @brief CicoState::addTransition
 * @param evInf transition trigger data
 * @param state Transition destination
 */
void CicoState::addTransition(const CicoEventInfo& evInf, CicoFinalState* state)
{
    CicoStateCore::addTransition(evInf, (CicoStateCore*)state);
}

/**
 * @brief CicoState::addTransition
 * @param evInf transition trigger data
 * @param state Transition destination
 */
void CicoState::addTransition(const CicoEventInfo& evInf, CicoHistoryState* state)
{
    CicoStateCore::addTransition(evInf, (CicoStateCore*)state);
}

/**
 * @brief CicoState::setFinishTransition
 * @param state Transition destination
 */
void CicoState::setFinishTransition(CicoState* state)
{
    CicoStateCore::setFinishTransition((CicoStateCore*)state);
}

/**
 * @brief CicoState::setFinishTransition
 * @param state
 * @param state Transition destination
 */
void CicoState::setFinishTransition(CicoFinalState* state)
{
    CicoStateCore::setFinishTransition((CicoStateCore*)state);
}

/**
 * @brief CicoState::setFinishTransition
 * @param state Transition destination
 */
void CicoState::setFinishTransition(CicoHistoryState* state)
{
    CicoStateCore::setFinishTransition((CicoStateCore*)state);
}

/**
 * @brief CicoState::addInitState
 * @param state start position
 */
void CicoState::addInitState(CicoState* state)
{
    CicoStateCore::addInitState((CicoStateCore*)state);
}

/**
 * @brief CicoState::addInitState
 * @param state start position
 */
void CicoState::addInitState(CicoHistoryState* state)
{
    CicoStateCore::addInitState((CicoStateCore*)state);
}
