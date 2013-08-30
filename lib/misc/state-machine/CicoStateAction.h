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
#ifndef CICOSTATEACTION_H
#define CICOSTATEACTION_H

#include "CicoEvent.h"

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOHISTORYSTATE_H
class CicoHistoryState;
#endif

#ifndef CICOFINALSTATE_H
class CicoFinalState;
#endif

/**
 * Base class for action execution of entry / exit / do
 * @author m.kamoshida
 * @version 0.1
 */
/**
 * @brief The CicoStateAction class
 */
class CicoStateAction
{
public:
    // Constructior
                CicoStateAction();

    virtual void onEnttry(const CicoEvent& ev, const CicoState* stt,
                          int addval);
    virtual void onEnttry(const CicoEvent& ev, const CicoFinalState* stt,
                          int addval);
    virtual void onEnttry(const CicoEvent& ev, const CicoHistoryState* stt,
                          int addval);

    virtual void onExit(const CicoEvent& ev, const CicoState* stt,
                        int addval);
    virtual void onExit(const CicoEvent& ev, const CicoHistoryState* stt,
                        int addval);

    virtual void onDo(const CicoEvent& ev, const CicoState* stt,
                      int addval);

private:
};

#endif // CICOSTATEACTION_H
