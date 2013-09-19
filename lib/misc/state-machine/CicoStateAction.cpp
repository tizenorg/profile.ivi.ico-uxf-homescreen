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

#include "CicoStateAction.h"

/**
 * Base class for action execution of entry / exit / do
 * @author m.kamoshida
 * @version 0.1
 */
/**
 * @brief CicoStateAction::CicoStateAction
 */
CicoStateAction::CicoStateAction()
{
}

/**
 * @brief CicoStateAction::onEntry
 * @param const CicoEvent& trigger event data
 * @param const CicoState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onEntry(const CicoEvent&, const CicoState*, int)
{
}

/**
 * @brief CicoStateAction::onEntry
 * @param const CicoEvent& trigger event data
 * @param const CicoFinalState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onEntry(const CicoEvent&, const CicoFinalState*, int)
{
}

/**
 * @brief CicoStateAction::onEntry
 * @param const CicoEvent& trigger event data
 * @param const CicoHistoryState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onEntry(const CicoEvent&, const CicoHistoryState*, int)
{
}


/**
 * @brief CicoStateAction::onExit
 * @param const CicoEvent& trigger event data
 * @param const CicoState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onExit(const CicoEvent&, const CicoState*, int)
{
}

/**
 * @brief CicoStateAction::onExit
 * @param const CicoEvent& trigger event data
 * @param const CicoHistoryState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onExit(const CicoEvent&, const CicoHistoryState*, int)
{
}


/**
 * @brief CicoStateAction::onDo
 * @param const CicoEvent& trigger event data
 * @param const CicoState* Object of the caller
 * @param int Added value at the time of registration action
 */
void CicoStateAction::onDo(const CicoEvent&, const CicoState*, int)
{
}
