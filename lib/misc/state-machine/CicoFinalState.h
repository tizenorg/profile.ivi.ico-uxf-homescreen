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
#ifndef CICOFINALSTATE_H
#define CICOFINALSTATE_H

#include <iostream>
#include <string>

#include "CicoStateCore.h"


#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

#ifndef CICOSTATE_H
class CicoState;
#endif


/**
 * final state object class
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * @brief The CicoFinalState class
 */
class CicoFinalState :public CicoStateCore
{
public:
    // Constructior
                CicoFinalState(CicoState* parent=0);
                CicoFinalState(const std::string& name, CicoState* parent=0);
                CicoFinalState(int value, CicoState* parent=0);
                CicoFinalState(const std::string& name, int value,
                               CicoState* parent=0);
                CicoFinalState(CicoStateMachine* parent);
                CicoFinalState(const std::string& name,
                               CicoStateMachine* parent);
                CicoFinalState(int value, CicoStateMachine* parent);
                CicoFinalState(const std::string& name, int value,
                               CicoStateMachine* parent);
    void        setParent(CicoState* parent);
    void        setParent(CicoStateMachine* parent);

    void        addEntryAction(CicoStateAction* action, int addval = 0);

protected:
    virtual bool start(const CicoEvent& ev, bool parent=false);

private:

};

/**
 * @brief CicoFinalState::addEntryAction
 * @param action register callback action class
 * @param addval additional value
 */
inline void CicoFinalState::addEntryAction(CicoStateAction* action, int addval)
{
    CicoStateCore::addEntryAction(action, addval);
}

#endif // CICOFINALSTATE_H
