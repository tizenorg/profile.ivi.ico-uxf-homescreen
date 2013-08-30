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
#ifndef __ICO_STATEMACHIN_CWRAPPER_H__
#define __ICO_STATEMACHIN_CWRAPPER_H__

/**
 * stateMachine-c++ objects to  C
 * C Wrapper functions
 * @author m.kamoshida
 * @version 0.1
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * STATE MACHINE C-WAPPER ERROR CODE
 * 
 */
typedef enum E_StateMachineCWrapperErr {
    STTMNOERROR = 0,            // No error
    STTMNOTFIND = 1,            // state-object not find
    STTMTYPEMISMATCH,           // state-object type mismatch
    STTMCREATEERR,              // CREATE ERROR
    STTMNEWERR,                 // new ERROR
    STTMEVNOTFIND,              // eventinfo-object not find
    STTMPARAMERR                // PARAMETER MISMATCH
} StateMachineCWrapperErr_E;

/**
 * enum of history type
 */
typedef enum E_HISTORY_TYPE
{
    E_UNKOWNHISTORYTYPE = 0,
    E_SHALLOW = 1,              // Shallow history
    E_DEEP                      // Deep history
} HISTORY_TYPE_E;

/**
 * enum of join logical operator
 */
typedef enum E_GC_JOIN
{
    E_GC_AND = 1,
    E_GC_OR
} GC_JOIN_E;

/**
 * enum of value type
 */
typedef enum E_EV_VAL_KIND
{
    E_NONE = 0,
    E_NUMBER,
    E_DOUBLE,
    E_STRING
} EV_VAL_KIND_E;

/**
 * enum of guard condition operate
 */
typedef enum E_GC_OP_KIND
{
    GC_OP_NON = 0,
    GC_OP_EQ = 1,                       // "="   Guard condition is equal to
    GC_OP_NE,                           // "!="  Guard condition is not equal to
    GC_OP_LT,                           // "<"   Guard condition is less than
    GC_OP_LE,                           // "<="  Guard condition is less than or equal
    GC_OP_GT,                           // ">"   Guard condition is greater than
    GC_OP_GE,                           // ">="  Guard condition is greater than or equal
    GC_OP_IN,                           // "in"  Guard condition is the state of the state-object
    GC_OP_NOT                           // "not" Guard condition is the state of the state-object
} GC_OP_KIND_E;


typedef enum E_OPERATION_STATE          // state-object operation state
{
    E_STT_INACTIVE =0,                  // INACTIVE
    E_STT_ACTIVE                        // ACTIVE
} OPERATION_STATE_E;

/**
 * call back infomation parameter
 * Where entry-action and exit-action, do-action use call back
 */
typedef struct
{
    unsigned short  ev;                 // event no.
    EV_VAL_KIND_E   uk;                 // union 'u' kind
    union {                             //
        int             i;              // Used if the uk == E_NUMBER
        double          d;              // Used if the uk == E_DOUBLE
        const char*     s;              // Used if the uk == E_STRING
    }               u;                  //
    void*           v;                  // Additional information user
} st_event;

/**
 * @brief Create an stateMachine-object and state-object with the specified file
 * @param file faile path pseudo-json
 * @return not 0:stateMachine-object Id no.(c-wrapper local ID)
 * @       0:create error
 */
int ico_sttm_createStateMachineFile(const char* file);

/**
 * @brief Create an stateMachine-object and state-object with the specified text
 * @param buff text pseudo-json
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
int ico_sttm_createStateMachineText(const char* buff);

/**
 * @brief Create stateMachine-object
 * @param name stateMachine-object identification name
 * @param value stateMachine-object identification number.
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
int ico_sttm_createStateMachine(const char* name, int value);

/**
 * @brief Create state-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name state-object identification name
 * @param value state-object identification number
 * @return not 0:state-object ID no.(c-wrapper local ID)
 * @       0:create faile
 */
int ico_sttm_createState(int parent, const char* name, int value);


