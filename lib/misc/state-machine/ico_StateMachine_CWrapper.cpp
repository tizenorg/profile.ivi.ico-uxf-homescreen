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
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateAction.h"
#include "CicoStateCore.h"
#include "CicoState.h"
#include "CicoHistoryState.h"
#include "CicoFinalState.h"
#include "CicoStateMachine.h"
#include "CicoStateMachineCreator.h"
#include "ico_StateMachine_CWrapper.h"
#include "CicoStateInternal.h"

/**
 * stateMachine C Wrapper functions
 */

static std::vector<CicoStateCore*>  stateObjects;
static std::vector<CicoEventInfo*>  evInfObjects;
static std::vector<void*>           callbackFnc;

StateMachineCWrapperErr_E sttmerr;


/**
 * @brief GCOpKind
 * @      Definitions used in the function ico_sttm_addTransitionGCI ,
 * @      ico_sttm_addTransitionGCD , ico_sttm_addTransitionGCS
 * @      ico_sttm_addGCI , ico_sttm_addGCD , ico_sttm_addGCS
 */
const char* GCOpKind[] = {
    "",
    DEFJGCE_EQ,     // "="   Guard condition is equal to
    DEFJGCE_NE,     // "!="  Guard condition is not equal to
    DEFJGCE_LT,     // "<"   Guard condition is less than
    DEFJGCE_LE,     // "<="  Guard condition is less than or equal
    DEFJGCE_GT,     // ">"   Guard condition is greater than
    DEFJGCE_GE,     // ">="  Guard condition is greater than or equal
    DEFJGCE_IN,     // "in"  Guard condition is the state of the state-object
    DEFJGCE_NOT,    // "not" Guard condition is the state of the state-object
};

/**
 * @brief getID2ObjSTT
 * @      c-wrapper local ID to CicoStateCore pointer
 * @param id c-wrapper local ID
 * @return not 0:get success
 * @       0:get faile
 */
CicoStateCore* getID2ObjSTT(int id)
{
    int sz = stateObjects.size();
    if ((1 > id) || (id > sz)) {
        return (CicoStateCore*)0;
    }
    return stateObjects[(id-1)];
}


/**
 * @brief getID2ObjSTTX
 * @      c-wrapper local ID to CicoStateCore pointer
 * @param id c-wrapper local ID
 * @return not 0:get success
 * @       0:get faile
 */
extern "C" void* getID2ObjSTTX(int id)
{
    return (void*)getID2ObjSTT(id);
}

/**
 * @brief getObjSTT2ID
 * @      CicoStateCore pointer to c-wrapper local ID
 * @param stt 
 * @return not 0:get success(c-wrapper local ID)
 * @       0:get faile
 */
int getObjSTT2ID(const CicoStateCore* stt)
{
    int r = 0;
    int sz = stateObjects.size();
    for (int i=0; i<sz; i++) {
        if (stt == stateObjects[i]) {
            r = i+1;
            break;                      // break of for i<sz
        }
    }
    return r;
}

/**
 * @brief getID2ObjEV
 * @      c-wrapper local ID to CicoEventInfo pointer
 * @param id c-wrapper local ID
 * @return not 0:CicoEventInfo pointer
 * @       0:get faile
 */
CicoEventInfo*  getID2ObjEV(int id)
{
    int sz = evInfObjects.size();
    if ((1 > id) || (id > sz)) {
        return (CicoEventInfo*)0;
    }
    return evInfObjects[(id-1)];
}

/**
 * @brief supplementName
 * @      supplement name
 * @param name
 * @return supplement name
 */
const char* supplementName(const char* name)
{
    static const char* emptychar = "";
    if ((char*)0 == name) {
        return emptychar;
    }
    return name;
}

/* Registration system */
/**
 * @brief Create an stateMachine-object and state-object with the specified file
 * @param file faile path pseudo-json
 * @return not 0:stateMachine-object Id no.(c-wrapper local ID)
 * @       0:create error
 */
extern "C" int ico_sttm_createStateMachineFile(const char* file)
{
    sttmerr = STTMNOERROR;
    CicoStateMachineCreator csmc;
    CicoStateMachine* csm = csmc.createFile(file);
    if ((CicoStateMachine*)0 == csm) {
        sttmerr = STTMCREATEERR;
        _DBG("ico_sttm_createStateMachineFile %s", csmc.getError().c_str());
        return 0;
    }
    csm->getObjects(stateObjects);
    int r = 0;
    int sz = stateObjects.size();
    for (int i = 0; i < sz; i++) {
        if (stateObjects[i] == csm) {   // match object pointer
            r = i+1;                    // get return value
            break;                      // break of for i<sz
        }
    }
    return r;
}


/**
 * @brief Create an stateMachine-object and state-object with the specified text
 * @param buff text pseudo-json
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
extern "C" int ico_sttm_createStateMachineText(const char* buff)
{
    sttmerr = STTMNOERROR;
    CicoStateMachineCreator csmc;
    CicoStateMachine* csm = csmc.create(buff);
    if ((CicoStateMachine*)0 == csm) {
        sttmerr = STTMCREATEERR;
        _DBG("ico_sttm_createStateMachineText %s", csmc.getError().c_str());
        return 0;
    }
    csm->getObjects(stateObjects);      // get all state-objects
    int r = 0;                          //
    int sz = stateObjects.size();       //
    for (int i = 0; i < sz; i++) {      //
        if (stateObjects[i] == csm) {   // match object pointer
            r = i+1;                    // get return value
            break;                      // break of for i<sz
        }
    }
    return r;
}

/**
 * @brief Create stateMachine-object
 * @param name stateMachine-object name (Option)
 * @param value stateMachine-object ID no.
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
extern "C" int ico_sttm_createStateMachine(const char* name, int value)
{
    sttmerr = STTMNOERROR;
    CicoStateMachine* csm;
    const char* sname = supplementName(name);
    csm = new CicoStateMachine(sname, value);
    if ((CicoStateMachine*)0 == csm) {
        sttmerr = STTMNEWERR;
        _ERR("ico_sttm_createStateMachine \"%s\", %d", sname, value);
        return 0;
    }
    stateObjects.push_back(csm);
    return stateObjects.size();
}

/**
 * @brief Create state-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name state-object name (Option)
 * @param value state-object ID no.
 * @return not 0:state-object ID no.(c-wrapper local ID)
 * @       0:create faile
 */
extern "C" int ico_sttm_createState(int parent, const char* name, int value)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(name);
    CicoStateCore* cscParent = getID2ObjSTT(parent); // ID to state-object
    if ((CicoStateCore*)0 == cscParent) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_createState >>%d<<, \"%s\", %d", parent, sname,value);
        return 0;
    }
    CicoState* cs = (CicoState*)0;
    if (cscParent->isStateMachine()) {
        cs = new CicoState(sname, value, (CicoStateMachine*)cscParent);
    }
    else if (cscParent->isState()) {
        cs = new CicoState(sname, value, (CicoState*)cscParent);
    }
    if ((CicoState*)0 == cs) {
        sttmerr = STTMNEWERR;
        _ERR("ico_sttm_createState %d(%s:%d:%d), \"%s\", %d",
             parent, cscParent->getName().c_str(), cscParent->getValue(),
             (int)cscParent->getType(), sname, value);
        return 0;
    }
    stateObjects.push_back(cs);
    return stateObjects.size();
}

