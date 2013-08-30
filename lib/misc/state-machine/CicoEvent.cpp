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
#include <sstream>
#include <string>

#include "CicoEvent.h"



/**
 * Event delivery class
 * @author m.kamoshida
 * @version 0.1
 */



/**
 * @brief CicoEvent::CicoEvent
 */
CicoEvent::CicoEvent():
    m_ev(0), m_tov(EEvtGCUnset), m_gcvi(0), m_gcvd(0.0), m_adVal((void*)0)
{
}

/**
 * @brief CicoEvent::CicoEvent
 * @param ev event number
 * @param adval Additional information user
 */
CicoEvent::CicoEvent(unsigned short ev, void* adval):
    m_ev(ev), m_tov(EEvtGCUnset), m_gcvi(0), m_gcvd(0.0), m_adVal(adval)
{
}

/**
 * @brief CicoEvent::CicoEvent
 * @param ev event number
 * @param gcv Comparison value of guard condition
 * @param adval Additional information user
 */
CicoEvent::CicoEvent(unsigned short ev, int gcv, void* adval):
    m_ev(ev), m_tov(EEvtGCInt), m_gcvi(gcv), m_gcvd(0.0), m_adVal(adval)
{
}

/**
 * @brief CicoEvent::CicoEvent
 * @param oev copy source
 */
CicoEvent::CicoEvent(const CicoEvent& oev)
{
    *this = oev;
}

/**
 * @brief CicoEvent::CicoEvent
 * @param ev event number
 * @param gcv Comparison value of guard condition
 * @param adval Additional information user
 */
CicoEvent::CicoEvent(unsigned short ev, double gcv, void* adval):
    m_ev(ev), m_tov(EEvtGCDouble), m_gcvi(0), m_gcvd(gcv), m_adVal(adval)
{
}

/**
 * @brief CicoEvent::CicoEvent
 * @param ev event number
 * @param gcv Comparison value of guard condition
 * @param adval Additional information user
 */
CicoEvent::CicoEvent(unsigned short ev, const std::string& gcv, void* adval):
    m_ev(ev), m_tov(EEvtGCStr), m_gcvi(0), m_gcvd(0.0), m_gcvs(gcv), m_adVal(adval)
{
}

/**
 * @brief CicoEvent::~CicoEvent
 */
CicoEvent::~CicoEvent()
{
    m_adVal = (void*)0;
}

/**
 * @brief isEventMatch
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:match false:mismatch
 */
bool isEventMatch(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_ev == b.m_ev) {
        return true;
    }
    return false;
}


/**
 * @brief operator ==
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:match false:mismatch
 */
bool operator==(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi == b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd == b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs == b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief operator !=
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:mismatch false:match
 */
bool operator!=(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi != b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd != b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs != b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief operator >
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:greater than false not greater than
 */
bool operator>(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi > b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd > b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs > b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief operator >=
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:greater than or equal false:not greater than or equal
 */
bool operator>=(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {           //
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi >= b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd >= b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs >= b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief operator <
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:less than false:not less than
 */
bool operator<(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi < b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd < b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs < b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief operator <=
 * @param a Comparison target A
 * @param b Comparison target B
 * @return true:less than or equal false:not less than or equal
 */
bool operator<=(const CicoEvent& a, const CicoEvent& b)
{
    if (a.m_tov == b.m_tov) {
        if (CicoEvent::EEvtGCUnset == a.m_tov) {
            return false;
        }
        else if (CicoEvent::EEvtGCInt == a.m_tov) {
            if (a.m_gcvi <= b.m_gcvi) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCDouble == a.m_tov) {
            if (a.m_gcvd <= b.m_gcvd) {
                return true;
            }
            return false;
        }
        else if (CicoEvent::EEvtGCStr == a.m_tov) {
            if (a.m_gcvs <= b.m_gcvs) {
                return true;
            }
            return false;
        }
    }
    return false;
}

/**
 * @brief CicoEvent::operator =
 * @param s copy source
 */
void CicoEvent::operator=(const CicoEvent& s)
{
    m_ev    = s.m_ev;
    m_tov   = s.m_tov;
    m_gcvi  = s.m_gcvi;
    m_gcvd  = s.m_gcvd;
    m_gcvs  = s.m_gcvs;
    m_adVal = s.m_adVal;
}

/**
 * @brief getLogPartF
 * @param e edit target
 * @param l log string output
 */
void getLogPartF(const CicoEvent& e, std::string& l)
{
    std::stringstream stm;
    stm << "EV:{" << e.getEV() << ":";     // EV:{9999:[99/99.9/"xxx"]} image edit

    switch (e.getVTG()) {
    case CicoEvent::EEvtGCInt:
        stm << e.getGCVi();
        break;
    case CicoEvent::EEvtGCDouble:
        stm << e.getGCVd();
        break;
    case CicoEvent::EEvtGCStr:
        stm << "\"" << e.getGCVs() << "\"";
        break;
    default:
        break;
    }
    stm << "}";
    l =  stm.str();
}
