/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
/**
 * @brief   Homescreen application history sequence manager
 *
 * @date    Sep-16-2013
 */
#ifndef CICOHSAPPHISTORYEXT_H
#define CICOHSAPPHISTORYEXT_H

#include <string>
#include <vector>
#include <list>
#include <utility>
#include "CicoHSAppHistory.h"

/**
 * ecore timer void data typedef
 */
typedef struct t_CHSAHE_data {
    int   id;
    void* pObj;
} CHSAHE_data_t;

/**
 * pid appid convert data item
 */
typedef std::pair<int, std::string> pairPidAppid;


class CicoHSAppHistoryExt :public CicoHSAppHistory
{
public:
    CicoHSAppHistoryExt();
    CicoHSAppHistoryExt(const char* user, const char* path, const char* flagpath);
    CicoHSAppHistoryExt(const std::string& user, const std::string& path,
                        const std::string& flagpath);
    virtual ~CicoHSAppHistoryExt();
    void setWaitTime(double msec);

    // operate
    virtual bool addAppHistory(const std::string& app);
    virtual bool delAppHistory(const std::string& app);
    virtual bool moveHistoryHead(const std::string& app);

    // AUL I/F
    int appLaunch(int pid, const char* appid);
    int appDead(int pid);

    // pid to appid converter
    bool getAppid(int pid, std::string& appid);

    // flick operation I/F
    void selectApp(int pid);
    void selectApp(const std::string& appid);
    void selectApp(const char* appid);

    // time out selected app
    void determined(CHSAHE_data_t* data);

    // touch operation I/F
    void activeApp(int pid);
    void activeApp(const std::string& appid);
    void activeApp(const char* appid);

protected:
    enum {
        COUNTER_START = 1,
        COUNTER_MAX = 30000
    };
    virtual void setHandler();
    int resetCounter();
    int getCounter();
    int nextCounter();
    bool chgChk();

protected:
    std::vector<pairPidAppid> m_vppa;
    int   m_cnt;
    std::string m_currentAppid;
    std::list<CHSAHE_data_t*> m_lCdt;
    double   m_waitTimer;    // ex.) 1sec = 1.0
    std::string m_bkTmp;
};

/**
 * @brief counter reset
 * @ret reset counter value
 */
inline int CicoHSAppHistoryExt::resetCounter()
{
    m_cnt = COUNTER_START;
    return m_cnt;
}

/**
 * @brief get counter value
 * @ret  counter value
 */
inline int CicoHSAppHistoryExt::getCounter()
{
    return m_cnt;
}

/**
 * @brief next counter value
 * @ret next counter value
 */
inline int CicoHSAppHistoryExt::nextCounter()
{
    m_cnt++;
    if (COUNTER_MAX < m_cnt) {
        resetCounter();
    }
    return m_cnt;
}

/**
 * @brief set wait timer val
 * @param sec wait time ex.) 1.0 == 1sec
 */
inline void CicoHSAppHistoryExt::setWaitTime(double sec)
{
    m_waitTimer = sec;
}

#endif // CICOHSAPPHISTORYEXT_H