/**
 * @brief Create historyState-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name historyState-object name (Option)
 * @param value historyState-object ID no.
 * @return not 0:historyState-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
extern "C" int ico_sttm_createHState(int parent, const char* name, int value,
                                 HISTORY_TYPE_E typ)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(name);
    CicoStateCore* cscParent = getID2ObjSTT(parent); // ID to state-object
    if ((CicoStateCore*)0 == cscParent) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_createHState >>%d<<, \"%s\", %d, %d", parent, sname, value, (int)typ);
        return 0;
    }
    CicoHistoryState* chs = (CicoHistoryState*)0;
    if (cscParent->isStateMachine()) {
        chs = new CicoHistoryState(sname, value, (CicoStateMachine*)cscParent);
    }
    else if (cscParent->isState()) {
        chs = new CicoHistoryState(sname, value, (CicoState*)cscParent);
    }
    if ((CicoHistoryState*)0 == chs) {
        sttmerr = STTMNEWERR;
        _ERR("ico_sttm_createHState >>%d(%s:%d:%d)<<, \"%s\", %d, %d",
             parent, cscParent->getName().c_str(), cscParent->getValue(),
             (int)cscParent->getType(), sname, value, (int)typ);
        return 0;
    }
    if (E_SHALLOW == typ) {
        chs->setHistoryType(CicoHistoryState::Shallow);
    }
    else if (E_DEEP == typ) {
        chs->setHistoryType(CicoHistoryState::Deep);
    }
    else {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_createHState %d(%s:%d:%d), \"%s\", %d,>>%d<<",
             parent, cscParent->getName().c_str(), cscParent->getValue(),
             (int)cscParent->getType(), sname, value, (int)typ);
        delete chs;
        return 0;
    }
    stateObjects.push_back((CicoStateCore*)chs);
    return stateObjects.size();
}

/**
 * @brief Create finalState-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name finalState-object name (Option)
 * @param value finalState-object ID no.
 * @return not 0:finalState-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
extern "C" int ico_sttm_createFState(int parent, const char* name, int value)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(name);
    CicoStateCore* cscParent = getID2ObjSTT(parent); // ID to state-object
    if ((CicoStateCore*)0 == cscParent) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_createFState >>%d<<, \"%s\", %d", parent, sname, value);
        return 0;
    }
    CicoFinalState* cfs = (CicoFinalState*)0;
    if (cscParent->isStateMachine()) {
        cfs = new CicoFinalState(sname, value, (CicoStateMachine*)cscParent);
    }
    else if (cscParent->isState()) {
        cfs = new CicoFinalState(sname, value, (CicoState*)cscParent);
    }
    if ((CicoFinalState*)0 == cfs) {
        sttmerr = STTMNEWERR;
        _ERR("ico_sttm_createFState >>%d(%s:%d:%d)<<, \"%s\", %d",
             parent, cscParent->getName().c_str(), cscParent->getValue(),
             (int)cscParent->getType(), sname, value);
        return 0;
    }
    stateObjects.push_back((CicoStateCore*)cfs);
    return stateObjects.size();
}

/**
 * @brief Registration of transition events
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param trCWId of transition destination(c-wrapper local ID)
 * @param evi Registration event
 * @param fname function name of the caller
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
int ico_sttm_addTransitionX(int sCWId, int trCWId, CicoEventInfo& evi,
                      const char* fname)
{
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("%s >>%d<<, \"%s\", %d, ..., %d",
             fname, sCWId, evi.getName().c_str(), (int)evi.getEV(),
             trCWId);
        return 0;
    }
    CicoStateCore* cscTrans = getID2ObjSTT(trCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTrans) {
        sttmerr = STTMNOTFIND;
        _DBG("%s %d, \"%s\", %d, ...,>>%d<<",
             fname, sCWId, evi.getName().c_str(), (int)evi.getEV(),
             trCWId);
        return 0;
    }
    if (false == cscTarget->isState()) {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("%s >>%d(%s:%d:%d)<<, \"%s\", %d, ..., %d",
             fname, sCWId, cscTarget->getName().c_str(),
             cscTarget->getValue(), (int)cscTarget->getType(),
             evi.getName().c_str(), (int)evi.getEV(), trCWId);
        return 0;
    }
    CicoState* csTgt = (CicoState*)cscTarget;
    if (true == cscTrans->isState()) {
        csTgt->addTransition(evi, (CicoState*)cscTrans);
    }
    else if (true == cscTrans->isFinalState()) {
        csTgt->addTransition(evi, (CicoFinalState*)cscTrans);
    }
    else if (true == cscTrans->isHistoryState()) {
        csTgt->addTransition(evi, (CicoHistoryState*)cscTrans);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("%s %d, \"%s\", %d, ...,>>%d(%s:%d:%d)<<",
             fname, sCWId, evi.getName().c_str(), (int)evi.getEV(),
             trCWId, cscTarget->getName().c_str(),
             cscTarget->getValue(), (int)cscTarget->getType());
        return 0;
    }
    const CicoEventInfo* t = csTgt->getEventInfo(evi.getName(), evi.getEV());
    evInfObjects.push_back((CicoEventInfo*)t);
    return evInfObjects.size();
}

/**
 * @brief Registration of transition events
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param name event name
 * @param ev event no.(Use the 1000-65535)
 * @param trCWId of transition destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransition(int sCWId, 
                                      const char* evname, unsigned short ev,
                                      int trCWId)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(evname);
    CicoEventInfo evi(sname, ev);
    int r = ico_sttm_addTransitionX(sCWId, trCWId, evi, "ico_sttm_addTransition");
    return r;
}


/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param name state-object Identification name
 * @param evname event name
 * @param ev event no.(Use the 1000-65535)
 * @param trVal state-object identification value destination
 * @param namTrns state-object Identification name destination
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransition_V_(int smCWId, int value,
                                         const char* evname, unsigned short ev,
                                         int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransition_VV >>%d<<,>>%d<<, %s,%d, %d", smCWId,
             value, evname, ev, trCWId);
        return 0;
    }
    return ico_sttm_addTransition(id, evname, ev, trCWId);
}

extern "C" int ico_sttm_addTransition_VV(int smCWId, int value,
                                         const char* evname, unsigned short ev,
                                         int trVal)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrieveSVal(smCWId, trVal);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransition_VV >>%d<<, %d, %s,%d,>>%d<<", smCWId,
             value, evname, ev, trVal);
        return 0;
    }
    return ico_sttm_addTransition_V_(smCWId, value, evname, ev, idT);
}

extern "C" int ico_sttm_addTransitionVVV(int smVal, int value,
                                         const char* evname, unsigned short ev,
                                         int trVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransitionVVV >>%d<<, %d, %s,%d, %d", smVal,
             value, evname, ev, trVal);
        return 0;
    }
    return ico_sttm_addTransition_VV(id, value, evname, ev, trVal);
}

extern "C" int ico_sttm_addTransition_N_(int smCWId, const char* name,
                                         const char* evname, unsigned short ev,
                                         int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransitionVVV >>%d<<,>>%s<<, %s,%d,%d", smCWId,
             name, evname, ev, trCWId);
        return 0;
    }
    return ico_sttm_addTransition(id, evname, ev, trCWId);


}
extern "C" int ico_sttm_addTransition_NN(int smCWId, const char* name,
                                         const char* evname, unsigned short ev,
                                         const char* trNm)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrivevSName(smCWId, trNm);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransitionVVV >>%d<<, %s, %s,%d,>>%s<<", smCWId,
             name, evname, ev, trNm);
        return 0;
    }
    return ico_sttm_addTransition_N_(smCWId, name, evname, ev, idT);
}

extern "C" int ico_sttm_addTransitionNNN(const char* smNm, const char* name,
                                         const char* evname, unsigned short ev,
                                         const char* trNm)
{
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransitionNNN >>%s<<, %s, %s, %d, %s", smNm,
             name, evname, ev, trNm);
        return 0;
    }
    return ico_sttm_addTransition_NN(id, name, evname, ev, trNm);
}

/**
 * @brief Registration of transition events
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param name event name
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trCWId destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransitionGCI(int sCWId,
                                         const char* evname, unsigned short ev,
                                         GC_OP_KIND_E gce, int gcv, int trCWId)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(evname);
    CicoEventInfo evi(sname, ev, GCOpKind[gce], gcv);
    int r = ico_sttm_addTransitionX(sCWId, trCWId, evi, "ico_sttm_addTransitionGCI");
    return r;
}

extern "C" int ico_sttm_addTransitionGCD(int sCWId,
                                         const char* evname, unsigned short ev,
                                         GC_OP_KIND_E gce, double gcv,
                                         int trCWId)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(evname);
    CicoEventInfo evi(sname, ev, GCOpKind[gce], gcv);
    int r = ico_sttm_addTransitionX(sCWId, trCWId, evi, "ico_sttm_addTransitionGCD");
    return r;
}

extern "C" int ico_sttm_addTransitionGCS(int sCWId,
                                         const char* evname, unsigned short ev,
                                         GC_OP_KIND_E gce, const char* gcv,
                                         int trCWId)
{
    sttmerr = STTMNOERROR;
    const char* sname = supplementName(evname);
    CicoEventInfo evi(sname, ev, GCOpKind[gce], gcv);
    int r = ico_sttm_addTransitionX(sCWId, trCWId, evi, "ico_sttm_addTransitionGCS");
    return r;
}


/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trCWId destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransGCI_V_(int smCWId, int value,
                          const char* evname, unsigned short ev,
                          GC_OP_KIND_E gce, int gcv,
                          int trCWId)
{

    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCI_V_ >>%d<<,>>%d<<, %s, %d, %d, %d, %d",
              smCWId, value, evname, ev, (int)gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCI(id, evname, ev, gce, gcv, trCWId);
}

extern "C" int ico_sttm_addTransGCD_V_(int smCWId, int value,
                          const char* evname, unsigned short ev,
                          GC_OP_KIND_E gce, double gcv,
                          int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCD_V_ >>%d<<,>>%d<<, %s, %d, %d, %f, %d",
              smCWId, value, evname, ev, (int)gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCD(id, evname, ev, gce, gcv, trCWId);
}

extern "C" int ico_sttm_addTransGCS_V_(int smCWId, int value,
                          const char* evname, unsigned short ev,
                          GC_OP_KIND_E gce, const char* gcv,
                          int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCD_V_ >>%d<<,>>%d<<, %s, %d, %d, %s, %d",
              smCWId, value, evname, ev, (int)gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCS(id, evname, ev, gce, gcv, trCWId);
}

/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trVl value is state-object identification number transition, destination
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransGCI_VV(int smCWId, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, int gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrieveSVal(smCWId, trVl);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCI_VV >>%d<<, %d, %s, %d, %d, %d,>>%d<<",
              smCWId, value, evname, ev, (int)gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCI_V_(smCWId, value, evname, ev, gce, gcv, idT);
}

extern "C" int ico_sttm_addTransGCD_VV(int smCWId, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, double gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrieveSVal(smCWId, trVl);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCD_VV >>%d<<, %d, %s, %d, %d, %f,>>%d<<",
              smCWId, value, evname, ev, (int)gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCD_V_(smCWId, value, evname, ev, gce, gcv, idT);
}

extern "C" int ico_sttm_addTransGCS_VV(int smCWId, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, const char* gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrieveSVal(smCWId, trVl);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _ERR("ico_sttm_addTransGCS_VV >>%d<<, %d, %s, %d, %d, %s,>>%d<<",
              smCWId, value, evname, ev, (int)gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCS_V_(smCWId, value, evname, ev, gce, gcv, idT);
}



extern "C" int ico_sttm_addTransGCIVVV(int smVal, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, int gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCIVVV >>%d<<, %d, %s, %d, %d, %d, %d", smVal,
             value, evname, ev, gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCI_VV(id, value, evname, ev, gce, gcv, trVl);
}

extern "C" int ico_sttm_addTransGCDVVV(int smVal, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, double gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCIVVV >>%d<<, %d, %s, %d, %d, %f, %d", smVal,
             value, evname, ev, gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCD_VV(id, value, evname, ev, gce, gcv, trVl);
}

extern "C" int ico_sttm_addTransGCSVVV(int smVal, int value,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, const char* gcv,
                                       int trVl)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCSVVV >>%d<<, %d, %s, %d, %d, %s, %d", smVal,
             value, evname, ev, gce, gcv, trVl);
        return 0;
    }
    return ico_sttm_addTransGCS_VV(id, value, evname, ev, gce, gcv, trVl);
}

/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param transition destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransGCI_N_(int smCWId, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, int gcv,
                                       int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCI_N_ >>%d<<, %s, %s, %d, %d, %d, %d", smCWId,
             name, evname, ev, gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCI(id, evname, ev, gce, gcv, trCWId);
}

extern "C" int ico_sttm_addTransGCD_N_(int smCWId, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, double gcv,
                                       int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCD_N_ >>%d<<, %s, %s, %d, %d, %f, %d", smCWId,
             name, evname, ev, gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCD(id, evname, ev, gce, gcv, trCWId);
}

extern "C" int ico_sttm_addTransGCS_N_(int smCWId, const char* name, 
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, const char* gcv,
                                       int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCD_N_ >>%d<<, %s, %s, %d, %d, %s, %d", smCWId,
             name, evname, ev, gce, gcv, trCWId);
        return 0;
    }
    return ico_sttm_addTransitionGCS(id, evname, ev, gce, gcv, trCWId);
}

/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param name name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trNm name is state-object Identification name destination
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
extern "C" int ico_sttm_addTransGCI_NN(int smCWId, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, int gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrivevSName(smCWId, trNm);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCI_NN >>%d<<, %s, %s, %d, %d, %d,>>%s<<", smCWId,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCI_N_(smCWId, name, evname, ev, gce, gcv, idT);
}

extern "C" int ico_sttm_addTransGCD_NN(int smCWId, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, double gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrivevSName(smCWId, trNm);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCD_NN >>%d<<, %s, %s, %d, %d, %f,>>%s<<", smCWId,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCD_N_(smCWId, name, evname, ev, gce, gcv, idT);
}

extern "C" int ico_sttm_addTransGCS_NN(int smCWId, const char* name, 
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, const char* gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrivevSName(smCWId, trNm);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addTransGCS_NN >>%d<<, %s, %s, %d, %d, %s,>>%s<<", smCWId,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCS_N_(smCWId, name, evname, ev, gce, gcv, idT);
}

extern "C" int ico_sttm_addTransGCINNN(const char* smNm, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, int gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        _DBG("ico_sttm_addTransGCINNN >>%s<<, %s, %s, %d, %d, %d, %s", smNm,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCI_NN(id, name, evname, ev, gce, gcv, trNm);
}

extern "C" int ico_sttm_addTransGCDNNN(const char* smNm, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, double gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        _DBG("ico_sttm_addTransGCDNNN >>%s<<, %s, %s, %d, %d, %f, %s", smNm,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCD_NN(id, name, evname, ev, gce, gcv, trNm);
}

extern "C" int ico_sttm_addTransGCSNNN(const char* smNm, const char* name,
                                       const char* evname, unsigned short ev,
                                       GC_OP_KIND_E gce, const char* gcv,
                                       const char* trNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        _DBG("ico_sttm_addTransGCSNNN >>%s<<, %s, %s, %d, %d, %s, %s", smNm,
             name, evname, ev, gce, gcv, trNm);
        return 0;
    }
    return ico_sttm_addTransGCS_NN(id, name, evname, ev, gce, gcv, trNm);
}


/**
 * @brief add guard condition
 * @param evi register target
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return true:Success
 * @       false:added failure
 */
