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
#include <Ecore.h>

#include "CicoHSAppHistory.h"
#include "CicoHSAppHistoryExt.h"
#include "Cico_aul_listen_app.h"
#include "CicoLog.h"

using namespace std;

#define DEFAULT_WAIT_TIME 5.0

/**
 * @brief applications are launched. callback function
 * @param pid
 * @param data user data
 * @ret   int
 * @retval 0: success
 * @retval not 0: fail
 */
static int ico_CHSAHapp_launch_handler(int pid, void *data)
{
    if ((NULL == data) || (0 == data)) {
        ICO_DBG("user data is NULL");
        return -1;
    }
    CicoHSAppHistoryExt *o = (CicoHSAppHistoryExt*) data;
    char appid[255];
    aul_app_get_appid_bypid(pid, appid, sizeof(appid));
    return o->appLaunch(pid, appid);
}

/**
 * @brief applications die. callback function
 * @param pid
 * @param data user data
 * @ret   int
 * @retval 0: success
 * @retval not 0: fail
 */
static int ico_CHSAHapp_dead_handler(int pid, void *data)
{
    if ((NULL == data) || (0 == data)) {
        ICO_DBG("user data is NULL");
        return -1;
    }
    CicoHSAppHistoryExt *o = (CicoHSAppHistoryExt*) data;
    return o->appDead(pid);
}

/**
 * @brief aplication select determined time out
 * @param data user data
 * @ret Eina_Bool
 * @retval ECORE_CALLBACK_DONE(EINA_FALSE) stop event handling
 */
static Eina_Bool ico_CHSAHE_timeout(void *data)
{
    CHSAHE_data_t* data_t = (CHSAHE_data_t*) data;
    CicoHSAppHistoryExt* obj = (CicoHSAppHistoryExt*) data_t->pObj;
    if ((NULL == obj) || (0 == obj)) {
        ICO_DBG("INVALID PARAMETER %x, %d, %x", data_t, data_t->id, obj);
        return ECORE_CALLBACK_DONE;
    }
    obj->determined(data_t);
    return ECORE_CALLBACK_DONE;
}

/**
 * @brief constructor
 */
CicoHSAppHistoryExt::CicoHSAppHistoryExt()
    :CicoHSAppHistory()
{
    resetCounter();
    setHandler();
    setWaitTime(DEFAULT_WAIT_TIME);
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistoryExt::CicoHSAppHistoryExt(const char* user, const char* path,
                                         const char* flagpath)
    :CicoHSAppHistory(user, path, flagpath)
{
    resetCounter();
    setHandler();
    setWaitTime(DEFAULT_WAIT_TIME);
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistoryExt::CicoHSAppHistoryExt(const string& user,
                                         const string& path,
                                         const string& flagpath)
    :CicoHSAppHistory(user, path, flagpath)
{
    resetCounter();
    setHandler();
    setWaitTime(DEFAULT_WAIT_TIME);
}

/**
 * @brief destructor
 *
 */
CicoHSAppHistoryExt::~CicoHSAppHistoryExt()
{
}

/**
 * @brief set aul launch/dead handler
 */
void CicoHSAppHistoryExt::setHandler()
{
    ICO_DBG("start");
    aul_listen_app_launch_signal_add(ico_CHSAHapp_launch_handler, (void*)this);
    aul_listen_app_dead_signal_add(ico_CHSAHapp_dead_handler, (void*)this);
    ICO_DBG("end %x", this);
}

/**
 * @brief applications are launched. callback function
 * @param pid 
 * @param appid
 * @ret   int
 * @retval 0: success
 * @retval not 0: fail
 */
int CicoHSAppHistoryExt::appLaunch(int pid, const char* appid)
{
    ICO_DBG("start %d, %s", pid, appid);
    m_vppa.push_back(pairPidAppid(pid, appid));
    addAppHistory(appid);
    ICO_DBG("end 0");
    return 0;
}

/**
 * @brief applications die. callback function
 * @param pid
 * @ret   int
 * @retval 0: success
 * @retval not 0: fail
 */
int CicoHSAppHistoryExt::appDead(int pid)
{
    ICO_DBG("start %d", pid);
    bool bBINGO = false;
    string appid;
    vector<pairPidAppid>::iterator it = m_vppa.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        if ((*it).first == pid) {
            appid = (*it).second;
            m_vppa.erase(it);
            bBINGO = true;
            break;  // break of while it
        }
        ++it;   // next data
    }
    if (false == bBINGO) {
        ICO_DBG("end not 0");
        return -1;
    }
    bool bExist = false;
    it = m_vppa.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        if ((*it).second == appid) {    // maluti running ?
            bExist = true;   // appid exist
            ICO_DBG("exist %s is %d", appid.c_str(), (*it).first);
            break;  // break of while it
        }
        ++it;   // next data
    }

    if (true == bExist) {
        ICO_DBG("end 0");
        return 0;
    }

    delAppHistory(appid);
    ICO_DBG("end 0");
    return 0;
}

