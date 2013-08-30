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


#ifndef CICOSTATEMACHINECREATOR_H
#define CICOSTATEMACHINECREATOR_H
#include <iostream>
#include <string>
#include <vector>
#include "CicoBlockParser.h"
#include "CicoEventInfo.h"

#ifndef CICOSTATECORE_H
class CicoStateCore;
#endif

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOFINALSTATE_H
class CicoFinalState;
#endif

#ifndef CICOHISTORYSTATE_H
class CicoHistoryState;
#endif

#define DKstateMachine      "stateMachine"
#define DKstate             "state"
#define DKfinalState        "finalState"
#define DKhistoryState      "historyState"
#define DKevent             "event"

#define DKname              "name"
#define DKname2             "n"
#define DKvalue             "value"
#define DKvalue2            "v"
#define DKinitial           "initial"
#define DKinitial2          "i"
#define DKfinish            "finish"
#define DKfinish2           "f"
#define DKdefault           "default"
#define DKdefault2          "d"
#define DKguardCondition    "guardCondition"
#define DKguardCondition2   "g"
#define DKoperator          "operator"
#define DKoperator2         "o"
#define DKtransition        "transition"
#define DKtransition2       "t"
#define DKtype              "type"
#define DKtype2             "ht"
#define DKvalued            "valued"
#define DKvalued2           "vd"
#define DKjoin              "join"
#define DKjoin2              "j"

#define DVShallow           "Shallow"
#define DVDeep              "Deep"
#define DVand               "and"
#define DVor                "or"


/**
 * StateMachine-Object creator
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief The CicoStateMachineCreator class
 * @      state-machine produce from the definition file or string
 */
class CicoStateMachineCreator
{
public:
    CicoStateMachine* createFile(const std::string& filename);

    CicoStateMachine* create(const char* sttdef);
    const std::string& getError();

protected:
    void        CreateStateObjects(CicoBlockParser& cbp,
                                  CicoStateCore* parent = 0);
    void        CreateStateMachine(CicoBlockParser& cbp);
    void        CreateState(CicoBlockParser& cbp, CicoStateCore* parent);
    void        CreateHistoryState(CicoBlockParser& cbp,
                                   CicoStateCore* parent);
    void        CreateFinalState(CicoBlockParser& cbp, CicoStateCore* parent);

    void        setConnect(CicoBlockParser& cbp, CicoStateMachine* csm,
                           CicoStateCore* parent=0);

    void        setConnectStateMachine(CicoBlockParser& cbp,
                                       CicoStateMachine* csm);
    void        setConnectState(CicoBlockParser& cbp, CicoStateMachine* csm);
    void        setConnectHistoryState(CicoBlockParser& cbp,
                                       CicoStateMachine* csm);
    void        setInitial(CicoBlockParser& cbp, const char* key,
                           CicoStateMachine* csm);
    void        addInitial(CicoStateCore* tgt, CicoStateCore* dat);

    void        setFinish(CicoBlockParser& cbp, const char* key,
                          CicoStateMachine* csm);

    void        setDefault(CicoBlockParser& cbp, CicoHistoryState* chs,
                           CicoStateMachine* csm);
    void        addDefault(CicoHistoryState* chs, CicoStateCore* csc);


    void        CreateEvent(CicoBlockParser& cbp, CicoStateMachine* csm,
                            CicoStateCore* parent);

    bool        regGuardCondition(picojson::value& v, CicoEventInfo& evi);
    void        setGuardCondition(CicoEventInfo& evi, picojson::object& o);

    bool        getJsonString(picojson::value& v, std::string& str);
    bool        getJsonNumber(picojson::value& v, int& num);
    bool        getJsonDouble(picojson::value& v, double& d);
    bool        parseName(picojson::object& o, std::string& nm);

    bool        parseValueI(picojson::object& o, int& v);
    bool        parseValueIS(picojson::object& o, int& n, std::string& s);

    bool        parseValuedD(picojson::object& o, double& v);

    bool        parseHistoryType(picojson::object& o,
                                 CicoHistoryState::E_HType& e);

    bool        parseStateObject(picojson::value& v,
                                 std::vector<CicoStateCore*>& vec,
                                 CicoStateMachine* csm, std::string& err);

    bool        parseOperator(picojson::object& o, std::string& op);
    bool        parseJoin(picojson::object& o, std::string& j);

private:
    std::string m_err;

};

/**
 * @brief CicoStateMachineCreator::getError
 * @      get error message
 * @return error message
 */
inline const std::string& CicoStateMachineCreator::getError()
{
    return m_err;
}

#endif // CICOSTATEMACHINECREATOR_H