bool addGC(CicoEventInfo* evi, GC_JOIN_E j, GC_OP_KIND_E gce, int gcv)
{
    if (E_GC_AND == j) {
        return evi->addAndGurdCondition(GCOpKind[gce], gcv);
    }
    else if (E_GC_OR == j) {
        return evi->addOrGurdCondition(GCOpKind[gce], gcv);
    }
    return false;
}


bool addGC(CicoEventInfo* evi, GC_JOIN_E j, GC_OP_KIND_E gce, double gcv)
{
    if (E_GC_AND == j) {
        return evi->addAndGurdCondition(GCOpKind[gce], gcv);
    }
    else if (E_GC_OR == j) {
        return evi->addOrGurdCondition(GCOpKind[gce], gcv);
    }
    return false;
}


bool addGC(CicoEventInfo* evi, GC_JOIN_E j, GC_OP_KIND_E gce, const char* gcv)
{
    if (E_GC_AND == j) {
        return evi->addAndGurdCondition(GCOpKind[gce], gcv);
    }
    else if (E_GC_OR == j) {
        return evi->addOrGurdCondition(GCOpKind[gce], gcv);
    }
    return false;
}

/**
 * @brief add guard condition
 * @param evId ID no. to be registered(c-wrapper local event-ID)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
extern "C" int ico_sttm_addGCI(int evId, GC_JOIN_E j, GC_OP_KIND_E gce, int gcv)
{
    sttmerr = STTMNOERROR;
    CicoEventInfo* eviTarget = getID2ObjEV(evId);
    if ((CicoEventInfo*)0 == eviTarget) {
        sttmerr = STTMEVNOTFIND;
        _DBG("ico_sttm_addGCI >>%d<<,%d, %d, %d",evId, (int)j, (int)gce, gcv);
        return 0;
    }
    if (false == addGC(eviTarget, j, gce, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addGCI ,>>%d<<, %d, %d, %d",evId, (int)j, (int)gce, gcv);
    }
    return evId;
}

extern "C" int ico_sttm_addGCD(int evId, GC_JOIN_E j, GC_OP_KIND_E gce, double gcv)
{
    sttmerr = STTMNOERROR;
    CicoEventInfo* eviTarget = getID2ObjEV(evId);
    if ((CicoEventInfo*)0 == eviTarget) {
        sttmerr = STTMEVNOTFIND;
        _DBG("ico_sttm_addGCD >>%d<<,%d, %d, %f",evId, (int)j, (int)gce, gcv);
        return 0;
    }
    if (false == addGC(eviTarget, j, gce, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addGCD %d,>>%d<<, %d, %f",evId, (int)j, (int)gce, gcv);
        return 0;
    }
    return evId;
}

extern "C" int ico_sttm_addGCS(int evId, GC_JOIN_E j, GC_OP_KIND_E gce,
                                const char* gcv)
{
    sttmerr = STTMNOERROR;
    if ((char*)0==gcv) {
        sttmerr = STTMEVNOTFIND;
        _DBG("ico_sttm_addGCS %d,%d, %d,>>nill<<",evId, (int)j, (int)gce);
        return 0;
    }
    CicoEventInfo* eviTarget = getID2ObjEV(evId);
    if ((CicoEventInfo*)0 == eviTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCS >>%d<<,%d, %d, \"%s\"",evId, (int)j, (int)gce,
             gcv);
        return 0;
    }
    if (false == addGC(eviTarget, j, gce, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addGCS %d,>>%d<<, %d, \"%s\"",evId, (int)j, (int)gce, gcv);
        return 0;
    }
    return evId;
}

/**
 * @brief ico_sttm_getStateMachineX
 * @param smCWId stateMachine-object ID
 * @param fname calling function Name
 * @return not 0:get success 0:get fail
 */
CicoStateMachine* ico_sttm_getStateMachineX(int smCWId, const char* fname)
{
    CicoStateCore* cscTarget = getID2ObjSTT(smCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("%s >>%d<<",fname, smCWId);
        return (CicoStateMachine*)0;
    }
    if (false == cscTarget->isStateMachine()) {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("%s >>%d(%s:%d:%d)<<", fname, smCWId,
             cscTarget->getName().c_str(), cscTarget->getValue(),
             (int)cscTarget->getType());
        return (CicoStateMachine*)0;
    }
    return (CicoStateMachine*)cscTarget;
}

/**
 * @brief add guard condition
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */

CicoStateCore* getStateObject(int smCWId, int value, const char* fnc)
{
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, fnc);
    if ((CicoStateMachine*)0 == csm) {
        sttmerr = STTMNOTFIND;
        _DBG("%s>>%d<<, %d", fnc, smCWId, value);
        return 0;
    }
    const CicoStateCore* csc = csm->getState(value);
    if ((CicoStateCore*)0 == csc) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d,>>%d<<", fnc, smCWId, value);
        return 0;
    }
    return (CicoStateCore*)csc;
}


CicoStateCore* getStateObject(int smCWId, const char* name, const char* fnc)
{
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, fnc);
    if ((CicoStateMachine*)0 == csm) {
        sttmerr = STTMNOTFIND;
        _DBG("%s>>%d<<, %s", fnc, smCWId, name);
        return 0;
    }
    const CicoStateCore* csc = csm->getState(name);
    if ((CicoStateCore*)0 == csc) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d,>>%s<<", fnc, smCWId, name);
        return 0;
    }
    return (CicoStateCore*)csc;
}


CicoEventInfo* getEventInfoV(int smCWId, int value,
                             const char* evname, unsigned short ev,
                             const char* fnc)
{
    CicoStateCore* csc;
    csc = getStateObject(smCWId, value, fnc);
    if ((CicoStateCore*)0 == csc) {
        return 0;
    }
    const CicoEventInfo* t = csc->getEventInfo(evname, ev);
    if ((CicoEventInfo*)0 == t) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %d,>>%s, %d<<", fnc, smCWId, value, evname, ev);
        return 0;
    }
    return (CicoEventInfo*)t;
}


CicoEventInfo* getEventInfoN(int smCWId, const char* name,
                             const char* evname, unsigned short ev,
                             const char* fnc)
{
    CicoStateCore* csc;
    csc = getStateObject(smCWId, name, fnc);
    if ((CicoStateCore*)0 == csc) {
        return 0;
    }
    const CicoEventInfo* t = csc->getEventInfo(evname, ev);
    if ((CicoEventInfo*)0 == t) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %s,>>%s, %d<<", fnc, smCWId, name, evname, ev);
        return 0;
    }
    return (CicoEventInfo*)t;
}


