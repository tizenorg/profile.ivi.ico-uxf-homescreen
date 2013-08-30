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
#include <fstream>

#include "CicoBlockParser.h"
#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateCore.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoFinalState.h"
#include "CicoStateMachine.h"
#include "CicoStateMachineCreator.h"

#include "picojson.h"

/**
 * state-machine produce from the definition file or string
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief CicoStateMachineCreator::createFile
 * @      state-machine produce from the definition file
 * @param filename state-machine definition file
 * @return not 0: create stateMachine-object pointer
 * @       0:create fail
 */
CicoStateMachine* CicoStateMachineCreator::createFile(const std::string& filename)
{
    m_err.clear();
    char tmp[512];
    std::string readtmp("");
    std::ifstream ifs;
    ifs.open(filename.c_str());
    if (ifs) {
        while (!ifs.eof()) {
            ifs.getline(tmp, sizeof(tmp));
            readtmp += std::string(tmp);
        }
        ifs.close();
    }
    if (!readtmp.empty()) {
        return create(readtmp.c_str());
    }
    else {
        m_err = std::string("file fail");
    }
    return (CicoStateMachine*)0;
}

/**
 * @brief CicoStateMachineCreator::create
 * @      state-machine produce from the definition string
 * @param sttdef state-machine definition string
 * @return not 0: create stateMachine-object pointer
 * @       0:create fail
 */
CicoStateMachine* CicoStateMachineCreator::create(const char* sttdef)
{
    m_err.clear();
    CicoBlockParser cbp;
    if (false == cbp.getStateMachineParts(sttdef)) { // parse
        m_err = cbp.getError();
        return (CicoStateMachine*)0;
    }
    if (CicoBlockParser::STTMAC != cbp.m_kind) { // Failure to obtain staeMachine
        m_err = "Failure to obtain staeMachine";
        return (CicoStateMachine*)0;
    }
    CreateStateObjects(cbp);
    CicoStateMachine* csm = (CicoStateMachine*)cbp.getVoid();
    if (!m_err.empty()) {
        if ((CicoStateMachine*)0 != csm) {
            delete csm;
        }
        return (CicoStateMachine*)0;
    }
    setConnect(cbp, csm);
    if (!m_err.empty()) {
        if ((CicoStateMachine*)0 != csm) {
            delete csm;
        }
        return (CicoStateMachine*)0;
    }
    return csm;
}

/**
 * @brief CicoStateMachineCreator::CreateStateObjects
 * @param cbp state-object json definition parts
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::CreateStateObjects(CicoBlockParser& cbp, CicoStateCore* parent)
{
    if (CicoBlockParser::EV == cbp.m_kind) {
        return;
    }
    switch (cbp.m_kind) {
    case CicoBlockParser::STTMAC:           // stateMachine
        CreateStateMachine(cbp);
        break;
    case CicoBlockParser::STT:              // state
        CreateState(cbp, parent);
        break;
    case CicoBlockParser::HSTT:             // history state
        CreateHistoryState(cbp, parent);
        break;
    case CicoBlockParser::FSTT:             // final state
        CreateFinalState(cbp, parent);
        break;
    default:
        break;
    }
    if (!m_err.empty()) {
        return;
    }
    CicoStateCore* my = (CicoStateCore*)cbp.getVoid();
    if ((CicoStateCore*)0 == my) {
        m_err = "create error state object";
        return;
    }
    int sz = cbp.m_childs.size();
    for (int i=0; i<sz; i++) {
        CreateStateObjects(*cbp.m_childs[i], my);
        if (!m_err.empty()) {
            return;
        }
    }
}

/**
 * @brief CicoStateMachineCreator::CreateStateMachine
 * @param cbp definition stateMachine-object json parts
 */
void CicoStateMachineCreator::CreateStateMachine(CicoBlockParser& cbp)
{
    if (false == cbp.isJsonParse()) {
        if (false == cbp.jsonParse(DKstateMachine)) {
            m_err = cbp.getError();
            return;
        }
    }
    picojson::object o = cbp.m_v.get(DKstateMachine).get<picojson::object>();
    std::string name;
    int value = 0;
    bool bn = parseName(o, name);
    bool bv = parseValueI(o, value);
    if ((true == bn) || (true == bv)) {
        CicoStateMachine* csm = new CicoStateMachine(name, value);
        cbp.setVoid((void*)csm);
    }
    else {
        std::ostringstream stream;
        if (false == cbp.m_parts.empty()) {
            std::string h;
            cbp.getErrorHead(h, cbp.m_parts.c_str());
            stream << "stateMachine define error:" << h;
        }
        else {
            stream << "stateMachine unkown";
        }
        m_err = stream.str();
    }
}

