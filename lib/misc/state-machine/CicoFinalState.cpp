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
#include "CicoFinalState.h"


/**
 * final state object class
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * @brief CicoFinalState::CicoFinalState
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(CicoState* parent):
    CicoStateCore(EFinalState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param name Identification name
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(const std::string& name, CicoState* parent):
    CicoStateCore(EFinalState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param value Identification number
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(int value, CicoState* parent):
    CicoStateCore(EFinalState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param name Identification name
 * @param value Identification number
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(const std::string& name, int value,
                               CicoState* parent):
    CicoStateCore(EFinalState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(CicoStateMachine* parent):
    CicoStateCore(EFinalState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param name Identification name
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(const std::string& name,
                               CicoStateMachine* parent):
    CicoStateCore(EFinalState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param value Identification number
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(int value, CicoStateMachine* parent):
    CicoStateCore(EFinalState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::CicoFinalState
 * @param name Identification name
 * @param value Identification number
 * @param parent parent object
 */
CicoFinalState::CicoFinalState(const std::string& name, int value,
                               CicoStateMachine* parent):
    CicoStateCore(EFinalState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoFinalState::start
 * @param ev
 * @return
 */
bool CicoFinalState::start(const CicoEvent& ev, bool)
{
    m_activeState = true;               // activate
    onEntry(ev);                        // entry
    m_activeState = false;              // deactivate
    stateEndF(m_parent, ev);
    return true;
}

/**
 * @brief CicoFinalState::setParent
 * @param parent parent object
 */
void CicoFinalState::setParent(CicoState* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}

/**
 * @brief CicoFinalState::setParent
 * @param parent parent object
 */
void CicoFinalState::setParent(CicoStateMachine* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}