/**
 * @brief add guard condition
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
extern "C" int ico_sttm_addGCI_V(int smCWId, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, int gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCI_V";
    CicoEventInfo* t;
    t = getEventInfoV(smCWId, value, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %d, %s, %d, >> %d, %d, %d<<", fnc, smCWId, value,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}


extern "C" int ico_sttm_addGCD_V(int smCWId, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, double gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCD_V";
    CicoEventInfo* t;
    t = getEventInfoV(smCWId, value, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %d, %s, %d, >> %d, %d, %f<<", fnc, smCWId, value,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}

extern "C" int ico_sttm_addGCS_V(int smCWId, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCS_V";
    CicoEventInfo* t;
    t = getEventInfoV(smCWId, value, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %d, %s, %d, >> %d, %d, %s<<", fnc, smCWId, value,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}

/**
 * @brief add guard condition
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
extern "C" int ico_sttm_addGCIVV(int smVal, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, int gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCIVV >>%d<<, %d, %s, %d, %d, %d, %d", smVal,
             value, evname, ev, j, gc, gcv);
        return 0;
    }
    return ico_sttm_addGCI_V(id, value, evname, ev, j, gc, gcv);
}

extern "C" int ico_sttm_addGCDVV(int smVal, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, double gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCDVV >>%d<<, %d, %s, %d, %d, %d, %f", smVal,
             value, evname, ev, j, gc, gcv);
        return 0;
    }
    return ico_sttm_addGCD_V(id, value, evname, ev, j, gc, gcv);
}

extern "C" int ico_sttm_addGCSVV(int smVal, int value,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCSVV >>%d<<, %d, %s, %d, %d, %d, %s", smVal,
             value, evname, ev, j, gc, gcv);
        return 0;
    }
    return ico_sttm_addGCS_V(id, value, evname, ev, j, gc, gcv);
}


/**
 * @brief add guard condition
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
extern "C" int ico_sttm_addGCI_N(int smCWId, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, int gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCI_N";
    CicoEventInfo* t;
    t = getEventInfoN(smCWId, name, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %s, %s, %d, >> %d, %d, %d<<", fnc, smCWId, name,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}

extern "C" int ico_sttm_addGCD_N(int smCWId, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, double gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCD_N";
    CicoEventInfo* t;
    t = getEventInfoN(smCWId, name, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %s, %s, %d, >> %d, %d, %f<<", fnc, smCWId, name,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}


extern "C" int ico_sttm_addGCS_N(int smCWId, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv)
{
    sttmerr = STTMNOERROR;
    const char* fnc = "ico_sttm_addGCS_N";
    CicoEventInfo* t;
    t = getEventInfoN(smCWId, name, evname, ev, fnc);
    if ((CicoEventInfo*)0 == t) {
        return 0;
    }
    if (false == addGC(t, j, gc, gcv)) {
        sttmerr = STTMPARAMERR;
        _DBG("%s %d, %s, %s, %d, >> %d, %d, %s<<", fnc, smCWId, name,
             evname, ev, (int)j, (int)gc, gcv);
        return 0;
    }
    return 1;
}

/**
 * @brief add guard condition
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
extern "C" int ico_sttm_addGCINN(const char* smNm, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, int gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCINN >>%s<<, %s, %s, %d, %d, %d, %d", smNm, 
             name, evname, ev, j,  gc, gcv);
        return 0;
    }
    return ico_sttm_addGCI_N(id, name, evname, ev, j,  gc, gcv);
}

extern "C" int ico_sttm_addGCDNN(const char* smNm, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, double gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCDNN >>%s<<, %s, %s, %d, %d, %d, %f", smNm, 
             name, evname, ev, j,  gc, gcv);
        return 0;
    }
    return ico_sttm_addGCD_N(id, name, evname, ev, j,  gc, gcv);
}

extern "C" int ico_sttm_addGCSNN(const char* smNm, const char* name,
                                 const char* evname, unsigned short ev,
                                 GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addGCSNN >>%s<<, %s, %s, %d, %d, %d, %s", smNm, 
             name, evname, ev, j,  gc, gcv);
        return 0;
    }
    return ico_sttm_addGCS_N(id, name, evname, ev, j,  gc, gcv);
}

/**
 * @brief callback call entryAction/exitAction/doAction
 * @param ev callback function parameter(trigger event)
 * @param sCWId callback function parameter
 * @param fnc callback function
 */
extern "C" {
    void callbackActionC(st_event* ev, int sCWId, void* vfnc)
    {
        void (*fnc)(st_event*, int) = (void (*)(st_event*, int))vfnc;
        fnc(ev, sCWId);
    }
}

/**
 * @breif callback action for c++ 
 * @param ev trigger event
 * @param state-objects the action occurred
 */
void callbackActionCPP(const CicoEvent& ev, const CicoStateCore* stt, int vec)
{
    int sz = callbackFnc.size();
    if ((1 > vec) || (sz < vec)) {
        return;
    }

    int sCWId = getObjSTT2ID(stt);
    if (0 == sCWId) {                   // state-objects outside the control
        return;                         //
    }

    st_event stev;
    stev.ev = ev.getEV();

    CicoEvent::E_TOV tov = ev.getVTG();
    if ( CicoEvent::EEvtGCInt == tov) {
        stev.uk = E_NUMBER;
        stev.u.i = ev.getGCVi();
    }
    else if (CicoEvent::EEvtGCDouble == tov) {
        stev.uk = E_DOUBLE;
        stev.u.d = ev.getGCVd();
    }
    else if (CicoEvent::EEvtGCStr == tov) {
        stev.uk = E_STRING;
        stev.u.s = ev.getGCVs().c_str();
    }
    else {
        stev.uk = E_NONE;
    }
    stev.v = ev.getAdVal();

    callbackActionC(&stev, sCWId, callbackFnc[vec-1]);
    return;
}

#ifndef CWAPPERCALLBACKACTION_H
#define CWAPPERCALLBACKACTION_H
/**
 * C Wrapper call back CicoStateAction
 */
class CicoCWrapperCallbackAction :public CicoStateAction
{
public:
    CicoCWrapperCallbackAction();
    // entry-Action
    void onEntry(const CicoEvent& ev, const CicoState* stt, int addval);
    void onEntry(const CicoEvent& ev, const CicoFinalState* stt, int addval);
    void onEntry(const CicoEvent& ev, const CicoHistoryState* stt,
                  int addval);
    // exit-Action
    void onExit(const CicoEvent& ev, const CicoState* stt, int addval);
    void onExit(const CicoEvent& ev, const CicoHistoryState* stt, int addval);

    // doAction
    void onDo(const CicoEvent& ev, const CicoState* stt, int addval);
};
#endif  // CWAPPERCALLBACKACTION_H

/**
 * @brief cnstractor
 */
CicoCWrapperCallbackAction::CicoCWrapperCallbackAction()
{
}

/**
 * @brief entry Action callback
 * @param ev trigger event data
 * @param stt entery state-object
 * @param addval Additional information
 */
void CicoCWrapperCallbackAction::onEntry(const CicoEvent& ev,
                                          const CicoState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

void CicoCWrapperCallbackAction::onEntry(const CicoEvent& ev,
                                          const CicoFinalState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

void CicoCWrapperCallbackAction::onEntry(const CicoEvent& ev,
                                          const CicoHistoryState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

/**
 * @brief exit Action callback
 * @param ev trigger event data
 * @param stt exit state-object
 * @param addval Additional information
 */
void CicoCWrapperCallbackAction::onExit(const CicoEvent& ev,
                                        const CicoState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

void CicoCWrapperCallbackAction::onExit(const CicoEvent& ev,
                                        const CicoHistoryState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

/**
 * @brief do Action callback
 * @param ev trigger event data
 * @param stt active state-object
 * @param addval Additional information
 */
void CicoCWrapperCallbackAction::onDo(const CicoEvent& ev,
                                      const CicoState* stt, int addval)
{
    callbackActionCPP(ev, (CicoStateCore*)stt, addval);
}

/**
 * Declaration of the class CicoCWrapperCallbackAction
 */
CicoCWrapperCallbackAction cWprActn;


/* registration entry-Action,exit-Action,do-action callback */
/**
 * @brief Callback registration
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:Failure
 */
extern "C" int ico_sttm_addEntryAction(int sCWId, void (*fnc)(st_event*, int))
{
    sttmerr = STTMNOERROR;
    CicoStateCore* csc = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == csc) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addEntryAction >>%d<<, fnc", sCWId);
        return 0;
    }
    if (0 == fnc) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addEntryAction %d,>>fnc(0x0)<<", sCWId);
        return 0;
    }

    callbackFnc.push_back((void*)fnc);
    int addval = callbackFnc.size();

    if (true == csc->isState()) {
        CicoState* cs = (CicoState*)csc;
        cs->addEntryAction((CicoStateAction*)&cWprActn, addval);
    }
    else if (true == csc->isFinalState()) {
        CicoFinalState* cfs = (CicoFinalState*)csc;
        cfs->addEntryAction((CicoStateAction*)&cWprActn, addval);
    }
    else if (true == csc->isHistoryState()) {
        CicoHistoryState* chs = (CicoHistoryState*)csc;
        chs->addEntryAction((CicoStateAction*)&cWprActn, addval);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_addEntryAction >>%d(%s:%d:%d)<<,fnc", sCWId,
             csc->getName().c_str(), csc->getValue(), (int)csc->getType());
        return 0;
    }
    return sCWId;
}


extern "C" int ico_sttm_addExitAction(int sCWId, void (*fnc)(st_event*, int))
{
    sttmerr = STTMNOERROR;
    CicoStateCore* csc = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == csc) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addExitAction >>%d<<, fnc", sCWId);
        return 0;
    }
    if (0 == fnc) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addExitAction %d,>>fnc(0x0)<<", sCWId);
        return 0;
    }

    callbackFnc.push_back((void*)fnc);
    int addval = callbackFnc.size();

    if (true == csc->isState()) {
        CicoState* cs = (CicoState*)csc;
        cs->addExitAction((CicoStateAction*)&cWprActn, addval);
    }
    else if (true == csc->isHistoryState()) {
        CicoHistoryState* chs = (CicoHistoryState*)csc;
        chs->addExitAction((CicoStateAction*)&cWprActn, addval);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_addExitAction >>%d(%s:%d:%d)<<,fnc", sCWId,
             csc->getName().c_str(), csc->getValue(), (int)csc->getType());
        return 0;
    }
    return sCWId;
}