/**
 * @brief CicoStateMachineCreator::CreateState
 * @param cbp definition state-object json parts
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::CreateState(CicoBlockParser& cbp,
                                          CicoStateCore* parent)
{
    if (false == cbp.isJsonParse()) {
        if (false == cbp.jsonParse(DKstate)) {
            m_err = cbp.getError();
            return;
        }
    }
    picojson::object o = cbp.m_v.get(DKstate).get<picojson::object>();
    std::string name;
    int value = 0;
    bool bn = parseName(o, name);
    bool bv = parseValueI(o, value);
    if ((true == bn) || (true == bv)) {
        CicoState* cs = (CicoState*)0;
        if (true == parent->isStateMachine()) {
            cs = new CicoState(name, value, (CicoStateMachine*)parent);
        }
        else if (true == parent->isState()) {
            cs = new CicoState(name, value, (CicoState*)parent);
        }

        if ((CicoState*)0 != cs) {
            cbp.setVoid((void*)cs);
        }
    }
    else {
        std::ostringstream stream;
        if (false == cbp.m_parts.empty()) {
            std::string h;
            cbp.getErrorHead(h, cbp.m_parts.c_str());
            stream << "state define error:" << h;
        }
        else {
            stream << "state unkown";
        }
        m_err = stream.str();
    }
}

/**
 * @brief CicoStateMachineCreator::CreateHistoryState
 * @param cbp definition historyState-object json parts
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::CreateHistoryState(CicoBlockParser& cbp,
                                                 CicoStateCore* parent)
{
    if (false == cbp.isJsonParse()) {
        if (false == cbp.jsonParse(DKhistoryState)) {
            m_err = cbp.getError();
            return;
        }
    }
    std::string h;
    if (false == cbp.m_parts.empty()) {
        cbp.getErrorHead(h, cbp.m_parts.c_str());
    }
    picojson::object o = cbp.m_v.get(DKhistoryState).get<picojson::object>();
    std::string name;
    int value = 0;
    bool bn = parseName(o, name);
    bool bv = parseValueI(o, value);
    CicoHistoryState::E_HType eht = CicoHistoryState::Shallow;
    if (false == parseHistoryType(o,  eht)) {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "history type define error:" << h;
        }
        else {
            stream << "history unkown";
        }
        m_err = stream.str();
        return;
    }
    if ((true == bn) || (true == bv)) {
        CicoHistoryState* chs = (CicoHistoryState*)0;
        if (true == parent->isStateMachine()) {
            chs = new CicoHistoryState(name, value, (CicoStateMachine*)parent);
        }
        else if (true == parent->isState()) {
            chs = new CicoHistoryState(name, value, (CicoState*)parent);
        }

        if ((CicoHistoryState*)0 != chs) {
            cbp.setVoid((void*)chs);
            chs->setHistoryType(eht);
        }
    }
    else {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "historyState define error:" << h;
        }
        else {
            stream << "historyState unkown";
        }
        m_err = stream.str();
        return;
    }
}

/**
 * @brief CicoStateMachineCreator::CreateFinalState
 * @param cbp definition finalState-object json parts
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::CreateFinalState(CicoBlockParser& cbp,
                                               CicoStateCore* parent)
{
    if (false == cbp.isJsonParse()) {
        if (false == cbp.jsonParse(DKfinalState)) {
            m_err = cbp.getError();
            return;
        }
    }
    picojson::object o = cbp.m_v.get(DKfinalState).get<picojson::object>();
    std::string name;
    int value = 0;
    bool bn = parseName(o, name);
    bool bv = parseValueI(o, value);
    if ((true == bn) || (true == bv)) {
        CicoFinalState* cfs = (CicoFinalState*)0;
        if (true == parent->isStateMachine()) {
            cfs = new CicoFinalState(name, value, (CicoStateMachine*)parent);
        }
        else if (true == parent->isState()) {
            cfs = new CicoFinalState(name, value, (CicoState*)parent);
        }

        if ((CicoFinalState*)0 != cfs) {
            cbp.setVoid((void*)cfs);
        }
    }
    else {
        std::ostringstream stream;
        if (false == cbp.m_parts.empty()) {
            std::string h;
            cbp.getErrorHead(h, cbp.m_parts.c_str());
            stream << "finalState define error:" << h;
        }
        else {
            stream << "finalState unkown";
        }
        m_err = stream.str();
    }
}

/**
 * @brief CicoStateMachineCreator::setConnect
 *        Registration of transition information
 * @param cbp definition json parts
 * @param csm stateMachine object pointer
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::setConnect(CicoBlockParser& cbp,
                                         CicoStateMachine* csm,
                                         CicoStateCore* parent)
{
    switch (cbp.m_kind) {
    case CicoBlockParser::STTMAC:           // stateMachine
        setConnectStateMachine(cbp, csm);
        break;
    case CicoBlockParser::STT:              // state
        setConnectState(cbp, csm);
        break;
    case CicoBlockParser::HSTT:             // history state
        setConnectHistoryState(cbp, csm);
        break;
    case CicoBlockParser::FSTT:             // final state
        break;
    case CicoBlockParser::EV:               // event
        CreateEvent(cbp, csm, parent);
        break;
    default:
        break;
    }
    if (!m_err.empty()) {
        return;
    }
    CicoStateCore* childsParent = (CicoStateCore*)cbp.getVoid();
    int sz = cbp.m_childs.size();
    for (int i=0; i<sz; i++) {
        setConnect(*cbp.m_childs[i], csm, childsParent);
        if (!m_err.empty()) {
            return;
        }
    }

}

/**
 * @brief CicoStateMachineCreator::setConnectStateMachine
 *        Acquisition and registration of the starting position
 * @param cbp definition json parts
 * @param csm stateMachine object pointer
 */