/**
 * @brief Create historyState-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name historyState-object identification name
 * @param value historyState-object identification number
 * @param type Shallow/Deep history type
 * @return not 0:historyState-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
int ico_sttm_createHState(int parent, const char* name, int value,
                      HISTORY_TYPE_E type);


/**
 * @brief Create finalState-object
 * @param parent parent ID no.(c-wrapper local ID)
 * @param name finalState-object identification name
 * @param value finalState-object identification number
 * @return not 0:finalState-object ID no.(c-wrapper local ID)
 * @       0:create error
 */
int ico_sttm_createFState(int parent, const char* name, int value);


/**
 * @brief Registration of transition events
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param evname event identification name
 * @param ev event no.(Use the 1000-65535) identification number
 * @param trCWId of transition destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
/* Please try a unique name in the naming event since the search key */
int ico_sttm_addTransition(int sCWId,
                           const char* evname, unsigned short ev,
                           int trCWId);


/**
 * @brief Registration of transition events
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param evname event name
 * @param ev event no.(Use the 1000-65535)
 * @param trVal state-object identification value destination
 * @param trCWId of transition destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
int ico_sttm_addTransitionVVV(int smVal, int value,
                              const char* evname, unsigned short ev,
                              int trVal);

/**
 * @brief Registration of transition events
 * @param smNm stateMachine-object identification name
 * @param name state-object identification name
 * @param evname event name
 * @param ev event no.(Use the 1000-65535)
 * @param namTrns state-object Identification name destination
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param trCWId of transition destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local ID)
 * @       0:added error
 */
int ico_sttm_addTransitionNNN(const char* smNm, const char* name,
                              const char* evname, unsigned short ev,
                              const char* trNm);

/**
 * @brief Registration of transition events
 * @param sCWId state-object ID no.(c-wrapper local ID)
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trCWId destination(c-wrapper local ID)
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
/* Please try a unique name in the naming event since the search key */
int ico_sttm_addTransitionGCI(int sCWId,
                              const char* evname, unsigned short ev,
                              GC_OP_KIND_E gce, int gcv,
                              int trCWId);

int ico_sttm_addTransitionGCD(int sCWId,
                              const char* evname, unsigned short ev,
                              GC_OP_KIND_E gce, double gcv,
                              int trCWId);

int ico_sttm_addTransitionGCS(int sCWId,
                              const char* evname, unsigned short ev,
                              GC_OP_KIND_E gce, const char* gcv,
                              int trCWId);

/**
 * @brief Registration of transition events
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param evname event name
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trVl value is state-object identification number transition, destination
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 */
int ico_sttm_addTransGCIVVV(int smVal, int value,
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, int gcv, int trVl);

int ico_sttm_addTransGCDVVV(int smVal, int value,
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, double gcv, int trVl);

int ico_sttm_addTransGCSVVV(int smVal, int value,
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, const char* gcv, int trVl);

/**
 * @brief Registration of transition events
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @param trNm name is state-object Identification name destination
 * @return not 0:ID no. of the event to register(c-wrapper local event-ID)
 * @       0:added error
 * construction ico_sttm_createStateMachineFile or ico_sttm_createStateMachineText
 * it can not be used
 */
int ico_sttm_addTransGCINNN(const char* smNm, const char* name,
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, int gcv,
                            const char* trNm);

int ico_sttm_addTransGCDNNN(const char* smNm, const char* name,
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, double gcv,
                            const char* trNm);

int ico_sttm_addTransGCSNNN(const char* smNm, const char* name, 
                            const char* evname, unsigned short ev,
                            GC_OP_KIND_E gce, const char* gcv,
                            const char* trNm);


/**
 * @brief add guard condition
 * @param evId ID no. to be registered(c-wrapper local event-ID)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 * construction ico_sttm_createStateMachineFile or ico_sttm_createStateMachineText
 * it can not be used
 */