extern "C" int ico_sttm_addDoAction(int sCWId, void (*fnc)(st_event*, int))
{
    sttmerr = STTMNOERROR;
    CicoStateCore* csc = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == csc) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDoAction >>%d<<, fnc", sCWId);
        return 0;
    }
    if (0 == fnc) {
        sttmerr = STTMPARAMERR;
        _DBG("ico_sttm_addDoAction %d,>>fnc(0x0)<<", sCWId);
        return 0;
    }

    callbackFnc.push_back((void*)fnc);
    int addval = callbackFnc.size();

    if (true == csc->isState()) {
        CicoState* cs = (CicoState*)csc;
        cs->addDoAction((CicoStateAction*)&cWprActn, addval);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_addDoAction >>%d(%s:%d:%d)<<,fnc", sCWId,
             csc->getName().c_str(), csc->getValue(), (int)csc->getType());
        return 0;
    }
    return sCWId;
}

/**
 * @brief Callback registration
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
extern "C" int ico_sttm_addEntryAction_V(int smCWId, int value,
                                         void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int lid = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addEntryAction_V >>%d<<,>>%d<<, .", smCWId, value);
        return 0;
    }
    return ico_sttm_addEntryAction(lid, fnc);
}

extern "C" int ico_sttm_addExitAction_V(int smCWId, int value,
                                        void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int lid = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addExitAction_V >>%d<<,>>%d<<, .", smCWId, value);
        return 0;
    }
    return ico_sttm_addExitAction(lid, fnc);
}

extern "C" int ico_sttm_addDoAction_V(int smCWId, int value,
                                      void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int lid = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDoAction_V >>%d<<,>>%d<<, .", smCWId, value);
        return 0;
    }
    return ico_sttm_addDoAction(lid, fnc);
}

/**
 * @brief Callback registration
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
extern "C" int ico_sttm_addEntryActionVV(int smVal, int value,
                                         void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addEntryActionVV >>%d<<, %d, .", smVal, value);
        return 0;
    }
    return ico_sttm_addEntryAction_V(id, value, fnc);
}

extern "C" int ico_sttm_addExitActionVV(int smVal, int value,
                                        void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addExitActionVV >>%d<<, %d, .", smVal, value);
        return 0;
    }
    return ico_sttm_addExitAction_V(id, value, fnc);
}

extern "C" int ico_sttm_addDoActionVV(int smVal, int value,
                                      void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDoActionVV >>%d<<, %d, .", smVal, value);
        return 0;
    }
    return ico_sttm_addDoAction_V(id, value, fnc);
}


/**
 * @brief Callback registration
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
extern "C" int ico_sttm_addEntryAction_N(int smCWId, const char* name,
                                         void (*fnc)(st_event* ev, int sCWId))
{
    int lid = ico_sttm_retrivevSName(smCWId, name);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addEntryAction_N >>%d<<,>>%s<<, .", smCWId, name);
        return 0;
    }
    return ico_sttm_addEntryAction(lid, fnc);
}

extern "C" int ico_sttm_addExitAction_N(int smCWId, const char* name,
                                        void (*fnc)(st_event* ev, int sCWId))
{
    int lid = ico_sttm_retrivevSName(smCWId, name);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addExitAction_N >>%d<<,>>%s<<, .", smCWId, name);
        return 0;
    }
    return ico_sttm_addExitAction(lid, fnc);
}

extern "C" int ico_sttm_addDoAction_N(int smCWId, const char* name,
                                      void (*fnc)(st_event* ev, int sCWId))
{
    int lid = ico_sttm_retrivevSName(smCWId, name);
    if (0 == lid) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDoAction_N >>%d<<,>>%s<<, .", smCWId, name);
        return 0;
    }
    return ico_sttm_addDoAction(lid, fnc);
}

/**
 * @brief Callback registration
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
extern "C" int ico_sttm_addEntryActionNN(const char* smNm, const char* name,
                                         void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addEntryActionNN >>%s<<, %s, .", smNm, name);
        return 0;
    }
    return ico_sttm_addEntryAction_N(id, name, fnc);
}

extern "C" int ico_sttm_addExitActionNN(const char* smNm, const char* name,
                                        void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addExitActionNN >>%s<<, %s, .", smNm, name);
        return 0;
    }
    return ico_sttm_addExitAction_N(id, name, fnc);
}

extern "C" int ico_sttm_addDoActionNN(const char* smNm, const char* name,
                                      void (*fnc)(st_event* ev, int sCWId))
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDoActionNN >>%s<<, %s, .", smNm, name);
        return 0;
    }
    return ico_sttm_addDoAction_N(id, name, fnc);
}


/**
 * @brief Registration at the time of termination detection transition destination
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param trCWId finish transition destination(c-wrapper local ID)
 * @return not 0:Success
 * @       0:set Failure
 */
extern "C" int ico_sttm_setFinishTransition(int sCWId, int trCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransition >>%d<<, %d", sCWId, trCWId);
        return 0;
    }
    CicoStateCore* cscTrans = getID2ObjSTT(trCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTrans) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransition %d,>>%d<<", sCWId, trCWId);
        return 0;
    }
    if (false == cscTarget->isState()) {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_setFinishTransition >>%d(%s:%d:%d)<<, %d", sCWId,
             cscTarget->getName().c_str(), cscTarget->getValue(),
             (int)cscTarget->getType(), trCWId);
        return 0;
    }
    CicoState* cs = (CicoState*) cscTarget;
    if (true == cscTrans->isState()) {
        cs->setFinishTransition((CicoState*)cscTrans);
    }
    else if (true == cscTrans->isFinalState()) {
        cs->setFinishTransition((CicoFinalState*)cscTrans);
    }
    else if (true == cscTrans->isHistoryState()) {
        cs->setFinishTransition((CicoHistoryState*)cscTrans);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_setFinishTransition %d,>>%d(%s:%d:%d)<<", sCWId,
             trCWId, cscTarget->getName().c_str(),
             cscTarget->getValue(), (int)cscTarget->getType());
        return 0;
    }
    return sCWId;
}

/**
 * @brief Registration at the time of termination detection transition destination
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param trCWId finish transition destination(c-wrapper local ID)
 * @param trVl value is state-object identification number transition destination
 * @return not 0:Success
 * @       0:set Failure
 */
extern "C" int ico_sttm_setFinishTransition_V_(int smCWId, int value,
                                               int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        _DBG("ico_sttm_setFinishTransition_V_ >> %d, %d<<, %d",smCWId, value, trCWId);
        return 0;
    }
    return ico_sttm_setFinishTransition(id, trCWId);
}

extern "C" int ico_sttm_setFinishTransition_VV(int smCWId, int value,
                                               int trVl)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrieveSVal(smCWId, trVl);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransition_VV >>%d<<, %d,>>%d<<",smCWId, value, trVl);
        return 0;
    }
    return ico_sttm_setFinishTransition_V_(smCWId, value, idT);
}

extern "C" int ico_sttm_setFinishTransitionVVV(int smVal, int value, int trVl)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransitionVVV >>%d<<, %d, %d", smVal,
             value, trVl);
        return 0;
    }
    return ico_sttm_setFinishTransition_VV(id, value, trVl);
}


/**
 * @brief Registration at the time of termination detection transition destination
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param trCWId finish transition destination(c-wrapper local ID)
 * @return not 0:Success
 * @       0:set Failure
 */
extern "C" int ico_sttm_setFinishTransition_N_(int smCWId, const char* name,
                                               int trCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransition_N_ >>%d, %s<<, %d",smCWId, name, trCWId);
        return 0;
    }
    return ico_sttm_setFinishTransition(id, trCWId);
}

extern "C" int ico_sttm_setFinishTransition_NN(int smCWId, const char* name,
                                               const char* trNm)
{
    sttmerr = STTMNOERROR;
    int idT = ico_sttm_retrivevSName(smCWId, trNm);
    if (0 == idT) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransition_NN >>%d<<, %s,>>%s<<",smCWId,
             name, trNm);
        return 0;
    }
    return ico_sttm_setFinishTransition_N_(smCWId, name, idT);
}

extern "C" int ico_sttm_setFinishTransitionNNN(const char* smNm,
                                               const char* name,
                                               const char* trNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setFinishTransitionNNN >>%s<<, %s, %s", smNm, name,
             trNm);
        return 0;
    }
    return ico_sttm_setFinishTransition_NN(id, name, trNm);

}


/**
 * @brief Registration of default history state-objectss
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param dfCWId  default transition destination(c-wrapper local ID)
 * @return not 0:Success
 * @       0:added Failure
 */
extern "C" int ico_sttm_addDefault(int sCWId, int dfCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefault >>%d<<, %d", sCWId, dfCWId);
        return 0;
    }
    CicoStateCore* cscDef = getID2ObjSTT(dfCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscDef) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefault %d,>>%d<<", sCWId, dfCWId);
        return 0;
    }
    if (false == cscTarget->isHistoryState()) {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_addDefault >>%d(%s:%d:%d)<<, %d", sCWId,
             cscTarget->getName().c_str(), cscTarget->getValue(),
             (int)cscTarget->getType(), dfCWId);
        return 0;
    }
    CicoHistoryState* chs = (CicoHistoryState*) cscTarget;
    if (true == cscDef->isState()) {
        chs->addDefaultState((CicoState*)cscDef);
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_addDefault %d,>>%d(%s:%d:%d)<<", sCWId,
             dfCWId, cscTarget->getName().c_str(),
             cscTarget->getValue(), (int)cscTarget->getType());
        return 0;
    }
    return sCWId;
}

