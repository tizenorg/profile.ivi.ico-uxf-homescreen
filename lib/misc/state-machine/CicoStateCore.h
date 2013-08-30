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
#ifndef CICOSTATECORE_H
#define CICOSTATECORE_H

#include <iostream>
#include <string>
#include <vector>
#include "CicoEvent.h"

#ifndef CICOEVENTINFO_H
class CicoEventInfo;
#endif

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOSTATEACTION_H
class CicoStateAction;
#endif

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

#ifndef CICOSTAEACTION_H
class CicoStateAction;
#endif

#ifndef CICOHISTORYSTATE_H
class CicoHistoryState;
#endif


/**
 * It is the core definition of the state transition
 * @author m.kamoshida
 * @version 0.1
 */
/**
 * @brief The CicoStateCore class
 */
class CicoStateCore {
public:
    // Type of state object
    enum E_TOSO {
        EStateMachine = 0,              // kind of stateMachine object
        EState,                         // kind of state object
        EHistoryState,                  // kind of history state object
        EFinalState                     // kind of final state 0bject
    };

    enum E_GetPolicy {                  // get current state policy
        ELvlTop = 0,                    //  First-degree active state
        ELvlAll,                        //  ALL active state
        ELvlBttm                        //  Buttom active state
    };
    // Constructior
                CicoStateCore(E_TOSO ts, CicoStateCore* parent =0);
                CicoStateCore(E_TOSO ts, const std::string& name,
                              CicoStateCore* parent =0);
                CicoStateCore(E_TOSO ts, const std::string& name, int value,
                              CicoStateCore* parent =0);
                CicoStateCore(E_TOSO ts, int value, CicoStateCore* parent =0);

    // destructor
    virtual     ~CicoStateCore();

    // Identification name get/set
    const std::string& getName() const;
    void        setName(const std::string& name);

    // Identification number get/set
    int         getValue() const;
    void        setValue(int value);

    // get parent
    const CicoStateCore* getParent() const;

    // active/inactive get
    bool        isActive() const;             // active state

    // get object kind
    E_TOSO      getType() const;
    bool        isStateMachine() const;
    bool        isState() const;
    bool        isFinalState() const;
    bool        isHistoryState() const;

    bool        isRegisteredInit() const;

    // c-wrapper I/F
    void        getObjects(std::vector<CicoStateCore*>& obj);
    const CicoEventInfo* getEventInfo(const std::string& name,
                                      unsigned short ev) const;
protected:
    /**
     * union kind type
     */
    enum E_SttUnionKind {
        EPUnused = 0,                   // unused
        EPDir,                          // pointer direct
        EPVec                           // pointer vector
    };

    /**
     * event infomation define
     */
    typedef struct {
        E_SttUnionKind                  suk;
        union {
            CicoEventInfo*               d; // direct pointer
            std::vector<CicoEventInfo*>* v; // vecter pointers
        } u;
    } stEventInfo;

    /**
     * start position define
     */
    typedef struct {
        E_SttUnionKind                  suk; // union kind
        union {
            CicoStateCore*               d; // direct pointer
            std::vector<CicoStateCore*>* v; // vector pointers
        } u;
    } stStartPosition;

    /**
     * StateAction define
     */
    typedef struct {
        CicoStateAction*                dcsa; // direct pointer
        int                             dav;  // additional value
    } stDirSttAction;

    typedef struct {
        std::vector<CicoStateAction*>*  vcsa; // vector pointers
        std::vector<int>*               vav;  // vector additional values
    } stVecSttAction;

    typedef struct {
        E_SttUnionKind                  suk; // union kind
        union {
            stDirSttAction              d;   // direct data
            stVecSttAction              v;   // vector datas
        } u;
    } stSttAction;

    // my area operations
    void        initTransition();
    void        initStartPosition();
    void        initSttAction();

    void        clearTransition();
    void        clearStartPosition();
    void        clearSttAction();



    void        setParent(CicoStateCore* parent);
    friend void setParentF(CicoStateCore* stt, CicoStateCore* parent);


    void        addState(CicoStateCore* state);
    friend void addStateF(CicoStateCore* parent, CicoStateCore* state);


    void        setFinishTransition(CicoStateCore* state);


    void        addTransition(const CicoEventInfo& evInf,
                              CicoStateCore* state);


    void        addInitState(CicoStateCore* state);


    void        addEntryAction(CicoStateAction* action, int addval);


    void        addExitAction(CicoStateAction* action, int addval);


    void        addDoAction(CicoStateAction* action, int addval);


    void        addAction(stSttAction& ssa, CicoStateAction* action,
                          int addval);


    void        toActivate(const CicoEvent& ev);
    friend void toActivateF(CicoStateCore* stt, const CicoEvent& ev);


    void        toDeactivate(const CicoEvent& ev);
    friend void toDeactivateF(CicoStateCore* stt, const CicoEvent& ev);


    void        onEntry(const CicoEvent& ev);
    void        onExit(const CicoEvent& ev);
    void        onDo(const CicoEvent& ev);
    void        onEntryRun(CicoStateAction* run, const CicoEvent& ev, int val);
    void        onExitRun(CicoStateAction* run, const CicoEvent& ev, int val);
    void        onDoRun(CicoStateAction* run, const CicoEvent& ev, int val);