int ico_sttm_addGCI(int evId, GC_JOIN_E j, GC_OP_KIND_E gc, int gcv);
int ico_sttm_addGCD(int evId, GC_JOIN_E j, GC_OP_KIND_E gc, double gcv);
int ico_sttm_addGCS(int evId, GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv);

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
int ico_sttm_addGCIVV(int smVal, int value,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, int gcv);
int ico_sttm_addGCDVV(int smVal, int value,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, double gcv);
int ico_sttm_addGCSVV(int smVal, int value,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv);

/**
 * @brief add guard condition
 * @param smNm stateMachine-object Identification name
 * @param name name is state-object Identification name
 * @param evname event name 
 * @param ev event no.(Use the 1000-65535)
 * @param j logical operator
 * @param gce operator of guard condition
 * @param gcv Comparison value of guard condition
 * @return not 0:Success
 * @       0:added failure
 */
int ico_sttm_addGCINN(const char* smNm, const char* name,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, int gcv);

int ico_sttm_addGCDNN(const char* smNm, const char* name,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, double gcv);

int ico_sttm_addGCSNN(const char* smNm, const char* name,
                      const char* evname, unsigned short ev,
                      GC_JOIN_E j, GC_OP_KIND_E gc, const char* gcv);


/**
 * @brief Callback registration
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
int ico_sttm_addEntryAction(int sCWId, void (*fnc)(st_event* ev, int sCWId));
int ico_sttm_addExitAction(int sCWId, void (*fnc)(st_event* ev, int sCWId));
int ico_sttm_addDoAction(int sCWId, void (*fnc)(st_event* ev, int sCWId));


/**
 * @brief Callback registration
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
int ico_sttm_addEntryActionVV(int smVal, int value,
                              void (*fnc)(st_event* ev, int sCWId));
int ico_sttm_addExitActionVV(int smVal, int value,
                             void (*fnc)(st_event* ev, int sCWId));
int ico_sttm_addDoActionVV(int smVal, int value,
                           void (*fnc)(st_event* ev, int sCWId));

/**
 * @brief Callback registration
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param fanc callback function
 * @return not 0:Success 
 * @       0:added Failure
 */
int ico_sttm_addEntryActionNN(const char* smNm, const char* name,
                              void (*fnc)(st_event* ev, int sCWId));

int ico_sttm_addExitActionNN(const char* smNm, const char* name,
                             void (*fnc)(st_event* ev, int sCWId));

int ico_sttm_addDoActionNN(const char* smNm, const char* name,
                           void (*fnc)(st_event* ev, int sCWId));


/**
 * @brief Registration at the time of termination detection transition destination
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param trCWId finish transition destination(c-wrapper local ID)
 * @return not 0:Success
 * @       0:set Failure
 */
int ico_sttm_setFinishTransition(int sCWId, int trCWId);

/**
 * @brief Registration at the time of termination detection transition destination
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param trVl value is state-object identification number transition destination
 * @return not 0:Success
 * @       0:set Failure
 */
int ico_sttm_setFinishTransitionVVV(int smVal, int value, int trVl);

/**
 * @brief Registration at the time of termination detection transition destination
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @return not 0:Success
 * @       0:set Failure
 */
int ico_sttm_setFinishTransitionNNN(const char* smNm, const char* name, 
                                    const char* trNm);

/**
 * @brief Registration of default history state-objectss
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param dfCWId  default transition destination(c-wrapper local ID)
 * @return not 0:Success
 * @       0:added Failure
 */
int ico_sttm_addDefault(int sCWId, int dfCWId);


/**
 * @brief Registration of default history state-objectss
 * @param value value is state-object identification number
 * @param dfVal value is state-object identification number
 * @return not 0:Success
 * @       0:added Failure
 */
int ico_sttm_addDefaultVVV(int smVal, int value, int dfVal);

/**
 * @brief Registration of default history state-objectss
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param dfNm  name is state-object Identification name
 * @return not 0:Success
 * @       0:added Failure
 */