/**
 * @brief Registration of default history state-objectss
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param dfCWId  default transition destination(c-wrapper local ID)
 * @param dfVal value is state-object identification number
 * @return not 0:Success
 * @       0:added Failure
 */
extern "C" int ico_sttm_addDefault_V_(int smCWId, int value, int dfCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefaultV >>%d, %d<<, %d", smCWId, value, dfCWId);
        return 0;
    }
    return ico_sttm_addDefault(id, dfCWId);
}

extern "C" int ico_sttm_addDefault_VV(int smCWId, int value, int dfVal)
{
    sttmerr = STTMNOERROR;
    int dfCWId = ico_sttm_retrieveSVal(smCWId, dfVal);
    if (0 == dfCWId) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefaultVV >>%d<<, %d,>>%d<<", smCWId, value, dfVal);
        return 0;
    }
    return ico_sttm_addDefault_V_(smCWId, value, dfCWId);
}

extern "C" int ico_sttm_addDefaultVVV(int smVal, int value, int dfVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefaultVVV >>%d<<, %d,>>%d<<", smVal, value,
             dfVal);
        return 0;
    }
    return ico_sttm_addDefault_VV(id, value, dfVal);
}

/**
 * @brief Registration of default history state-objectss
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param dfCWId  default transition destination(c-wrapper local ID)
 * @param dfNm  name is state-object Identification name
 * @return not 0:Success
 * @       0:added Failure
 */
extern "C" int ico_sttm_addDefault_N_(int smCWId, const char* name, int dfCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefault_N_ >>%d, %s<<, %d",smCWId, name, dfCWId);
        return 0;
    }
    return ico_sttm_addDefault(id, dfCWId);
}

extern "C" int ico_sttm_addDefault_NN(int smCWId, const char* name, const char* dfNm)
{
    sttmerr = STTMNOERROR;
    int idD = ico_sttm_retrivevSName(smCWId, dfNm);
    if (0 == idD) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefault_NN >>%d<<, %s,>>%s<<", smCWId, name, dfNm);
        return 0;
    }
    return ico_sttm_addDefault_N_(smCWId, name, idD);
}

extern "C" int ico_sttm_addDefaultNNN(const char* smNm, const char* name,
                                      const char* dfNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addDefaultNNN >>%s<<, %s, %s", smNm, name, dfNm);
        return 0;
    }
    return ico_sttm_addDefault_NN(id, name, dfNm);
}

/**
 * @breif Registration of start position state-objects
 * @param ID stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @param sCWId start position state-object ID no.(c-wrapper local ID)
 * @return not 0:Success
 * @       0:add failure
 */
extern "C" int ico_sttm_addStartPosition(int CWId, int sCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(CWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setStartPosition >>%d<<, %d", CWId, sCWId);
        return 0;
    }
    CicoStateCore* cscIni = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscIni) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_setStartPosition %d,>>%d<<", CWId, sCWId);
        return 0;
    }
    CicoState* cs = (CicoState*)0;
    CicoHistoryState* chs = (CicoHistoryState*)0;
    if (true == cscIni->isState()) {
        cs = (CicoState*)cscIni;
    }
    else if (true == cscIni->isHistoryState()) {
        chs = (CicoHistoryState*)cscIni;
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_setStartPosition %d,>>%d(%s:%d:%d)<<", CWId, sCWId,
             cscIni->getName().c_str(), cscIni->getValue(),
             (int)cscIni->getType());
        return 0;
    }
    if (true == cscTarget->isStateMachine()) {
        CicoStateMachine* csm = (CicoStateMachine*)cscTarget;
        if ((CicoState*)0 != cs) {
            csm->addInitState(cs);
        }
        else {
            csm->addInitState(chs);
        }
    }
    else if (true == cscTarget->isState()) {
        CicoState* csT = (CicoState*)cscTarget;
        if ((CicoState*)0 != cs) {
            csT->addInitState(cs);
        }
        else {
            csT->addInitState(chs);
        }
    }
    else {
        sttmerr = STTMTYPEMISMATCH;
        _DBG("ico_sttm_setStartPosition >>%d(%s:%d:%d)<<, %d", CWId,
             cscTarget->getName().c_str(), cscTarget->getValue(),
             (int)cscTarget->getType(), sCWId);
        return 0;
    }
    return CWId;
}



/**
 * @breif Registration of start position state-objects
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param sCWId start position state-object ID no.(c-wrapper local ID)
 * @param value spVal is state-object identification number
 * @return not 0:Success
 * @       0:set failure
 */
extern "C" int ico_sttm_addStartPosition_V_(int smCWId, int value, int sCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveSVal(smCWId, value);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPosition_V_ >>%d, %d<<, %d",smCWId, value, sCWId);
        return 0;
    }
    return ico_sttm_addStartPosition(id, sCWId);
}


extern "C" int ico_sttm_addStartPosition_VV(int smCWId, const int value, int spVal)
{
    sttmerr = STTMNOERROR;
    int idSP = ico_sttm_retrieveSVal(smCWId, spVal);
    if (0 == idSP) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPosition_VV >>%d<<, %d,>>%d<<",smCWId, value, spVal);
        return 0;
    }
    return ico_sttm_addStartPosition_V_(smCWId, value, idSP);
}

extern "C" int ico_sttm_addStartPositionVVV(int smVal, int value, int spVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionVVV >>%d<<, %d, %d", smVal, value,
             spVal);
        return 0;
    }
    return ico_sttm_addStartPosition_VV(id, value, spVal);
}

extern "C" int ico_sttm_addStartPositionSMVV(int smVal, int spVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionSMVV >>%d<<, %d", smVal, spVal);
        return 0;
    }
    int idSP = ico_sttm_retrieveSVal(id, spVal);
    if (0 == idSP) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionSMVV %d,>>%d<<",smVal, spVal);
        return 0;
    }
    return ico_sttm_addStartPosition(id, idSP);
}


/**
 * @breif Registration of start position state-objects
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param name  name is state-object Identification name
 * @param sCWId start position state-object ID no.(c-wrapper local ID)
 * @param name  spNm is state-object Identification name
 * @return not 0:Success
 * @       0:set failure
 */
extern "C" int ico_sttm_addStartPosition_N_(int smCWId, const char* name, int sCWId)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrivevSName(smCWId, name);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPosition_N_ >>%d, %s<<, %d",smCWId, name, sCWId);
        return 0;
    }
    return ico_sttm_addStartPosition(id, sCWId);
}

extern "C" int ico_sttm_addStartPosition_NN(int smCWId, const char* name,
                                            const char*  spNm)
{
    sttmerr = STTMNOERROR;
    int idSP = ico_sttm_retrivevSName(smCWId, spNm);
    if (0 == idSP) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPosition_NN >>%d<<, %s,>>%s<<",smCWId, name,
             spNm);
        return 0;
    }
    return ico_sttm_addStartPosition_N_(smCWId, name, idSP);
}

extern "C" int ico_sttm_addStartPositionNNN(const char* smNm, const char* name,
                                            const char* spNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionNNN >>%s<<, %s, %s", smNm, name,
             spNm);
        return 0;
    }
    return ico_sttm_addStartPosition_NN(id, name, spNm);
}

extern "C" int ico_sttm_addStartPositionSMNN(const char* smNm, const char* spNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionSMNN >>%s<<, %s", smNm, spNm);
        return 0;
    }
    int idSP = ico_sttm_retrivevSName(id, spNm);
    if (0 == idSP) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_addStartPositionSMNN %s,>>%s<<",smNm, spNm);
        return 0;
    }
    return ico_sttm_addStartPosition(id, idSP);
}


/* Operational system */
/**
 * @breif start stateMachine
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @return not 0:Success
 * @       0:start Failure
 */
extern "C" int ico_sttm_start(int smCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateMachine* csm = ico_sttm_getStateMachineX(smCWId, "ico_sttm_start");
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    return csm->start()? smCWId:0;
}



extern "C" int ico_sttm_startV(int smVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_startV >>%d<<", smVal);
        return 0;
    }
    return ico_sttm_start(id);
}

extern "C" int ico_sttm_startN(const char* smNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_startN >>%s<<", smNm);
        return 0;
    }
    return ico_sttm_start(id);
}

/**
 * @breif stop stateMachine
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @return not 0:Success
 * @       0:stop Failure
 */
extern "C" int ico_sttm_stop(int smCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateMachine* csm = ico_sttm_getStateMachineX(smCWId, "ico_sttm_stop");
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    return csm->stop()? smCWId:0;
}

extern "C" int ico_sttm_stopV(int smVal)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_stopV >>%d<<", smVal);
        return 0;
    }
    return ico_sttm_stop(id);
}

extern "C" int ico_sttm_stopN(const char* smNm)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_stopN >>%s<<", smNm);
        return 0;
    }
    return ico_sttm_stop(id);
}

bool ico_sttm_eventX(int smCWId, CicoEvent& ev, const char* fname)
{
    CicoStateMachine* csm = ico_sttm_getStateMachineX(smCWId, fname);
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    return csm->eventEntry(ev);
}

/**
 * @brief Issue an event
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @param ev event No.(Use the 1000-65535)
 * @param gcv Comparison value of guard condition
 * @param info Additional information user
 * @return not 0:Success on Transition
 * @       0: none Transition
 */
extern "C" int ico_sttm_event(int smCWId, unsigned short ev, void* info)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, info);
    return ico_sttm_eventX(smCWId, oev, "ico_sttm_event")? smCWId:0;
}

