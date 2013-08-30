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
#include "CicoHistoryState.h"

/**
 * history state object class
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(CicoState* parent):
    CicoStateCore(EHistoryState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param name Identification name
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(const std::string& name, CicoState* parent):
    CicoStateCore(EHistoryState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param value Identification number
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(int value, CicoState* parent):
    CicoStateCore(EHistoryState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param name Identification name
 * @param value Identification number
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(const std::string& name, int value,
                                   CicoState* parent):
    CicoStateCore(EHistoryState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(CicoStateMachine* parent):
    CicoStateCore(EHistoryState, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param name Identification name
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(const std::string& name,
                                   CicoStateMachine* parent):
    CicoStateCore(EHistoryState, name, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param value Identification number
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(int value, CicoStateMachine* parent):
    CicoStateCore(EHistoryState, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::CicoHistoryState
 * @param name Identification name
 * @param value Identification number
 * @param parent parent object
 */
CicoHistoryState::CicoHistoryState(const std::string& name, int value,
                                   CicoStateMachine* parent):
    CicoStateCore(EHistoryState, name, value, (CicoStateCore*)parent)
{
}

/**
 * @brief CicoHistoryState::~CicoHistoryState
 */
CicoHistoryState::~CicoHistoryState()
{
    m_vDState.clear();
    m_vHState.clear();
}

/**
 * @brief CicoHistoryState::addDefaultState
 * @param stts default state object
 */
void CicoHistoryState::addDefaultState(std::vector<CicoState*> stts)
{
    int sz = stts.size();
    for (int i = 0; i < sz; i++) {
        m_vDState.push_back(stts[i]);
    }
}

/**
 * @brief CicoHistoryState::addDefaultState
 * @param stts default state object
 */
void CicoHistoryState::addDefaultState(CicoState* stt)
{
        m_vDState.push_back(stt);
}

/**
 * @brief CicoHistoryState::holdHistory
 */
void CicoHistoryState::holdHistory()
{
    bool bR = false;
    if ((CicoStateCore*)0 != m_parent) { // exist is parent
        // Discard the previous history
        m_vHState.clear();
        std::vector<const CicoState*> t;
        // Stores the new history
        if (Deep == m_historyType) {    // deep history ?
            /*
            * Deep history
            */
            bR = getCurrentStateF(m_parent, t, CicoStateCore::ELvlBttm);
        }
        else {
            /*
            * Shallow history
            */
            bR = getCurrentStateF(m_parent, t, CicoStateCore::ELvlTop);
        }
        if (false == bR) {
            /*
            * _D_ERROR
            */
        }
        else {
            int sz = t.size();
            for (int i = 0; i < sz; i++) {
                m_vHState.push_back((CicoState*)t[i]);
            }
        }
    }
    else {
        /*
        * _D_ERROR
        */
    }
}

/**
 * @brief CicoHistoryState::start
 * @param ev trigger event data
 * @return
 */
bool CicoHistoryState::start(const CicoEvent& ev, bool)
{
    toActivate(ev);

    toDeactivate(ev);

    int szH = m_vHState.size();
    int szD = m_vDState.size();
    if (0 != szH) {
        for (int i = 0; i < szH; i++) {
            CicoStateCore* t = (CicoStateCore*)m_vHState[i];
            startF(t, ev, false);
        }
    }
    else if (0 != szD) {
        for (int i = 0; i < szD; i++) {
            CicoStateCore* t =(CicoStateCore*)m_vDState[i];
            startF(t, ev, true);
        }
    }
    else {
        /*
        * _D_ERROR
        */
        startF(m_parent, ev, true);
    }
    return true;
}

/**
 * @brief getCurrentStateF
 * @param stt get top 
 * @param states store active state objects area
 * @param policy get poicy
 * @return true:get success false:get fail
 */
bool getCurrentStateF(CicoStateCore* stt,
                      std::vector<const CicoState*>& states,
                      CicoStateCore::E_GetPolicy policy)
{
    return stt->getCurrentState(states, policy);
}

/**
 * @brief CicoHistoryState::setParent
 * @param parent parent object
 */
void CicoHistoryState::setParent(CicoState* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}

/**
 * @brief CicoHistoryState::setParent
 * @param parent parent object
 */
void CicoHistoryState::setParent(CicoStateMachine* parent)
{
    CicoStateCore::setParent((CicoStateCore*)parent);
}
