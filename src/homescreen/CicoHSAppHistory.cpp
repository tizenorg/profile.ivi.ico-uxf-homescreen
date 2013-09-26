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
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <cstdio>
#include <aul/aul.h>
#include <sys/stat.h>
#include "CicoHSAppHistory.h"
#include "CicoLog.h"

using namespace std;

/**
 * @brief constructor
 */
CicoHSAppHistory::CicoHSAppHistory()
{
    ICO_DBG("constructor");
    m_user.clear();
    m_path.clear();
    m_flagPath.clear();
    homeSwipe();
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistory::CicoHSAppHistory(const char* user, const char* path,
                                   const char* flagpath)
    :m_user(user), m_path(path), m_flagPath(flagpath)
{
    ICO_DBG("constructor %s, %s, %s", user, path, flagpath);
    homeSwipe();
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistory::CicoHSAppHistory(const string& user, const string& path,
                                   const string& flagpath)
    :m_user(user), m_path(path), m_flagPath(flagpath)
{
    ICO_DBG("constructor %s, %s, %s", user.c_str(), path.c_str(), flagpath.c_str());
    homeSwipe();
}

/**
 * @brief destructor
 *
 */
CicoHSAppHistory::~CicoHSAppHistory()
{
    writeAppHistory();
    m_user.clear();
    m_path.clear();
    m_flagPath.clear();
}

/**
 * @brief added application id to list
 * @param app target application id
 * @ret bool
 * @retval true added ok
 * @retval false added ng
 */
bool CicoHSAppHistory::addAppHistory(const string& app)
{
    const char* tgt = app.c_str();
    ICO_TRA("start(%s, %d)", tgt, (int)m_appHistoryList.size());
    if (true == filterChk(m_filterM, tgt)) {
       ICO_TRA("end false");
       return false;
    }
    m_appHistoryList.remove(app);
    m_appHistoryList.push_front(app);
    homeSwipe();
    ICO_TRA("end (true, %d)", (int)m_appHistoryList.size());
    return true;
}

/**
 * @brief delete application id to list
 * @param app target application id
 * @ret bool
 * @retval true added ok
 * @retval false added ng
 */
bool CicoHSAppHistory::delAppHistory(const string& app)
{
    int sz = m_appHistoryList.size();
    ICO_TRA("start(%s, %d)", app.c_str(), sz);
    bool r = false;
    if (false == isFlag()) {
        m_appHistoryList.remove(app);
        int sztmp = sz;
        sz = m_appHistoryList.size();
        if (sz != sztmp) {
            r = true;
            homeSwipe();
        }
    }
    ICO_TRA("end(%s, %d)", r? "true": "false", sz);
    return r;
}

/**
 * @brief priority change list
 * @param app target application id
 * @ret bool
 * @retval true added ok
 * @retval false added ng
 */
bool CicoHSAppHistory::moveHistoryHead(const string& app)
{
    ICO_TRA("start(%s, %d)", app.c_str(), (int)m_appHistoryList.size());
    bool r = false;
    list<string>::iterator p = m_appHistoryList.begin();
    for (; p != m_appHistoryList.end(); ++p) {
        if (app == *p) {
            r = true; // app is exist
            break; // break of for p
        }
    }
    if (true == r) { // application exist ?
        r = addAppHistory(app);
        homeSwipe();
    }
    ICO_TRA("end(%s %d)", r? "true": "false", (int)m_appHistoryList.size());
    return r;
}

/**
 * @brief is flag status check(system-controller control flag)
 * @ret bool
 * @retval true: flag on (flag file exist)
 * @retval false flag off
 */
bool CicoHSAppHistory::isFlag()
{
    ICO_DBG("start");
    struct stat stat_buf;
    if (0 != stat(m_flagPath.c_str(), &stat_buf)) {
        ICO_DBG("end false");
        return false;
    }
    ICO_DBG("end true");
    return true;
}

/**
 * @brief flag off(force)
 * @ret bool
 * @retval true flag on -> off
 * @retval false  flag is off
 */
bool CicoHSAppHistory::force_flagoff()
{
    ICO_DBG("start");
    if (true == isFlag()) {
        remove(m_flagPath.c_str());
        ICO_DBG("end true");
        return true;
    }
    ICO_DBG("end false");
    return false;
}

/**
 * @brief write application history
 */
bool CicoHSAppHistory::writeAppHistory()
{
    ICO_DBG("start(%d)", (int)m_appHistoryList.size());
    if (true == m_path.empty()) {
        ICO_DBG("end false file none");
        return false;
    }
    vector<string> vs;
    list<string>::iterator p = m_appHistoryList.begin();
    for (; p != m_appHistoryList.end(); ++p) {
        if (true == (*p).empty()) {
            continue;
        }
        if (true == filterChk(m_filterW, (*p).c_str())) {
            continue;   // continue of for p
        }
        vs.push_back(*p);
    }
    if (0 == vs.size()) {
        remove(m_path.c_str());
        ICO_DBG("end remove file");
        return true;
    }
    ofstream ofs;
    ofs.open(m_path.c_str(), ios::trunc);
    vector<string>::iterator it = vs.begin();
    for (; it != vs.end(); ++it) {
        ofs << *it << endl;
    }
    ofs.close();
    ICO_DBG("end write size(%d)", (int)vs.size());
    return false;
}

/**
 * @brief string match check
 * @param filter string chek targets
 * @param tgt string chek target
 * @return bool
 * @retval true match
 * @retval false no match
 */
bool CicoHSAppHistory::filterChk(vector<string>& filter, const char* tgt) const
{
    ICO_DBG("start %d", filter.size());
    bool r = false;
    vector<string>::iterator fil = filter.begin(); // iterator set begin
    while(fil != filter.end()) {  // loop to the last data
        if (*fil == tgt) {
            r = true;
            break;  // break of while fil
        }
        ++fil;
    }
    ICO_DBG("end %s %s", tgt, r? "true": "false");
    return r;
}

/**
 * @brief next swipe app and appid get
 * @ret  appid
 * @retval empty is stopped
 */
const string& CicoHSAppHistory::nextSwipe()
{
    m_swipeStr.clear();
    if (m_swipeCurr == m_appHistoryList.begin()) {
        ICO_DBG("Next empty! stoped");
        return m_swipeStr;
    }
    --m_swipeCurr;
    m_swipeStr = *m_swipeCurr;
    ICO_DBG("Next %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief Previous swipe app and appid get
 * @ret  appid
 * @retval empty is stopped
 */
const string& CicoHSAppHistory::prevSwipe()
{
    m_swipeStr.clear();
    if (m_swipeCurr == m_appHistoryList.end()) {
        ICO_DBG("Prev empty! stoped");
        return m_swipeStr;
    }
    ++m_swipeCurr;
    if (m_swipeCurr == m_appHistoryList.end()) {
        ICO_DBG("Prev empty! stoped");
        return m_swipeStr;
    }
    m_swipeStr = *m_swipeCurr;
    ICO_DBG("Prev %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief home position set
 * @ret appid
 * @retval empty is Not running application
 */
const string& CicoHSAppHistory::homeSwipe()
{
    m_swipeStr.clear();
    m_swipeCurr = m_appHistoryList.begin();
    if (m_appHistoryList.end() != m_swipeCurr) {
        m_swipeStr = *m_swipeCurr;
    }
    return m_swipeStr;
}