extern "C" int ico_sttm_eventGCI(int smCWId, unsigned short ev, int gcv, void* info)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv, info);
    return ico_sttm_eventX(smCWId, oev, "ico_sttm_eventGCI")? smCWId:0;
}

extern "C" int ico_sttm_eventGCD(int smCWId, unsigned short ev, double gcv, void* info)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv, info);
    return ico_sttm_eventX(smCWId, oev, "ico_sttm_eventGCD")? smCWId:0;
}

extern "C" int ico_sttm_eventGCS(int smCWId, unsigned short ev, const char* gcv, void* info)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv, info);
    return ico_sttm_eventX(smCWId, oev, "ico_sttm_eventGCS")? smCWId:0;
}

extern "C" int ico_sttm_eventV(int smVal, unsigned short ev, void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventV >>%d<<, %d, .", smVal, ev);
        return 0;
    }
    return ico_sttm_event(id, ev, info);
}

extern "C" int ico_sttm_eventGCIV(int smVal, unsigned short ev, int gcv,
                                  void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCIV >>%d<<, %d, %d, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCI(id, ev, gcv, info);
}

extern "C" int ico_sttm_eventGCDV(int smVal, unsigned short ev, double gcv,
                                  void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCDV >>%d<<, %d, %f, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCD(id, ev, gcv, info);
}

extern "C" int ico_sttm_eventGCSV(int smVal, unsigned short ev,
                                  const char* gcv, void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCSV >>%d<<, %d, %s, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCS(id, ev, gcv, info);
}

extern "C" int ico_sttm_eventN(const char* smNm, unsigned short ev, void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventN >>%s<<, %d, .", smNm, ev);
        return 0;
    }
    return ico_sttm_event(id, ev, info);
}

extern "C" int ico_sttm_eventGCIN(const char* smNm, unsigned short ev, int gcv,
                                  void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCIN >>%s<<, %d, %d, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCI(id, ev, gcv, info);
}

extern "C" int ico_sttm_eventGCDN(const char* smNm, unsigned short ev, double gcv,
                                  void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCDN >>%s<<, %d, %f, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCD(id, ev, gcv, info);
}

extern "C" int ico_sttm_eventGCSN(const char* smNm, unsigned short ev, const char* gcv,
                       void* info)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCSN >>%s<<, %d, %s, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventGCS(id, ev, gcv, info);
}



bool ico_sttm_eventTestX(int smCWId, CicoEvent& ev, const char* fname)
{
    CicoStateMachine* csm = ico_sttm_getStateMachineX(smCWId, fname);
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    return csm->eventTest(ev);
}

/**
 * @brief Test of the transition
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @param ev event No.(Use the 1000-65535)
 * @param gcv Comparison value of guard condition
 * @param info Additional information user
 * @return not 0:Success on Transition
 * @       0: none Transition
 */
extern "C" int ico_sttm_eventTest(int smCWId, unsigned short ev)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev);
    return ico_sttm_eventTestX(smCWId, oev, "ico_sttm_eventTest")? smCWId:0;
}

extern "C" int ico_sttm_eventTestGCI(int smCWId, unsigned short ev, int gcv)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv);
    return ico_sttm_eventTestX(smCWId, oev, "ico_sttm_eventTestGCI")? smCWId:0;
}

extern "C" int ico_sttm_eventTestGCD(int smCWId, unsigned short ev, double gcv)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv);
    return ico_sttm_eventTestX(smCWId, oev, "ico_sttm_eventTestGCD")? smCWId:0;
}

extern "C" int ico_sttm_eventTestGCS(int smCWId, unsigned short ev, const char* gcv)
{
    sttmerr = STTMNOERROR;
    CicoEvent oev(ev, gcv);
    return ico_sttm_eventTestX(smCWId, oev, "ico_sttm_eventTestGCS")? smCWId:0;
}

extern "C" int ico_sttm_eventTestV(int smVal, unsigned short ev)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestV >>%d<<, %d, .", smVal, ev);
        return 0;
    }
    return ico_sttm_eventTest(id, ev);
}

extern "C" int ico_sttm_eventTestGCIV(int smVal, unsigned short ev, int gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventGCIV >>%d<<, %d, %d, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCI(id, ev, gcv);
}

extern "C" int ico_sttm_eventTestGCDV(int smVal, unsigned short ev, double gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestGCDV >>%d<<, %d, %f, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCD(id, ev, gcv);
}

extern "C" int ico_sttm_eventTestGCSV(int smVal, unsigned short ev, const char* gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestGCSV >>%d<<, %d, %s, .", smVal, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCS(id, ev, gcv);
}

extern "C" int ico_sttm_eventTestN(const char* smNm, unsigned short ev)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestN >>%s<<, %d, .", smNm, ev);
        return 0;
    }
    return ico_sttm_eventTest(id, ev);
}

extern "C" int ico_sttm_eventTestGCIN(const char* smNm, unsigned short ev, int gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestGCIN >>%s<<, %d, %d, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCI(id, ev, gcv);
}

extern "C" int ico_sttm_eventTestGCDN(const char* smNm, unsigned short ev, double gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestGCDN >>%s<<, %d, %f, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCD(id, ev, gcv);
}

extern "C" int ico_sttm_eventTestGCSN(const char* smNm, unsigned short ev,
                           const char* gcv)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_eventTestGCSN >>%s<<, %d, %s, .", smNm, ev, gcv);
        return 0;
    }
    return ico_sttm_eventTestGCS(id, ev, gcv);
}

/**
 * @brief retrieve stateMachine-object
 * @param value/name  stateMachine-object ID no. / name
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:not find
 */
extern "C" int ico_sttm_retrieveMVal(int value)
{
    sttmerr = STTMNOERROR;
    int r = 0;
    int sz = stateObjects.size();
    for (int i = 0; i< sz; i++) {
        if (true == stateObjects[i]->isStateMachine()) {
            if (value == stateObjects[i]->getValue() ) {
                r = i+1;
                break;                  // break of for i<sz
            }
        }
    }
    return r;
}

extern "C" int ico_sttm_retrieveMName(const char* name)
{
    sttmerr = STTMNOERROR;
    std::string oname(name);
    int r = 0;
    int sz = stateObjects.size();
    for (int i = 0; i< sz; i++) {
        if (true == stateObjects[i]->isStateMachine()) {
            if (oname == stateObjects[i]->getName()) {
                r = i+1;
                break;                  // break of for i<sz
            }
        }
    }
    return r;
}

/**
 * @brief retrieve state-object(state/historyState/finalState)
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value/name  state(state/historyState/finalState)-object ID no. / name
 * @return not 0:state-object ID no.(c-wrapper local ID)
 * @       0:not find
 */
extern "C" int ico_sttm_retrieveSVal(int smCWId, int value)
{
    sttmerr = STTMNOERROR;
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, "ico_sttm_retrieveSVal");
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    const CicoStateCore* csc = csm->getState(value);
    if ((CicoStateCore*)0 == csc) {
        _DBG("ico_sttm_retrieveSVal %d, >>%d<<", smCWId, value);
        return 0;
    }
    return getObjSTT2ID(csc);
}

extern "C" int ico_sttm_retrivevSName(int smCWId, const char* name)
{
    sttmerr = STTMNOERROR;
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, "ico_sttm_retrivevSName");
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    const CicoStateCore* csc = csm->getState(name);
    if ((CicoStateCore*)0 == csc) {
        _DBG("ico_sttm_retrivevSName %d, >>%s<<", smCWId, name);
        return 0;
    }
    return getObjSTT2ID(csc);
}


/**
 * @brief object check
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @return not 0:stateMachine-object
 * @           0:no stateMachine-object
 */
extern "C" int ico_sttm_isStateMachine(int smCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(smCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isStateMachine >>%d<<", smCWId);
        return 0;
    }
    return cscTarget->isStateMachine()? smCWId:0;
}

/**
 * @brief object check
 * @param sCWId state-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
extern "C" int ico_sttm_isState(int sCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isState >>%d<<", sCWId);
        return 0;
    }
    return cscTarget->isState()? sCWId:0;
}


/**
 * @brief object check
 * @param sCWId historyState-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
extern "C" int ico_sttm_isHistoryState(int sCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isHistoryState >>%d<<", sCWId);
        return 0;
    }
    return cscTarget->isHistoryState()? sCWId:0;
}


/**
 * @brief object check
 * @param sCWId finalState-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
extern "C" int ico_sttm_isFinalState(int sCWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(sCWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isFinalState >>%d<<", sCWId);
        return 0;
    }
    return cscTarget->isFinalState()? sCWId:0;
}

/**
 * @brief get state-object running
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param X[] Enter the active state-objects(c-wrapper local ID) or value or name
 * @param size of array sCWIds or values or names
 * @param value true:get value false:get c-wrapper ID
 * @param p top or all or bottom get position is getCurrentState parameter
 * @param fname call function name
 * @return Number of registered X
 * @       -1:size over
 */