void CicoStateMachineCreator::setConnectStateMachine(CicoBlockParser& cbp,
                                                     CicoStateMachine* csm)
{
    setInitial(cbp, DKstateMachine, csm);
}

/**
 * @brief CicoStateMachineCreator::setInitial
 *        Acquisition and registration of the starting position
 * @param cbp definition json parts
 * @param key json object parse key
 * @param csm stateMachine object pointer

 */
void CicoStateMachineCreator::setInitial(CicoBlockParser& cbp, const char* key,
                                         CicoStateMachine* csm)
{
    CicoStateCore* csc = (CicoStateCore*)cbp.getVoid();
    if ((CicoStateCore*)0 == csc) {
        m_err = std::string("have no stateCore Pointer");
        return;
    }
    picojson::object o = cbp.m_v.get(key).get<picojson::object>();
    picojson::value& vini = o[DKinitial];
    std::vector<CicoStateCore*> vec;
    std::string err;
    if (false == parseStateObject(vini, vec, csm, err)) {
        std::string tmp("initial ");
        tmp += m_err;
        m_err = tmp;
        return;
    }
    int sz = vec.size();
    if (0 == sz) {
        vini = o[DKinitial2];
        if (false == parseStateObject(vini, vec, csm, err)) {
            std::string tmp("i ");
            tmp += m_err;
            m_err = tmp;
            return;
        }
    }
    sz = vec.size();
    if (0 == sz) {
        return;
    }
    for (int i = 0; i < sz; i++) {
        addInitial(csc, vec[i]);
    }
}

/**
 * @brief CicoStateMachineCreator::addInitial
 *        registration of the starting position
 * @param tgt registration target
 * @param dat registration position data
 */
void CicoStateMachineCreator::addInitial(CicoStateCore* tgt,
                                         CicoStateCore* dat)
{
    CicoStateMachine* tgt1 = (CicoStateMachine*)0;
    CicoState* tgt2 = (CicoState*)0;
    if (tgt->isStateMachine()) {
        tgt1 = (CicoStateMachine*)tgt;
    }
    else if (tgt->isState()) {
        tgt2 = (CicoState*)tgt;
    }
    else {
        m_err = std::string("target type mismatch");
        return;
    }

    if (dat->isState()) {
        if (tgt1) {
            tgt1->addInitState((CicoState*)dat);
        }
        else if (tgt2) {
            tgt2->addInitState((CicoState*)dat);
        }
    }
    else if (dat->isHistoryState()) {
        if (tgt1) {
            tgt1->addInitState((CicoHistoryState*)dat);
        }
        else if (tgt2) {
            tgt2->addInitState((CicoHistoryState*)dat);
        }
    }
    else {
        m_err = std::string("get pointer type mismatch");
        return;
    }
}

