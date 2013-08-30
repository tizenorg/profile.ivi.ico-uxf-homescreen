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
#ifndef CICOEVENT_H
#define CICOEVENT_H

#include <iostream>
#include <string>


/**
 * Event delivery class
 * @author m.kamoshida
 * @version 0.1
 */


/**
 * @brief The CicoEvent class
 */
class  CicoEvent {
public:
    enum E_EvType {
        Resv    = 0,                    // 
        Start   = 1,                    // start of state-machine
        Stop    = 2,                    // stop of state-machine
        History = 101,                  //
        DetectTheEnd = 201,             //
        User    = 1000,                 // user 
        MaxUser = 65535                 // user max
    };

    // type of value
    enum E_TOV {                        // Type the value of the guard condition
        EEvtGCUnset = 0,                //  unset
        EEvtGCInt,                      //  Integer number
        EEvtGCDouble,                   //  Floating point
        EEvtGCStr                       //  String
    };
    // Constructior
                CicoEvent();
                CicoEvent(unsigned short ev, void* adval = 0);
                CicoEvent(unsigned short ev, int gcv, void* adval = 0);
                CicoEvent(unsigned short ev, double gcv, void* adval = 0);
                CicoEvent(unsigned short ev, const std::string& gcv,
                          void* adval = 0);
                CicoEvent(const CicoEvent& oev);

    // destructor
    virtual     ~CicoEvent();

    unsigned short getEV() const;
    void        setEV(unsigned short ev);

    int         getGCVi() const;
    double      getGCVd() const;
    const std::string& getGCVs() const;
    void        setGCV(int gcv);
    void        setGCV(double gcv);
    void        setGCV(const std::string& gcv);

    E_TOV       getVTG() const;

    void*       getAdVal() const;
    void        setAdVal(void* adval);

    friend bool isEventMatch(const CicoEvent& a, const CicoEvent& b);

    friend bool operator==(const CicoEvent& a, const CicoEvent& b);
    friend bool operator!=(const CicoEvent& a, const CicoEvent& b);
    friend bool operator>(const CicoEvent& a, const CicoEvent& b);
    friend bool operator>=(const CicoEvent& a, const CicoEvent& b);
    friend bool operator<(const CicoEvent& a, const CicoEvent& b);
    friend bool operator<=(const CicoEvent& a, const CicoEvent& b);

    void operator=(const CicoEvent& s);

protected:
    unsigned short m_ev;                // event number
    E_TOV       m_tov;                  // Type the value of the guard condition
    int         m_gcvi;                 // store Integer number
    double      m_gcvd;                 // store Floating point
    std::string m_gcvs;                 // store String

private:
    void*       m_adVal;                // Additional information value

};

/**
 * @brief get event number
 * @return event number
 */
inline unsigned short CicoEvent::getEV() const
{
    return m_ev;                        // event number
}

/**
 * @brief register event number
 * @param ev event number
 */
inline void CicoEvent::setEV(unsigned short ev)
{
    m_ev = ev;                          // event number
}

/**
 * @brief get Comparison value of guard condition
 * @return Comparison value of guard condition
 */
inline int CicoEvent::getGCVi() const
{
    return m_gcvi;                      // Type the value of the guard condition
}

/**
 * @brief get Comparison value of guard condition
 * @return Comparison value of guard condition
 */
inline double CicoEvent::getGCVd() const
{
    return m_gcvd;                      // Type the value of the guard condition
}

/**
 * @brief get Comparison value of guard condition
 * @return Comparison value of guard condition
 */
inline const std::string& CicoEvent::getGCVs() const
{
    return m_gcvs;
}

/**
 * @brief get type of guard condition value
 * @return type of guard condition value
 */
inline CicoEvent::E_TOV CicoEvent::getVTG() const
{
    return m_tov;
}

/**
 * @brief register Comparison value of guard condition
 * @param gcv Comparison value of guard condition
 */
inline void CicoEvent::setGCV(int gcv)
{
    m_tov = EEvtGCInt;
    m_gcvi = gcv;
}

/**
 * @brief register Comparison value of guard condition
 * @param gcv Comparison value of guard condition
 */
inline void CicoEvent::setGCV(double gcv)
{
    m_tov = EEvtGCDouble;
    m_gcvd = gcv;
}

/**
 * @brief register Comparison value of guard condition
 * @param gcv Comparison value of guard condition
 */
inline void CicoEvent::setGCV(const std::string& gcv)
{
    m_tov = EEvtGCStr;
    m_gcvs = gcv;
}

/**
 * @brief get Additional information user
 * @return Additional information user
 */
inline void* CicoEvent::getAdVal() const
{
    return m_adVal;
}

/**
 * @brief register Additional information user
 * @param adval Additional information user
 */
inline void CicoEvent::setAdVal(void* adval)
{
    m_adVal = adval;
}

#endif // CICOEVENT_H
