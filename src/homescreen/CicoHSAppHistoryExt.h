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

/**
 * order of the start-up window
 */
typedef std::pair<bool, pairPidAppid> pairBoolPidAppid;

class CicoHSAppHistoryExt :public CicoHSAppHistory
{
public:
    CicoHSAppHistoryExt();
    CicoHSAppHistoryExt(const char* user, const char* path, const char* pathD,
                        const char* flagpath);
    CicoHSAppHistoryExt(const std::string& user, const std::string& path,
                        const std::string& pathD, const std::string& flagpath);
    virtual ~CicoHSAppHistoryExt();
    void setWaitTime(double msec);

    // operate
    virtual bool addAppHistory(const std::string& app);
    virtual bool delAppHistory(const std::string& app);
    virtual bool moveHistoryHead(const std::string& app);
    const std::string& getNearHistory();

    // AUL I/F
    int appLaunch(int pid, const char* appid, int aulstt);
    int appDead(int pid);

    // pid to appid converter
    bool getAppid(int pid, std::string& appid);

    // swipe operation I/F
    void selectApp(int pid);
    void selectApp(const std::string& appid);
    void selectApp(const char* appid);
    const std::string& getSelectApp() const;
    void clearSelectApp();

    // time out selected app
    void determined(CHSAHE_data_t* data);

    // touch operation I/F
    void activeApp(int pid);
    void activeApp(const std::string& appid);
    void activeApp(const char* appid);

    // swipe operate
    const std::string& prevSwipe();
    const std::string& nextSwipe();
    const std::string& getSwipeCurrentAppid();
    const std::string& homeSwipe();

    void update_appid(int pid, std::string& appid, int& aulstt);
    bool update_pairPidAppid(std::string& appid);
    void update_appid();

    // order of the start-up window
    void startupCheckAdd(int pid, const std::string& appid, bool d = false);
    bool isStartupChecking() const;
    void startupEntryFinish(int pid);
    void startupEntryFinish(const std::string& appid);
    bool isFinish();
    void stopStartupCheck();
    const std::string& getLastStartupAppid() const;
    const std::string& getSubDispAppid() const;

    bool chgChk();
protected:
    enum {
        COUNTER_START = 1,
        COUNTER_MAX = 30000
    };
    virtual void setHandler();
    int resetCounter();
    int getCounter();
    int nextCounter();

protected:
    std::vector<pairPidAppid> m_vppa;
    std::vector<int> m_aulstt;
    int   m_cnt;
    std::string m_waitSelApp;
    std::list<CHSAHE_data_t*> m_lCdt;
    double   m_waitTimer;    // ex.) 1sec = 1.0
    std::string m_bkTmp;
    std::vector<pairBoolPidAppid> m_vpbpa;
    std::string m_lastStartupApp;
    std::string m_subDispApp;
    std::string m_empty;
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

/**
 * @brief get start-up checking state
 * @ret  bool
 * @retval true cheking
 * @retval false no checking
 */
inline bool CicoHSAppHistoryExt::isStartupChecking() const
{
    if (0 == m_vpbpa.size()) {
        return  false;
    }
    return true;
}

/**
 * @brief get last start-up appid
 * @retuen appid Valid only application startup
 * @retval empty is application startup finish or no have history
*/

inline const std::string& CicoHSAppHistoryExt::getLastStartupAppid() const
{
    return m_lastStartupApp;
}

/**
 * @brief get history sub display appid
 * @return appid 
 * @retval empty is no have history sub display
 */
inline const std::string& CicoHSAppHistoryExt::getSubDispAppid() const
{
    return m_subDispApp;
}

/**
 * @brief get select App entry appid
 * @ret appid
 * @retval empty is time out or none select
 */
inline const std::string& CicoHSAppHistoryExt::getSelectApp() const
{
    return m_waitSelApp;
}

/**
 * @brief select appid clear
 */
inline void CicoHSAppHistoryExt::clearSelectApp()
{
    m_waitSelApp.clear();
}

#endif // CICOHSAPPHISTORYEXT_H