/**
 * @brief CicoStateMachineCreator::setConnectState
 *        Acquisition and registration of the starting position
 *        and finish position to state-object
 * @param cbp definition json parts
 * @param csm stateMachine object pointer
 */
void CicoStateMachineCreator::setConnectState(CicoBlockParser& cbp,
                                              CicoStateMachine* csm)
{
    setInitial(cbp, DKstate, csm);
    setFinish(cbp, DKstate, csm);
}

/**
 * @brief CicoStateMachineCreator::setFinish
 *        Acquisition and registration of the and finish position
 * @param cbp definition json parts
 * @param key json parse get key
 * @param csm stateMachine object pointer
 */
void CicoStateMachineCreator::setFinish(CicoBlockParser& cbp, const char* key,
                                        CicoStateMachine* csm)
{
    CicoStateCore* csc = (CicoStateCore*)cbp.getVoid();
    if ((CicoStateCore*)0 == csc) {
        m_err = std::string("have no stateCore Pointer");
        return;
    }
    if (false == csc->isState()) {
        m_err = std::string("have no state Pointer");
        return;
    }
    CicoState* tgt = (CicoState*)csc;
    const CicoStateCore* dat = (CicoStateCore*)0;
    picojson::object o = cbp.m_v.get(key).get<picojson::object>();
    picojson::value& vf = o[DKfinish];
    std::vector<CicoStateCore*> vec;
    std::string err;
    if (false == parseStateObject(vf, vec, csm, err)) {
        std::string tmp("finish ");
        tmp += err;
        m_err = tmp;
        return;
    }
    int sz = vec.size();
    if (0 == sz) {
        vf = o[DKfinish2];
        if (false == parseStateObject(vf, vec, csm, err)) {
            std::string tmp("finish ");
            tmp += err;
            m_err = tmp;
            return;
        }
    }
    std::string h;
    if (false == cbp.m_parts.empty()) {
        cbp.getErrorHead(h, cbp.m_parts.c_str());
    }
    sz = vec.size();
    if (1 < sz) {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "finish is multiple:" << h;
        }
        else {
            stream << "finish is multiple";
        }
        m_err = stream.str();
    }
    if (1 == sz ) {
        dat = vec[0];
        if (dat->isState()) {
            tgt->setFinishTransition((CicoState*)dat);
        }
        else if (dat->isHistoryState()) {
            tgt->setFinishTransition((CicoHistoryState*)dat);
        }
        else if (dat->isFinalState()) {
            tgt->setFinishTransition((CicoFinalState*)dat);
        }
        else {
            std::ostringstream stream;
            if (false == h.empty()) {
                stream << "finish is mismatch:" << h;
            }
            else {
                stream << "finish is mismatch";
            }
            m_err = stream.str();
        }
    }
}

/**
 * @brief CicoStateMachineCreator::setConnectHistoryState
 * @param cbp definition json parts
 * @param csm stateMachine object pointer
 */
void CicoStateMachineCreator::setConnectHistoryState(CicoBlockParser& cbp,
                                                     CicoStateMachine* csm)
{
    CicoStateCore* csc = (CicoStateCore*)cbp.getVoid();
    if ((CicoState*)0 == csc) {
        m_err = std::string("have no stateCore Pointer");
        return;
    }
    if (false == csc->isHistoryState()) {
        m_err = std::string("have no historyState Pointer");
        return;
    }
    CicoHistoryState* chs = (CicoHistoryState*)csc;
    setDefault(cbp, chs, csm);
}

/**
 * @brief CicoStateMachineCreator::setDefault
 * @param cbp definition json parts
 * @param chs historyState-objects
 * @param csm stateMachine object pointer
 */