int ico_sttm_addDefaultNNN(const char* smNm, const char* name,
                           const char* dfNm);

/**
 * @breif Registration of start position state-objects
 * @param ID stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @param sCWId start position state-object ID no.(c-wrapper local ID)
 * @return not 0:Success
 * @       0:set failure
 */
int ico_sttm_addStartPosition(int CWId, int sCWId);


/**
 * @breif Registration of start position state-objects
 * @param smVal value is stateMachine-object identification number
 * @param value value is state-object identification number
 * @param value spVal is state-object identification number
 * @return not 0:Success
 * @       0:set failure
 */
int ico_sttm_addStartPositionSMVV(int smVal, int spVal);

int ico_sttm_addStartPositionVVV(int smVal, int value, int spVal);


/**
 * @breif Registration of start position state-objects
 * @param smNm stateMachine-object Identification name
 * @param name  name is state-object Identification name
 * @param name  spNm is state-object Identification name
 * @return not 0:Success
 * @       0:set failure
 */
int ico_sttm_addStartPositionSMNN(const char* smNm, const char* spNm);

int ico_sttm_addStartPositionNNN(const char* smNm, const char* name,
                                 const char* spNm);


/**
 * @breif start stateMachine
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @return not 0:Success
 * @       0:start Failure
 */
int ico_sttm_startV(int smVal);

int ico_sttm_startN(const char* smNm);

int ico_sttm_start(int smCWId);


/**
 * @breif stop stateMachine
 * @param smCWId stateMachine-object ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @return not 0:Success
 * @       0:stop Failure
 */
int ico_sttm_stopV(int smVal);

int ico_sttm_stopN(const char* smNm);

int ico_sttm_stop(int smCWId);


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
int ico_sttm_eventV(int smVal, unsigned short ev, void* info);
int ico_sttm_eventGCIV(int smVal, unsigned short ev, int gcv, void* info);
int ico_sttm_eventGCDV(int smVal, unsigned short ev, double gcv, void* info);
int ico_sttm_eventGCSV(int smVal, unsigned short ev, const char* gcv,
                       void* info);

int ico_sttm_eventN(const char* smNm, unsigned short ev, void* info);
int ico_sttm_eventGCIN(const char* smNm, unsigned short ev, int gcv,
                       void* info);
int ico_sttm_eventGCDN(const char* smNm, unsigned short ev, double gcv,
                       void* info);
int ico_sttm_eventGCSN(const char* smNm, unsigned short ev, const char* gcv,
                       void* info);

int ico_sttm_event(int smCWId, unsigned short ev, void* info);
int ico_sttm_eventGCI(int smCWId, unsigned short ev, int gcv, void* info);
int ico_sttm_eventGCD(int smCWId, unsigned short ev, double gcv, void* info);
int ico_sttm_eventGCS(int smCWId, unsigned short ev, const char* gcv,
                      void* info);

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
int ico_sttm_eventTestV(int smVal, unsigned short ev);
int ico_sttm_eventTestGCIV(int smVal, unsigned short ev, int gcv);
int ico_sttm_eventTestGCDV(int smVal, unsigned short ev, double gcv);
int ico_sttm_eventTestGCSV(int smVal, unsigned short ev, const char* gcv);

int ico_sttm_eventTestN(const char* smNm, unsigned short ev);
int ico_sttm_eventTestGCIN(const char* smNm, unsigned short ev, int gcv);
int ico_sttm_eventTestGCDN(const char* smNm, unsigned short ev, double gcv);
int ico_sttm_eventTestGCSN(const char* smNm, unsigned short ev,
                           const char* gcv);

int ico_sttm_eventTest(int smCWId, unsigned short ev);
int ico_sttm_eventTestGCI(int smCWId, unsigned short ev, int gcv);
int ico_sttm_eventTestGCD(int smCWId, unsigned short ev, double gcv);
int ico_sttm_eventTestGCS(int smCWId, unsigned short ev, const char* gcv);

