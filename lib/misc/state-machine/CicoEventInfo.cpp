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
#include <algorithm>
#include <sstream>
#include "CicoEvent.h"
#include "CicoEventInfo.h"
#include "CicoStateCore.h"


/**
 * event infomation class
 * @author m.kamoshida
 * @version 0.1
 */

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param ev base event data
 */
CicoEventInfo::CicoEventInfo(unsigned short ev):
    CicoEvent(ev)
{
    m_JGCE =  Ejnj;                     //
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(unsigned short ev, const std::string& gcstr,
                             int value):
    CicoEvent(ev, value)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(unsigned short ev, const std::string& gcstr,
                             double value):
    CicoEvent(ev, value)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(unsigned short ev, const std::string& gcstr,
                             const std::string& value):
    CicoEvent(ev, value)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param name event name
 * @param ev event number
 */
CicoEventInfo::CicoEventInfo(const std::string& name, unsigned short ev):
    CicoEvent(ev), m_name(name)
{
    m_JGCE =  Ejnj;
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param name event name
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(const std::string& name, unsigned short ev,
                             const std::string& gcstr, int value):
    CicoEvent(ev, value), m_name(name)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param name event name
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(const std::string& name, unsigned short ev,
                             const std::string& gcstr, double value):
    CicoEvent(ev, value), m_name(name)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param name event name
 * @param ev event number
 * @param gcstr operator kind
 * @param value Comparison value
 */
CicoEventInfo::CicoEventInfo(const std::string& name, unsigned short ev,
                             const std::string& gcstr,
                             const std::string& value):
    CicoEvent(ev, value), m_name(name)
{
    m_JGCE = chgGCType(gcstr);          // string change to enum
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
}

/**
 * @brief CicoEventInfo::CicoEventInfo
 * @param ei copy source
 */
CicoEventInfo::CicoEventInfo(const CicoEventInfo& ei):
    CicoEvent()
{
    m_JGCE = Ejnj;
    m_transition = (CicoStateCore*)0;   // Initialize the pointer value
    *this = ei;
}

/**
 * @brief CicoEventInfo::~CicoEventInfo
 */
CicoEventInfo::~CicoEventInfo()
{
    if (false == m_vEvt.empty()) {
        m_vEvt.clear();
    }
    if (false == m_vJGCE.empty()) {
        m_vJGCE.clear();
    }
    if (false == m_vAgc.empty()) {
        m_vAgc.clear();
    }
//    m_transition = (CicoStateCore*)0;
}

/**
 * @brief CicoEventInfo::addAndGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addAndGurdCondition(const std::string& gcstr, int value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_AND);
    return true;
}

/**
 * @brief CicoEventInfo::addAndGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addAndGurdCondition(const std::string& gcstr, double value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_AND);
    return true;
}

/**
 * @brief CicoEventInfo::addAndGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addAndGurdCondition(const std::string& gcstr,
                                        const std::string& value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_AND);
    return true;
}

/**
 * @brief CicoEventInfo::addOrGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addOrGurdCondition(const std::string& gcstr, int value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_OR);
    return true;
}


/**
 * @brief CicoEventInfo::addOrGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addOrGurdCondition(const std::string& gcstr,
                                       double value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_OR);
    return true;
}

/**
 * @brief CicoEventInfo::addOrGurdCondition
 * @param gcstr operator kind
 * @param value Comparison value
 * @return true:added success false:added fail
 */
bool CicoEventInfo::addOrGurdCondition(const std::string& gcstr,
                                       const std::string& value)
{
    CicoEvent e(m_ev, value);
    E_JGCE j = chgGCType(gcstr);        // string change to enum
    if (Ejnj == j) {
        return false;
    }
    addGurdCondition(e, j, EAGC_OR);
    return true;
}

/**
 * @brief CicoEventInfo::Judge
 * @param ev event data
 * @param sm compare trigger state-object
 * @return true:event match false:event Mismatch
 */
bool CicoEventInfo::Judge(const CicoEvent& ev, const CicoStateCore* sm) const
{
    if (m_ev != ev.getEV()) {           //
        return false;                   // event number mismatch
    }
    // The event number matches
    if (Ejnj == m_JGCE) {               // Without guard condition
        return true;                    // event match !
    }
    // check guard condition
    bool bR = checkGurdCondition(m_JGCE, *((CicoEvent*)this), ev, sm); // return value

    int sz = m_vEvt.size();
    for (int i=0; i<sz; i++) {
        bool bR2 = checkGurdCondition(m_vJGCE[i], m_vEvt[i], ev, sm);
        E_AGC agc = m_vAgc[i];
        if (EAGC_AND == agc) {
            bR &= bR2;
        }
        else if (EAGC_OR == agc) {
            bR |= bR2;
        }
        else {
            return false;
        }
    }
    return bR;
}

/**
 * @brief CicoEventInfo::checkGurdCondition
 * @param j operator
 * @param base event data
 * @param rec event data
 * @param sm compair trigger state-object
 * @return treu:match false:Mismatch
 */
bool CicoEventInfo::checkGurdCondition(E_JGCE j, const CicoEvent& base,
                                       const CicoEvent& rec,
                                       const CicoStateCore* sm) const
{
    bool bR = false;                    // return value init.
    if ((Ein == j)||(Enot == j)) {      // Guard condition of active state object
        const CicoStateCore* stt = (CicoStateCore*)0;
        if (EEvtGCInt == base.getVTG()) {
            stt = getStateF(sm, base.getGCVi());
        }
        else if (EEvtGCStr == base.getVTG()) {
            stt = getStateF(sm, base.getGCVs());
        }
        if ((CicoStateCore*)0 == stt) {
            return false;
        }
        bR = checkActive(j, *stt);
    }
    else {                              // Remaining comparisons
        bR = checkCompare(j, base, rec);
    }
    return bR;
}

/**
 * @brief CicoEventInfo::checkActive
 * @param j in or not operator
 * @param stt check target
 * @return true:event match false:event Mismatch
 */
bool CicoEventInfo::checkActive(E_JGCE j, const CicoStateCore& stt) const
{
    if (Ein == j) {
        if (true == stt.isActive()) {
            return true;
        }
    }
    else if (Enot == j) {
        if (false == stt.isActive()) {
            return true;
        }
    }
    return false;
}

/**
 * @brief CicoEventInfo::checkCompare
 * @param j operator
 * @param base Comparison Event
 * @param rec Comparison Event
 * @return true:match false:Mismatch
 */
bool CicoEventInfo::checkCompare(E_JGCE j, const CicoEvent& base,
                                 const CicoEvent& rec) const
{
    bool bR = false;
    switch (j) {
    case Ejeq:                          // equal
        if (base == rec) {
            bR = true;
        }
        break;
    case Ejne:                          // Not equal
        if (base != rec) {
            bR = true;
        }
        break;
    case Ejgt:                          // greater than
        if (base < rec) {
            bR = true;
        }
        break;
    case Ejge:                          // greater than or equal
        if (base <= rec) {
            bR = true;
        }
        break;
    case Ejlt:                          // less than
        if (base > rec) {
            bR = true;
        }
        break;
    case Ejle:                          // less than or equal
        if (base >= rec) {
            bR = true;
        }
        break;
    default:                            // ERROR
        break;
    }
    return bR;
}

/**
 * @brief CicoEventInfo::addGurdCondition
 * @param e Comparison Event
 * @param j operator infomation(==/!=/>/>=/</<=/in/not)
 * @param a and/or
 */
void CicoEventInfo::addGurdCondition(CicoEvent& e, E_JGCE j, E_AGC a)
{
    if (Ejnj == m_JGCE) {
        E_TOV tov = e.getVTG();
        if (EEvtGCInt == tov) {
            setGCV(e.getGCVi());
            m_JGCE = j;
        }
        else if (EEvtGCDouble == tov) {
            setGCV(e.getGCVd());
            m_JGCE = j;
        }
        else if (EEvtGCStr == tov) {
            setGCV(e.getGCVs());
            m_JGCE = j;
        }
    }
    else {
       m_vEvt.push_back(e);
       m_vJGCE.push_back(j);
       m_vAgc.push_back(a);
    }
}

/**
 * @brief CicoEventInfo::chgGCType
 * @param gcstr string operator
 * @return enum operator
 */
CicoEventInfo::E_JGCE CicoEventInfo::chgGCType(const std::string& gcstr) const
{
    if (gcstr == DEFJGCE_EQ) {          // "="
        return Ejeq;                    // equal
    }
    else if (gcstr == DEFJGCE_NE) {     // "!="
        return Ejne;                    // Not equal
    }
    else if (gcstr == DEFJGCE_GT) {     // "<"
        return Ejgt;                    // greater than
    }
    else if (gcstr == DEFJGCE_GE) {     // "<="
        return Ejge;                    // greater than or equal
    }
    else if (gcstr == DEFJGCE_GE2) {    // "=<"
        return Ejge;                    // greater than or equal
    }
    else if (gcstr == DEFJGCE_LT) {     // ">"
        return Ejlt;                    // less than
    }
    else if (gcstr == DEFJGCE_LE) {     // ">="
        return Ejle;                    // less than or equal
    }
    else if (gcstr == DEFJGCE_LE2) {    // "=>"
        return Ejle;                    // less than or equal
    }
    else {                              //
        std::string s(gcstr);           // copy string
        transform(s.begin(), s.end(), s.begin(), tolower); // converted to lower case
        if (s == DEFJGCE_IN) {          // "in"
            return Ein;                 // in active state object
        }
        else if (s == DEFJGCE_NOT) {    // "not"
            return Enot;                // not active state object
        }
        else {
            /*
            * _D_ERROR
            */
        }
    }
    return Ejnj;                        // Not Judgment
}

/**
 * @brief CicoEventInfo::operator =
 * @param s copy source
 */
void CicoEventInfo::operator=(const CicoEventInfo& s)
{
    // CicoEvent Block
    setEV(s.getEV());
    E_TOV tov = s.getVTG();
    switch (tov) {
    case EEvtGCInt:
        setGCV(s.getGCVi());
        break;
    case EEvtGCDouble:
        setGCV(s.getGCVd());
        break;
    case EEvtGCStr:
        setGCV(s.getGCVs());
        break;
    default:
        break;
    }

    setAdVal(s.getAdVal());

    // CicoEventInfo Block
    m_name = s.m_name;                  // copy event name
    m_JGCE = s.m_JGCE;                  // copy Judgment guard condition expression
    int sz = s.m_vEvt.size();           // get vector member size
    for (int i = 0; i < sz; i++) {      // vector menber size loop
        m_vEvt.push_back(s.m_vEvt[i]);
        m_vJGCE.push_back(s.m_vJGCE[i]);
        m_vAgc.push_back(s.m_vAgc[i]);
    }
    m_transition = s.m_transition;      //
}

/**
 * @brief getLogPartF
 * @param e log target
 * @param l store log string
 */
void getLogPartF(const CicoEventInfo& e, std::string& l)
{
    const char* opstr[] = {
        "??",                       //       Ejnj Not Judgment
        DEFJGCE_EQ,                     // "="   Ejeq equal
        DEFJGCE_NE,                     // "!="  Ejne Not equal
        DEFJGCE_LT,                     // "<"   Ejlt less than
        DEFJGCE_LE,                     // "<="  Ejle less than or equal
        DEFJGCE_GT,                     // ">"   Ejgt greater than
        DEFJGCE_GE,                     // ">="  Ejge greater than or equal
        DEFJGCE_IN,                     // "in"  Ein  in active state object
        DEFJGCE_NOT,                    // "not" Enot not active state object
    };
    std::stringstream stm;
    stm << "EI:{";
    if (!((e.getName()).empty())) {
        stm << "\"" << e.getName() << "\"";
    }
    stm << ":" << e.getEV() << ":";
    CicoEventInfo::E_JGCE jgce = e.getGCType();
    if (CicoEventInfo::Ejnj != jgce) {
        stm << opstr[jgce];
    }
    bool bInNot = false;
    if ((CicoEventInfo::Ein == jgce)||
        (CicoEventInfo::Enot == jgce)) {
        bInNot = true;
        stm << "(";
    }
    switch (jgce) {
    case CicoEvent::EEvtGCInt:
        stm << e.getGCVi();
        break;
    case CicoEvent::EEvtGCDouble:
        stm << e.getGCVd();
        break;
    case CicoEvent::EEvtGCStr:
        stm << "\"" << e.getGCVs() << "\"";
        break;
    default:                            // CicoEvent::EEvtGCUnset
        break;
    }
    if (true == bInNot) {
        stm << ")";
    }
//    if (0 != e->m_vAgc.size()) {
//        stm << ":{...}";
//    }
    stm << "}";
    l = stm.str();
}

/**
 * @brief operator ==
 * @param a Comparison target A
 * @param b Comparison target B
 * @return
 */
bool operator==(const CicoEventInfo& a, const CicoEventInfo& b)
{
    if (a.getEV() == b.getEV()) {       // event No. match
        bool anm = a.m_name.empty();
        bool bnm = b.m_name.empty();
        if ((true == anm) && (true == bnm)) { //
            return true;
        }
        else if ((false == anm) && (false == bnm)) {
            if (a.m_name == b.m_name) {
                return true;
            }
        }
    }
    return false;
}