void CicoStateMachineCreator::setDefault(CicoBlockParser& cbp,
                                         CicoHistoryState* chs,
                                         CicoStateMachine* csm)
{
    picojson::object o = cbp.m_v.get(DKhistoryState).get<picojson::object>();
    picojson::value& vd = o[DKdefault];
    std::vector<CicoStateCore*> vec;
    std::string err;
    if (false == parseStateObject(vd, vec, csm, err)) {
        std::string tmp("default ");
        tmp += err;
        m_err = tmp;
        return;
    }
    int sz = vec.size();
    if (0 == sz) {
        vd = o[DKdefault2];
        if (false == parseStateObject(vd, vec, csm, err)) {
            std::string tmp("d ");
            tmp += err;
            m_err = tmp;
            return;
        }
    }
    sz = vec.size();
    if (0 == sz) {
        return;
    }
    for (int i = 0; i < sz; i++) {
        addDefault(chs, vec[i]);
    }
}

/**
 * @brief CicoStateMachineCreator::addDefault
 * @param chs registration target
 * @param csc default position object
 */
void CicoStateMachineCreator::addDefault(CicoHistoryState* chs, CicoStateCore* csc)
{
    if (false == csc->isState()) {
        m_err = std::string("have no state Pointer");
        return;
    }
    chs->addDefaultState((CicoState*)csc);
}

/**
 * @brief CicoStateMachineCreator::CreateEvent
 *        registration of event triger and transition
 * @param cbp definition json parts 
 * @param csm stateMachine object pointer
 * @param parent Hierarchical information
 */
void CicoStateMachineCreator::CreateEvent(CicoBlockParser& cbp,
                                          CicoStateMachine* csm,
                                          CicoStateCore* parent)
{
    if ((CicoState*)0 == parent) {
        m_err = std::string("have no stateCore Parent");
        return;
    }
    if (false == parent->isState()) {
        m_err = std::string("no State Parent");
        return;
    }
    std::string h;
    if (false == cbp.m_parts.empty()) {
        cbp.getErrorHead(h, cbp.m_parts.c_str());
    }
    if (false == cbp.isJsonParse()) {
        if (false == cbp.jsonParse(DKevent)) {
            m_err = cbp.getError();
            return;
        }
    }
    CicoState* cs = (CicoState*)parent;
    picojson::object o = cbp.m_v.get(DKevent).get<picojson::object>();
    // get name
    std::string name;
    parseName(o, name);
    // get value
    int value = 0;
    if (false == parseValueI(o, value)) {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "value is mismatch:" << h;
        }
        else {
            stream << "value is mismatch";
        }
        m_err = stream.str();
        return;
    }
    CicoEventInfo evi(name, (unsigned short)value);
    // tarnsition get
    picojson::value& vd = o[DKtransition];
    std::vector<CicoStateCore*> vec;
    std::string err;
    if (false == parseStateObject(vd, vec, csm, err)) {
        std::string tmp("transition ");
        tmp += err;
        m_err = tmp;
        return;
    }
    int sz = vec.size();
    if (0 == sz) {
        vd = o[DKtransition2];
        if (false == parseStateObject(vd, vec, csm, err)) {
            std::string tmp("t ");
            tmp += err;
            m_err = tmp;
            return;
        }
    }
    sz = vec.size();
    if (1 != sz) {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "transition is mismatch:" << h;
        }
        else {
            stream << "transition is mismatch";
        }
        m_err = stream.str();
        return;
    }
    const CicoStateCore* trs = vec[0];
    // get guard condition
    picojson::value& vgc = o[DKguardCondition];
    if (false == regGuardCondition(vgc, evi)) {
        vgc = o[DKguardCondition2];
        regGuardCondition(vgc, evi);
    }
    // register event infomation
    if (trs->isState()) {
        cs->addTransition(evi, (CicoState*)trs);
    }
    else if (trs->isHistoryState()) {
        cs->addTransition(evi, (CicoHistoryState*)trs);
    }
    else if (trs->isFinalState()) {
        cs->addTransition(evi, (CicoFinalState*)trs);
    }
    else {
        std::ostringstream stream;
        if (false == h.empty()) {
            stream << "transition is mismatch:" << h;
        }
        else {
            stream << "transition is mismatch";
        }
        m_err = stream.str();
        return;
    }
}

/**
 * @brief CicoStateMachineCreator::regGuardCondition
 * @param v json parse data
 * @param evi registration of event target
 * @return
 */