int ico_sttm_getActiveStateX(int smCWId, int X[], size_t arysz, bool value,
                             CicoStateCore::E_GetPolicy p, const char* fname)
{
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, fname);
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    std::vector<const CicoState*> stt;
    if (false == csm->getCurrentState(stt, p)) {
        _DBG("%s NO ACTIVE STATEMACHINE(\"%s\":%d)",fname,
             csm->getName().c_str(), csm->getValue());
        return 0;
    }
    int r = 0;
    int sz = stt.size();
    for (int i = 0; i < sz; i++) {
        if ( r >= (int)arysz ) {
            r = -1;
            break;                      // break of for i<sz
        }
        if (true == value) {
            X[r] = stt[i]->getValue();  // set state-object value
            r++;
        }
        else {
            X[r] = getObjSTT2ID(stt[i]); // set state-object C-wrapper local ID
            if (0 != X[r]) {
                r++;
            }
        }
    }
    if (0 == r) {
        sttmerr = STTMNOTFIND;
        _DBG("%s get size %d -> %d",fname, sz, r);
        return 0;
    }
    return r;
}
int ico_sttm_getActiveState_NX(int smCWId, const char* X[], size_t arysz,
                             CicoStateCore::E_GetPolicy p, const char* fname)
{
    CicoStateMachine* csm;
    csm = ico_sttm_getStateMachineX(smCWId, fname);
    if ((CicoStateMachine*)0 == csm) {
        return 0;
    }
    std::vector<const CicoState*> stt;
    if (false == csm->getCurrentState(stt, p)) {
        _DBG("%s NO ACTIVE STATEMACHINE(\"%s\":%d)",fname,
             csm->getName().c_str(), csm->getValue());
        return 0;
    }
    int r = 0;
    int sz = stt.size();
    for (int i = 0; i < sz; i++) {
        if ( r >= (int)arysz ) {
            r = -1;
            break;                      // break of for i<sz
        }
        X[r] = stt[i]->getName().c_str(); // set state-object name
        r++;
    }
    if (0 == r) {
        sttmerr = STTMNOTFIND;
        _DBG("%s get size %d -> %d",fname, sz, r);
        return 0;
    }
    return r;
}


/**
 * @brief get state-object running true
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param sCWIds[] Enter the active state-objects(c-wrapper local ID)
 * @param values[] Enter the active state-objects value
 * @param names[] Enter the active state-objects names
 * @param size of array sCWIds or values or names
 * @return Number of registered sCWIds
 * @       -1:size over
 */
extern "C" int ico_sttm_getActiveState(int smCWId, int sCWIds[], size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, sCWIds, sz, false,
                                    CicoStateCore::ELvlBttm,
                                    "ico_sttm_getActiveState");
}

extern "C" int ico_sttm_getActiveState_V(int smCWId, int values[], size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, values, sz, true,
                                    CicoStateCore::ELvlBttm,
                                    "ico_sttm_getActiveState_V");
}

extern "C" int ico_sttm_getActiveStateVV(int smVal, int values[], size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateVV >>%d<<, ., %d", smVal, sz);
        return 0;
    }
    return ico_sttm_getActiveStateX(id, values, sz, true,
                                    CicoStateCore::ELvlBttm,
                                    "ico_sttm_getActiveStateVV");
}

extern "C" int ico_sttm_getActiveState_N(int smCWId, const char* names[],
                                           size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveState_NX(smCWId, names, sz,
                                    CicoStateCore::ELvlBttm,
                                    "ico_sttm_getActiveState_N");
}

extern "C" int ico_sttm_getActiveStateNN(const char* smNm, const char* names[],
                                         size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateNN >>%s<<, ., %d", smNm, sz);
        return 0;
    }
    return ico_sttm_getActiveState_NX(id, names, sz,
                                    CicoStateCore::ELvlBttm,
                                    "ico_sttm_getActiveStateNN");
}

/**
 * @brief get state-object running directly under
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param sCWIds[] Enter the active state-objects(c-wrapper local ID)
 * @param values[] Enter the active state-objects value
 * @param names[] Enter the active state-objects names
 * @param size of array sCWIds or values or names
 * @return Number of registered sCWIds
 * @       -1:size over
 */
extern "C" int ico_sttm_getActiveStateDirectlyUnder(int smCWId, int sCWIds[],
                                                    size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, sCWIds, sz, false,
                                    CicoStateCore::ELvlTop,
                                    "ico_sttm_getActiveStateDirectlyUnder");
}
extern "C" int ico_sttm_getActiveStateDirectlyUnder_V(int smCWId, 
                                                         int values[],
                                                         size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, values, sz, true,
                                    CicoStateCore::ELvlTop,
                                    "ico_sttm_getActiveStateDirectlyUnder_V");
}

extern "C" int ico_sttm_getActiveStateDirectlyUnderVV(int smVal, int values[],
                                           size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateDirectlyUnderVV >>%d<<, ., %d",
             smVal, sz);
        return 0;
    }
    return ico_sttm_getActiveStateX(id, values, sz, true,
                                    CicoStateCore::ELvlTop,
                                    "ico_sttm_getActiveStateDirectlyUnderVV");
}

extern "C" int ico_sttm_getActiveStateDirectlyUnder_N(int smCWId,
                                                        const char* names[],
                                                        size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveState_NX(smCWId, names, sz,
                                    CicoStateCore::ELvlTop,
                                    "ico_sttm_getActiveStateDirectlyUnder_N");
}

extern "C" int ico_sttm_getActiveStateDirectlyUnderNN(const char* smNm, 
                                                      const char* names[],
                                                      size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateDirectlyUnderNN >>%s<<, ., %d", smNm, sz);
        return 0;
    }
    return ico_sttm_getActiveState_NX(id, names, sz,
                                    CicoStateCore::ELvlTop,
                                    "ico_sttm_getActiveStateDirectlyUnderNN");
}

/**
 * @brief get state-object running all
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @param sCWIds[] Enter the active state-objects(c-wrapper local ID)
 * @param values[] Enter the active state-objects value
 * @param names[] Enter the active state-objects names
 * @param size of array sCWIds or values or names
 * @return Number of registered sCWIds
 * @       -1:size over
 */
extern "C" int ico_sttm_getActiveStateAll(int smCWId, int sCWIds[], size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, sCWIds, sz, false,
                                    CicoStateCore::ELvlAll,
                                    "ico_sttm_getActiveStateAll");
}

extern "C" int ico_sttm_getActiveStateAll_V(int smCWId, int values[],
                                            size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveStateX(smCWId, values, sz, true,
                                    CicoStateCore::ELvlAll,
                                    "ico_sttm_getActiveStateAll_V");
}

extern "C" int ico_sttm_getActiveStateAllVV(int smVal, int values[],
                                            size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateAllVV >>%d<<, ., %d", smVal, sz);
        return 0;
    }
    return ico_sttm_getActiveStateX(id, values, sz, true,
                                    CicoStateCore::ELvlAll,
                                    "ico_sttm_getActiveStateAllVV");
}

extern "C" int ico_sttm_getActiveStateAll_N(int smCWId, const char* names[],
                                              size_t sz)
{
    sttmerr = STTMNOERROR;
    return ico_sttm_getActiveState_NX(smCWId, names, sz,
                                    CicoStateCore::ELvlAll,
                                    "ico_sttm_getActiveStateAll_N");
}

extern "C" int ico_sttm_getActiveStateAllNN(const char* smNm,
                                            const char* names[], size_t sz)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getActiveStateAllNN >>%s<<, ., %d", smNm, sz);
        return 0;
    }
    return ico_sttm_getActiveState_NX(id, names, sz,
                                    CicoStateCore::ELvlAll,
                                    "ico_sttm_getActiveStateAllNN");
}

/**
 * @brief get name value
 * @param CWId stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @return name
 */
extern "C" const char* ico_sttm_getName(int CWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(CWId); // CWId to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getName >>%d<<", CWId);
        return 0;
    }
    return cscTarget->getName().c_str();
}

/**
 * @brief get value
 * @param CWId stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @return value
 */
extern "C" int ico_sttm_getValue(int CWId)
{
    sttmerr = STTMNOERROR;
    CicoStateCore* cscTarget = getID2ObjSTT(CWId); // ID to state-object
    if ((CicoStateCore*)0 == cscTarget) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_getValue >>%d<<", CWId);
        return 0;
    }
    return cscTarget->getValue();
}

/**
 * @brief get error code
 * @return error code
 */
StateMachineCWrapperErr_E ico_sttm_errcode()
{
    return sttmerr;
}

/**
 * @breif get operation state
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param value value is state-object identification number
 * @param name  name is state-object Identification name
 * @return OPERATION_STATE_E
 */
extern "C" OPERATION_STATE_E ico_sttm_isActiveSttM(int smCWId)
{
    return ico_sttm_isActiveStt(smCWId);
}

/**
 *
 */
extern "C" OPERATION_STATE_E ico_sttm_isActiveStt(int sCWId)
{
    sttmerr = STTMNOERROR;
    int sz = stateObjects.size();
    if ((0 < sCWId) && (sCWId <= sz)) {
        if (true == stateObjects[sCWId-1]->isActive()) {
            return E_STT_ACTIVE;
        }
    }
    else {
        sttmerr = STTMPARAMERR;
    }
    return E_STT_INACTIVE;
}

extern "C" OPERATION_STATE_E ico_sttm_isActive_V(int smCWId, int value)
{
    sttmerr = STTMNOERROR;
    int lid = ico_sttm_retrieveSVal(smCWId, value);
    if (0 != lid) {
        return ico_sttm_isActiveStt(lid);
    }
    else {
        sttmerr = STTMNOTFIND;
    }
    return E_STT_INACTIVE;
}

extern "C" OPERATION_STATE_E ico_sttm_isActiveVV(int smVal, int value)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMVal(smVal);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isActiveVV >>%d<<, %d", smVal, value);
        return E_STT_INACTIVE;
    }
    return ico_sttm_isActive_V(id, value);
}

extern "C" OPERATION_STATE_E ico_sttm_isActive_N(int smCWId, const char* name)
{
    sttmerr = STTMNOERROR;
    int lid = ico_sttm_retrivevSName(smCWId, name);
    if (0 != lid) {
        return ico_sttm_isActiveStt(lid);
    }
    else {
        sttmerr = STTMNOTFIND;
    }
    return E_STT_INACTIVE;
}

extern "C" OPERATION_STATE_E ico_sttm_isActiveNN(const char* smNm,
                                                 const char* name)
{
    sttmerr = STTMNOERROR;
    int id = ico_sttm_retrieveMName(smNm);
    if (0 == id) {
        sttmerr = STTMNOTFIND;
        _DBG("ico_sttm_isActiveNN >>%s<<, %s", smNm, name);
        return E_STT_INACTIVE;
    }
    return ico_sttm_isActive_N(id, name);
}
