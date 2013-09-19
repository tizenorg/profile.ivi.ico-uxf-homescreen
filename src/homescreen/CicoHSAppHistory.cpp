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
#include <string>
#include <list>
#include "CicoHSAppHistory.h"
#include "CicoLog.h"

using namespace std;

/**
 * @brief constructor
 */
CicoHSAppHistory::CicoHSAppHistory()
{
    m_user.clear();
    m_path.clear();
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list
 */
CicoHSAppHistory::CicoHSAppHistory(const char* user, const char* path)
    :m_user(user), m_path(path)
{
}

/**
 * @brief destructor
 *
 */
CicoHSAppHistory::~CicoHSAppHistory()
{
    m_user.clear();
    m_path.clear();
}

/**
 * @brief added application id to list
 * @param app target application id
 */
void CicoHSAppHistory::addAppHistory(const string& app)
{
    ICO_TRA("CicoHSAppHistory::addAppHistory(%s) start(%d)", app.c_str(),
            (int)m_appHistoryList.size());
    delAppHistory(app);
    if (0 == m_appHistoryList.size()) {
        m_appHistoryList.push_back(app);
    }
    else {
        list<string>::iterator p = m_appHistoryList.begin();
        m_appHistoryList.insert(p, 1, app);
    }
    ICO_TRA("CicoHSAppHistory::addAppHistory() end()",
            (int)m_appHistoryList.size());
}

/**
 * @brief delete application id to list
 * @param app target application id
 */
void CicoHSAppHistory::delAppHistory(const string& app)
{
    ICO_TRA("CicoHSAppHistory::delAppHistory(%s) start(%d)", app.c_str(),
            (int)m_appHistoryList.size());
    m_appHistoryList.remove(app);
    ICO_TRA("CicoHSAppHistory::delAppHistory() end(%d)",
            (int)m_appHistoryList.size());
}

/**
 * @brief priority change list
 * @param app target application id
 */
void CicoHSAppHistory::moveHistoryHead(const string& app)
{
    ICO_TRA("CicoHSAppHistory::moveHistoryHead(%s) start(%d)", app.c_str(),
            (int)m_appHistoryList.size());
//    delAppHistory(app);
    bool b = false;
    list<string>::iterator p = m_appHistoryList.begin();
    for (; p != m_appHistoryList.end(); ++p) {
        if (app == *p) {
            b = true; // app is exist
            break; // break of for p
        }
    }
    if (true == b) { // application exist ?
        addAppHistory(app);
    }
    ICO_TRA("CicoHSAppHistory::moveHistoryHead() end");
}