bool CicoStateMachineCreator::regGuardCondition(picojson::value& v,
                                                CicoEventInfo& evi)
{
    bool r = false;
    if (v.is<picojson::array>()) {
        picojson::array& arry = v.get<picojson::array>();
        for (picojson::array::const_iterator it = arry.begin();
             it != arry.end(); it++) {
            std::string vdata;
            if (it->is<picojson::object>()) {
                picojson::object ogc = it->get<picojson::object>();
                setGuardCondition(evi, ogc);
                r = true;
            }
        }
    }
    else if (v.is<picojson::object>()) {
        picojson::object ogc = v.get<picojson::object>();
        setGuardCondition(evi, ogc);
        r = true;
    }
    return r;
}

/**
 * @brief CicoStateMachineCreator::setGuardCondition
 * @param evi registration of event target
 * @param o definition gurd condition data of event
 */
void CicoStateMachineCreator::setGuardCondition(CicoEventInfo& evi, picojson::object& o)
{
    std::string op;
    if (false == parseOperator(o, op)) {
        m_err = std::string("operator unkown");
        return;
    }
    bool bAnd = false;
    std::string j;
    if (true ==parseJoin(o, j)) {
        if (std::string(DVand)==j) {
            bAnd = true;
        }
        else if (std::string(DVor)!=j) {
            m_err = std::string("join unkown");
            return;
        }
    }
    int num;
    double dbl;
    std::string str;
    if (true == parseValueIS(o, num, str)) {
        if (str.empty()) {
            if (true == bAnd) {
                evi.addAndGurdCondition(op, num);
            }
            else {
                evi.addOrGurdCondition(op, num);
            }
        }
        else {
            if (true == bAnd) {
                evi.addAndGurdCondition(op, str);
            }
            else {
                evi.addOrGurdCondition(op, str);
            }
        }
    }
    else if (true == parseValuedD(o, dbl)) {
        if (true == bAnd) {
            evi.addAndGurdCondition(op, dbl);
        }
        else {
            evi.addOrGurdCondition(op, dbl);
        }
    }
    else {
        m_err = std::string("value,valued unkown");
        return;
    }
}

/**
 * @brief CicoStateMachineCreator::getJsonString
 * @param v json parse value
 * @param str store json parse value
 * @return true:get success  false:get fail
 */
