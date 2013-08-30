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
#ifndef CICOEVENTINFO_H
#define CICOEVENTINFO_H

#include <iostream>
#include <string>
#include <vector>
#include "CicoEvent.h"

#ifndef CICOSTATECORE_H
class CicoStateCore;
#endif

#ifndef CICOSTATE_H
class CicoState;
#endif

#ifndef CICOSTATEMACHINE_H
class CicoStateMachine;
#endif

#define DEFJGCE_EQ      "="
#define DEFJGCE_NE      "!="
#define DEFJGCE_LT      "<"
#define DEFJGCE_LE2     "=<"
#define DEFJGCE_LE      "<="
#define DEFJGCE_GT      ">"
#define DEFJGCE_GE2     "=>"
#define DEFJGCE_GE      ">="
#define DEFJGCE_IN      "in"
#define DEFJGCE_NOT     "not"


/**
 * event infomation object class
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * @brief The CicoEventInfo class
 */
class  CicoEventInfo : public  CicoEvent
{
public:
    // type of added guard condition
    enum E_AGC {
        EAGC_NON = 0,                   // none
        EAGC_AND = 1,                   // AND
        EAGC_OR                         // OR
    };

    // Judgment guard condition expression
    enum  E_JGCE {
        Ejnj = 0,                       // Not Judgment
        Ejeq = 1,                       // equal
        Ejne,                           // Not equal
        Ejlt,                           // less than
        Ejle,                           // less than or equal
        Ejgt,                           // greater than
        Ejge,                           // greater than or equal
        Ein,                            // in active state object
        Enot                            // not active state object
    };

    // Constructior
                CicoEventInfo(unsigned short ev);
                CicoEventInfo(unsigned short ev, const std::string& gcstr,
                              int value);
                CicoEventInfo(unsigned short ev, const std::string& gcstr,
                              double value);
                CicoEventInfo(unsigned short ev, const std::string& gcstr,
                              const std::string& value);

                CicoEventInfo(unsigned short ev, const std::string& gcstr,
                              CicoState* stt);

                CicoEventInfo(const std::string& name, unsigned short ev);
                CicoEventInfo(const std::string& name, unsigned short ev,
                              const std::string& gcstr, int value);
                CicoEventInfo(const std::string& name, unsigned short ev,
                              const std::string& gcstr, double value);
                CicoEventInfo(const std::string& name, unsigned short ev,
                              const std::string& gcstr,
                              const std::string& value);
                CicoEventInfo(const std::string& name, unsigned short ev,
                              const std::string& gcstr, CicoState* stt);
                CicoEventInfo(const CicoEventInfo& ei);
    // destructor
    virtual     ~CicoEventInfo();

    bool        addAndGurdCondition(const std::string& gcstr, int value);
    bool        addAndGurdCondition(const std::string& gcstr, double value);
    bool        addAndGurdCondition(const std::string& gcstr,
                                    const std::string& value);

    bool        addOrGurdCondition(const std::string& gcstr, int value);
    bool        addOrGurdCondition(const std::string& gcstr, double value);
    bool        addOrGurdCondition(const std::string& gcstr,
                                   const std::string& value);

    bool        Judge(const CicoEvent& ev, const CicoStateCore* sm) const;

    const std::string& getName() const;
    void        setName(const std::string& name);

    CicoStateCore*  getTransition() const;
    void        setTransition(CicoStateCore* transition);

    void operator=(const CicoEventInfo& s);
    friend bool operator==(const CicoEventInfo& a, const CicoEventInfo& b);

    E_JGCE      getGCType() const;

protected:

    E_JGCE      chgGCType(const std::string& gcstr) const;

    bool        checkGurdCondition(E_JGCE j, const CicoEvent& base,
                                   const CicoEvent& rec,
                                   const CicoStateCore* sm) const;
    bool        checkActive(E_JGCE j, const CicoStateCore& stt) const;
    bool        checkCompare(E_JGCE j, const CicoEvent& base,
                             const CicoEvent& rec) const;

    void        addGurdCondition(CicoEvent& e, E_JGCE j, E_AGC a);

    friend const CicoStateCore* getState(const CicoStateCore* s,
                                         const int value);
    friend const CicoStateCore* getState(const CicoStateCore* s,
                                         const std::string& name);

private:

    std::string     m_name;             // event name
    E_JGCE          m_JGCE;             // guard condition 
    std::vector<CicoEvent> m_vEvt;      // additional guard condition
    std::vector<E_JGCE>    m_vJGCE;     // 
    std::vector<E_AGC>     m_vAgc;      // 
    CicoStateCore*  m_transition;       // Transition destination

};

/**
 * @brief CicoEventInfo::getName
 * @return event identification name
 */
inline const std::string& CicoEventInfo::getName() const
{
    return m_name;
}

/**
 * @brief CicoEventInfo::setName
 * @      set event name
 * @param name event identification name
 */
inline void CicoEventInfo::setName(const std::string& name)
{
    m_name = name;
}

/**
 * @brief CicoEventInfo::getTransition
 * @      get object destination state
 * @return registered transition state-object pointer
 */
inline CicoStateCore* CicoEventInfo::getTransition() const
{
    return m_transition;
}

/**
 * @brief CicoEventInfo::setTransition
 *  @     set object destination state
 * @param transition pointer state object
 */
inline void CicoEventInfo::setTransition(CicoStateCore* transition)
{
    m_transition = transition;
}

/**
 * @brief CicoEventInfo::getGCType
 * @return
 */
inline CicoEventInfo::E_JGCE CicoEventInfo::getGCType() const
{
    return m_JGCE;
}

#endif // CICOEVENTINFO_H
