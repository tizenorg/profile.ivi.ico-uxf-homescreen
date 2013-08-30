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

#ifndef CICOHISTORYSTATE_H
#define CICOHISTORYSTATE_H

#include <iostream>
#include <string>
#include <vector>
#include "CicoStateCore.h"

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

/**
 * history state object class
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief The CicoHistoryState class
 */
class  CicoHistoryState : public CicoStateCore
{
public:
    // history type
    enum E_HType {
        Shallow = 1,                    // Shallow History
        Deep                            // Deep History
    };
    // constructor
                CicoHistoryState(CicoState* parent=0);
                CicoHistoryState(const std::string& name, CicoState* parent=0);
                CicoHistoryState(int value, CicoState* parent=0);
                CicoHistoryState(const std::string& name, int value,
                                 CicoState* parent=0);
                CicoHistoryState(CicoStateMachine* parent);
                CicoHistoryState(const std::string& name,
                                 CicoStateMachine* parent);
                CicoHistoryState(int value, CicoStateMachine* parent);
                CicoHistoryState(const std::string& name, int value,
                                 CicoStateMachine* parent);

    // destructor
                ~CicoHistoryState();

    void        addDefaultState(std::vector<CicoState*> stts);
    void        addDefaultState(CicoState* stt);

    E_HType     getHistoryType() const;
    void        setHistoryType(E_HType type);

    void        setParent(CicoState* parent);
    void        setParent(CicoStateMachine* parent);

    void        addEntryAction(CicoStateAction* action, int addval = 0);

    void        addExitAction(CicoStateAction* action, int addval = 0);

protected:
    virtual bool start(const CicoEvent& ev, bool parent=false);

    virtual void holdHistory();

private:
    E_HType     m_historyType;          // history type
    std::vector<CicoState*> m_vDState;  // store default area
    std::vector<CicoState*> m_vHState;  // store history area
};

/**
 * @brief CicoHistoryState::getHistoryType
 * @return history type(Shallow or Deep)
 */
inline CicoHistoryState::E_HType CicoHistoryState::getHistoryType() const
{
    return m_historyType;
}

/**
 * @brief CicoHistoryState::setHistoryType
 * @      set history type
 * @param type history type(Shallow or Deep)
 */
inline void CicoHistoryState::setHistoryType(E_HType type)
{
    m_historyType = type;
}

/**
 * @brief CicoHistoryState::addEntryAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoHistoryState::addEntryAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addEntryAction(action, addval);
}

/**
 * @brief CicoHistoryState::addExitAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoHistoryState::addExitAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addExitAction(action, addval);
}

#endif // CICOHISTORYSTATE_H