    enum E_ActiveImpact {
        Single = 0,                     // is Active
        Follower                        // is Follower active
    };
    // get active state-objects
    bool        getActiveSonar(std::vector<CicoState*>& stt,
                               std::vector<E_ActiveImpact>& impact);
    //
    bool        getActiveSonarChild(std::vector<CicoState*>& stt,
                                    std::vector<E_ActiveImpact>& impact);

    // 
    const CicoEventInfo* eventTest(const CicoEvent& ev,
                                   const CicoStateCore* sm) const;
    // 
    friend const CicoEventInfo* eventTestF(const CicoStateCore* stt,
                                           const CicoEvent& ev,
                                           const CicoStateCore* sm);

    // start state-object   Activation
    virtual bool start(const CicoEvent& ev, bool parent = true);
    friend bool startF(CicoStateCore* s, const CicoEvent& ev, bool parent);

    // stop state-object  Deactivation
    virtual bool stop(const CicoEvent& ev);

    //
    virtual void holdHistory();         // Hold History
    friend void holdHistoryF(CicoStateCore* s);

    // 
    bool        getCurrentState(std::vector<const CicoState*>& states,
                                E_GetPolicy policy);
    const CicoState* getCurrentState(); //
    friend bool getCurrentStateF(CicoStateCore* stt,
                                std::vector<const CicoState*>& states,
                                E_GetPolicy policy);

    void        getRegisteredInit(std::vector<CicoStateCore*>& stt, int& cnt);

    //
    void        stateEnd(const CicoEvent& ev);
    friend void stateEndF(CicoStateCore* stt, const CicoEvent& ev);

    // The transition from s to d
    void        onTransition(const CicoEvent& ev, CicoStateCore* s, CicoStateCore* d);

    //
    void        getRoots(const CicoStateCore* stt, std::vector<CicoStateCore*>& roots);

    // do action system
    bool        onDoExec(const CicoEvent& ev);
    friend bool onDoExecF(CicoStateCore* s, const CicoEvent& ev);

    //
    const CicoStateCore* getState(const int value) const;
    const CicoStateCore* getState(const std::string& name) const;
    friend const CicoStateCore* getStateF(const CicoStateCore* s,
                                          const int value);
    friend const CicoStateCore* getStateF(const CicoStateCore* s,
                                          const std::string& name);

    // log
    friend void getLogPartF(const CicoStateCore* s, std::string& l);

protected:
    bool        m_activeState;
    std::string m_name;                 // identification name
    int         m_value;                // identification number
    E_TOSO      m_eType;                // type my class
    CicoStateCore* m_parent;            // parent state object
    std::vector<CicoStateCore*> m_childs; // Child state-objects
    bool        m_existHistoy;          // flag History holder
    CicoStateCore* m_stateTermination;  //
    // transition datas
    stEventInfo m_eventInfo;
    // start position
    stStartPosition m_startPosition;
    // entry action
    stSttAction m_entry;
    // exit action
    stSttAction m_exit;
    // do action
    stSttAction m_do;

private:

};

/**
 * @brief CicoStateCore::getName
 * @      get state-object identification nemae
 * @return state-object identification name
 */
inline const std::string& CicoStateCore::getName() const
{
    return m_name;
}

/**
 * @brief CicoStateCore::getValue
 * @     get state-object identification number
 * @return state-object identification number
 */
inline int CicoStateCore::getValue() const
{
    return m_value;
}

/**
 * @brief CicoStateCore::setValue
 * @      set state-object identification number
 * @paran value identification number
 */
inline void CicoStateCore::setValue(int value)
{
    m_value = value;
}

/**
 * @brief CicoStateCore::getParent
 * @return not 0:parent state-object pointer / 0:none parent
 */
inline const CicoStateCore* CicoStateCore::getParent() const
{
    return m_parent;                    // parent state object
}

/**
 * @brief CicoStateCore::getType
 * @return type of state-object class
 */
inline CicoStateCore::E_TOSO CicoStateCore::getType() const
{
    return m_eType;
}

/**
 * @brief CicoStateCore::isActive
 * get active/notactive status
 * @return active state true:active / false:not active
 */
inline bool CicoStateCore::isActive() const
{
    return m_activeState;
}

/**
 * @brief CicoStateCore::isStateMachine
 * @return true:stateMachine-object
 * @       false:none stateMachine-object
 */
inline bool CicoStateCore::isStateMachine() const
{
    if (EStateMachine == m_eType) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateCore::isState
 * @return true:state-object
 * @       false:none state-object
 */
inline bool CicoStateCore::isState() const
{
    if (EState == m_eType) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateCore::isFinalState
 * @return true:finalState-object
 * @       false:none finaltate-object
 */
inline bool CicoStateCore::isFinalState() const
{
    if (EFinalState == m_eType) {
        return true;
    }
    return false;
}

/**
 * @brief CicoStateCore::isHistoryState
 * @return true:historyState-object
 * @       false:none historystate-object
 */
inline bool CicoStateCore::isHistoryState() const
{
    if (EHistoryState == m_eType) {
        return true;
    }
    return false;
}
/**
 * @brief getLogPartF
 * @param e
 * @param l
 */
void getLogPartF(const CicoEvent& e, std::string& l);

/**
 * @brief getLogPartF
 * @param e
 * @param l
 */
void getLogPartF(const CicoEventInfo& e, std::string& l);

#endif // CICOSTATECORE_H