bool CicoStateMachineCreator::getJsonString(picojson::value& v,
                                            std::string& str)
{
    if (v.is<std::string>()) {
        str = v.get<std::string>();
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::getJsonNumber
 * @param v json parse value
 * @param num store json parse value
 * @return true:get success   false:get fail
 */
bool CicoStateMachineCreator::getJsonNumber(picojson::value& v, int& num)
{
    if (v.is<double>()) {
        num = (int)v.get<double>();
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::getJsonDouble
 * @param v json parse value
 * @param d store json parse value
 * @return true:get success  false:get fail
 */
bool CicoStateMachineCreator::getJsonDouble(picojson::value& v, double& d)
{
    if (v.is<double>()) {
        d = (double)v.get<double>();
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::parseName
 * @param o  json parse objects
 * @param nm store json parse "name" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseName(picojson::object& o, std::string& nm)
{
    picojson::value& vnm = o[DKname];
    if (true == getJsonString(vnm, nm)) {
        return true;
    }
    vnm = o[DKname2];
    if (true == getJsonString(vnm, nm)) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::parseValueI
 * @param o  json parse objects
 * @param v store json parse "value" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseValueI(picojson::object& o, int& v)
{
    picojson::value& vvl = o[DKvalue];
    if (true == getJsonNumber(vvl, v)) {
        return true;
    }
    vvl = o[DKvalue2];
    if (true == getJsonNumber(vvl, v)) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::parseValueIS
 * @param o json parse objects
 * @param n store json parse "value" object value
 * @param s store json parse "value" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseValueIS(picojson::object& o, int& n,
                                           std::string& s)
{
    picojson::value& vvl = o[DKvalue];
    if (true == getJsonNumber(vvl, n)) {
        s.clear();
        return true;
    }
    else if (true == getJsonString(vvl, s)) {
        return true;
    }
    vvl = o[DKvalue2];
    if (true == getJsonNumber(vvl, n)) {
        s.clear();
        return true;
    }
    else if (true == getJsonString(vvl, s)) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::parseValuedD
 * @param o  json parse objects
 * @param v store json parse "valued" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseValuedD(picojson::object& o, double& v)
{
    picojson::value& vvl = o[DKvalued];
    if (true == getJsonDouble(vvl, v)) {
        return true;
    }
    vvl = o[DKvalued2];
    if (true == getJsonDouble(vvl, v)) {
        return true;
    }
    return false;
}


/**
 * @brief CicoStateMachineCreator::parseHistoryType
 * @param o  json parse objects
 * @param e store json parse "type" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseHistoryType(picojson::object& o,
                                               CicoHistoryState::E_HType& e)
{
    picojson::value& vty = o[DKtype];
    std::string tmp;
    if (false == getJsonString(vty, tmp)) {
        vty = o[DKtype2];
        if (false == getJsonString(vty, tmp)) {
            return false;
        }
    }
    if (false == tmp.empty()) {
        if (std::string(DVShallow) == tmp) {
            e = CicoHistoryState::Shallow;
            return true;
        }
        else if (std::string(DVDeep) == tmp) {
            e = CicoHistoryState::Deep;
            return true;
        }
    }
    return false;
}

/**
 * @brief CicoStateMachineCreator::parseStateObject
 * @param v json parse value
 * @param vec store state-objects pointer array
 * @param csm stateMachine object pointer
 * @param err store gat fail error message
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseStateObject(picojson::value& v,
                                               std::vector<CicoStateCore*>& vec,
                                               CicoStateMachine* csm,
                                               std::string& err)
{
    if (v.is<picojson::array>()) {
        picojson::array& arry = v.get<picojson::array>();
        for (picojson::array::const_iterator it = arry.begin();
             it != arry.end(); it++) {
            std::string vdata;
            if (it->is<std::string>()) {
                std::string v = it->get<std::string>();
                const CicoStateCore* csc = csm->getState(v);
                if ((CicoStateCore*)0 == csc) {
                    std::ostringstream stream;
                    stream << "state is not find:V:\"" << v << "\"";
                    err = stream.str();
                    return false;
                }
                vec.push_back((CicoStateCore*)csc);
            }
            else if (it->is<double>()) {
                int v = (int) it->get<double>();
                const CicoStateCore* csc = csm->getState(v);
                if ((CicoStateCore*)0 == csc) {
                    std::ostringstream stream;
                    stream << "state is not find:V:" << v;
                    err = stream.str();
                    return false;
                }
                vec.push_back((CicoStateCore*)csc);
            }
        }
    }
    else if (v.is<double>()) {
        int vi = (int) v.get<double>();
        const CicoStateCore* csc = csm->getState(vi);
        if ((CicoStateCore*)0 == csc) {
            std::ostringstream stream;
            stream << "state is not find:S:" << vi;
            err = stream.str();
            return false;
        }
        vec.push_back((CicoStateCore*)csc);
    }
    else if (v.is<std::string>()) {
        std::string vs = v.get<std::string>();
        const CicoStateCore* csc = csm->getState(vs);
        if ((CicoStateCore*)0 == csc) {
            std::ostringstream stream;
            stream << "state is not find:S:\"" << vs << "\"";
            err = stream.str();
            return false;
        }
        vec.push_back((CicoStateCore*)csc);
    }
    return true;
}

/**
 * @brief CicoStateMachineCreator::parseOperator
 * @param o json parse object
 * @param op store json "operation" object value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseOperator(picojson::object& o, std::string& op)
{
    picojson::value& vop = o[DKoperator];
    if (true == getJsonString(vop, op)) {
        return true;
    }
    vop = o[DKoperator2];
    if (true == getJsonString(vop, op)) {
        return true;
    }
    return false;
}


/**
 * @brief CicoStateMachineCreator::parseJoin
 * @param o json parse object
 * @param j store "join" value
 * @return true:get success false:get fail
 */
bool CicoStateMachineCreator::parseJoin(picojson::object& o, std::string& j)
{
    picojson::value& vnm = o[DKjoin];
    if (true == getJsonString(vnm, j)) {
        return true;
    }
    vnm = o[DKjoin2];
    if (true == getJsonString(vnm, j)) {
        return true;
    }
    return false;
}