/**
 * @brief pid to appid convert
 * @param pid
 * @param appid
 * @ret bool
 * @retval true convert success
 * @retval false convert fail
 */
bool CicoHSAppHistoryExt::getAppid(int pid, string& appid)
{
    bool r = false;
    vector<pairPidAppid>::iterator it = m_vppa.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        if ((*it).first == pid) {
            appid = (*it).second;
            r = true;
            ICO_DBG("pid to appid %d -> %s", pid, appid.c_str());
            break;  // break of while it
        }
        ++it;   // next data
    }
    ICO_DBG("CHSAHgetAppid %s", r? "true": "false");
    return r;
}

/**
 * @brief select app
 * @param pid
 */
void CicoHSAppHistoryExt::selectApp(int pid)
{
    string appid;
    if (false == getAppid(pid, appid)) {
        ICO_DBG("NG pis(%d)", pid);
        return;
    }
    selectApp(appid);
}

/**
 * @brief select app
 * @param appid
 */
void CicoHSAppHistoryExt::selectApp(const string& appid)
{
    if (false == appid.empty()) {
        selectApp(appid.c_str());
    }
}

/**
 * @brief select app
 * @param appid
 */
void CicoHSAppHistoryExt::selectApp(const char* appid)
{
    CHSAHE_data_t* data = new CHSAHE_data_t;
    data->id = nextCounter();
    data->pObj = this;
    ICO_DBG("timer set(%x) %s, %d", data, appid, data->id);
    ecore_timer_add(m_waitTimer, ico_CHSAHE_timeout, data);
    m_lCdt.push_back(data);
    m_currentAppid = appid;
}

/**
 * @brief determined applicationselect
 * @param data is id value and delete(free) target
 */
void CicoHSAppHistoryExt::determined(CHSAHE_data_t* data)
{
    ICO_DBG("start %x.id(%d)", data, data->id);
    int id = data->id;       // counter id get
    int sz1 = m_lCdt.size(); // size get
    m_lCdt.remove(data);     // delete request
    if ((int)m_lCdt.size() != sz1) { // is size change
        ICO_DBG("free %x", data);
        delete data;        // area free
    }
    if (true == m_currentAppid.empty()) {
        ICO_DBG("end appid lost");
        return;
    }
    if (id == getCounter()) {
        ICO_DBG("Match id");
        moveHistoryHead(m_currentAppid);
        m_currentAppid.clear();
    }
    ICO_DBG("end");
}

/**
 * @brief active app receve event
 * @param pid
 */
void CicoHSAppHistoryExt::activeApp(int pid)
{
    string appid;
    if (false == getAppid(pid, appid)) {
        ICO_DBG("NG pid(%d)", pid);
        return;
    }
    activeApp(appid);
}

/**
 * @brief active app receve event
 * @param appid
 */
void CicoHSAppHistoryExt::activeApp(const string& appid)
{
    if (false == appid.empty()) {
        activeApp(appid.c_str());
    }
}

/**
 * @brief active app receve event
 * @param appid
 */
void CicoHSAppHistoryExt::activeApp(const char* appid)
{
    ICO_DBG("start %s", appid);
    if (true == m_currentAppid.empty()) {    
#if 0
        moveHistoryHead(appid);
#endif
        ICO_DBG("end current none");
        return;
    }
    if (m_currentAppid == appid) { // touch operate app
        moveHistoryHead(m_currentAppid);
        m_currentAppid.clear();
        ICO_DBG("end touch operate app");
        return;
    }
    ICO_DBG("end  no match curr(%s)", m_currentAppid.c_str());
#if 0
    moveHistoryHead(appid);
#endif
#if 0
    m_currentAppid.clear();
#endif
    return;
}

bool CicoHSAppHistoryExt::chgChk()
{
    string tmp(m_bkTmp);
    m_bkTmp.clear();
    int i = 0; 
    list<string>::iterator p = m_appHistoryList.begin();
    while (p != m_appHistoryList.end()) {
        if (i != 0) {
            m_bkTmp += ',';
        }
        m_bkTmp += *p;
        i++;
        ++p;
    }
    if (tmp == m_bkTmp) {
        ICO_DBG("false, %s", m_bkTmp.c_str());
        return false;
    }
    ICO_DBG("true, %d %d", m_bkTmp.size(), tmp.size());
    return true;
}
bool CicoHSAppHistoryExt::addAppHistory(const string& app)
{
    bool r;
    r = CicoHSAppHistory::addAppHistory(app);
    if (true == chgChk()) {
        writeAppHistory();
    }
    return r;
}
bool CicoHSAppHistoryExt::delAppHistory(const string& app)
{
    bool r;
    r = CicoHSAppHistory::delAppHistory(app);
    if (true == chgChk()) {
        writeAppHistory();
    }
    return r;
}
bool CicoHSAppHistoryExt::moveHistoryHead(const string& app)
{
    bool r;
    r = CicoHSAppHistory::moveHistoryHead(app);
    if (true == chgChk()) {
        writeAppHistory();
    }
    return r;
}