/**
 * @brief retrieve stateMachine-object
 * @param value/name  stateMachine-object ID no. / name
 * @return not 0:stateMachine-object ID no.(c-wrapper local ID)
 * @       0:not find
 */
int ico_sttm_retrieveMVal(int value);
int ico_sttm_retrieveMName(const char* name);


/**
 * @brief retrieve state-object(state/historyState/finalState)
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param value/name  state(state/historyState/finalState)-object ID no. / name
 * @return not 0:state-object ID no.(c-wrapper local ID)
 * @       0:not find
 */
int ico_sttm_retrieveSVal(int smCWId, int value);
int ico_sttm_retrivevSName(int smCWId, const char* name);


/**
 * @brief object check
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @return not 0:stateMachine-object
 * @           0:no stateMachine-object
 */
int ico_sttm_isStateMachine(int smCWId);

/**
 * @brief object check
 * @param sCWId state-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
int ico_sttm_isState(int sCWId);

/**
 * @brief object check
 * @param sCWId historyState-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
int ico_sttm_isHistoryState(int sCWId);

/**
 * @brief object check
 * @param sCWId finalState-object ID no.(c-wrapper local ID)
 * @return not 0:state-object
 * @       0:no state-object
 */
int ico_sttm_isFinalState(int sCWId);

/**
 * @brief get state-object running true
 * @param smCWId stateMachine-object ID no.(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @param sCWIds[] Enter the active state-objects(c-wrapper local ID)
 * @param values[] Enter the active state-objects identification number
 * @param names[] Enter the active state-objects identification name
 * @param size of array sCWIds or values or names
 * @return Number of registered sCWIds
 * @       -1:size over
 */
int ico_sttm_getActiveState(int smCWId, int sCWIds[], size_t sz);

int ico_sttm_getActiveStateVV(int smVal, int values[], size_t sz);

int ico_sttm_getActiveStateNN(const char* smNm, const char* names[],
                              size_t sz);

/**
 * @brief get state-object running directly under
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
int ico_sttm_getActiveStateDirectlyUnder(int smCWId, int sCWIds[], size_t sz);

int ico_sttm_getActiveStateDirectlyUnderVV(int smval, int values[], size_t sz);

int ico_sttm_getActiveStateDirectlyUnderNN(const char* smNm,
                                           const char* names[], size_t sz);

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
int ico_sttm_getActiveStateAll(int smCWId, int sCWIds[], size_t sz);

int ico_sttm_getActiveStateAllVV(int smVal, int values[], size_t sz);

int ico_sttm_getActiveStateAllNN(const char* smNm, const char* names[],
                                 size_t sz);

/**
 * @brief get name value
 * @param CWId stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @return not 0:identification name  0:get fail
 */
const char* ico_sttm_getName(int CWId);


/**
 * @brief get name value
 * @param CWId stateMachine-object/state-object ID no.(c-wrapper local ID)
 * @return not 0:identification value  0:get fail
 */
int ico_sttm_getValue(int CWId);


/**
 * @breif get operation state
 * @param smCWId ID no. to eb registered stateMachine-object(c-wrapper local ID)
 * @param sCWId ID no. to be registered(c-wrapper local ID)
 * @param smVal value is stateMachine-object identification number
 * @param smNm stateMachine-object Identification name
 * @param value value is state-object identification number
 * @param name  name is state-object Identification name
 * @return OPERATION_STATE_E
 */
OPERATION_STATE_E ico_sttm_isActiveSttM(int smCWId);

OPERATION_STATE_E ico_sttm_isActiveStt(int sCWId);

OPERATION_STATE_E ico_sttm_isActiveVV(int smVal, int value);

OPERATION_STATE_E ico_sttm_isActiveNN(const char* smNm, const char* name);

#ifdef __cplusplus
}
#endif

#endif  /* end of __ICO_STATEMACHIN_CWRAPPER_H__ */
