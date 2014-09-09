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
#include "CicoHomeScreenCommon.h"
#include "CicoHomeScreen.h"
#include <ico_log.h>

using namespace std;

#define DEFAULT_WAIT_TIME 5.0

extern int Xaul_app_get_appid_bypid(int pid, char *appid, int len);

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
    memset(appid, 0, sizeof(appid));
    int iR = Xaul_app_get_appid_bypid(pid, appid, sizeof(appid));
    ICO_PRF("CHG_APP_STA 1.notice  app=%s, pid=%d, rval=%d", appid, pid, iR);
    return o->appLaunch(pid, appid, iR);
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
 * @retval ECORE_CALLBACK_CANCEL(EINA_FALSE) stop event handling
 */
static Eina_Bool ico_CHSAHE_timeout(void *data)
{
    CHSAHE_data_t* data_t = (CHSAHE_data_t*) data;
    CicoHSAppHistoryExt* obj = (CicoHSAppHistoryExt*) data_t->pObj;
    if ((NULL == obj) || (0 == obj)) {
        ICO_DBG("INVALID PARAMETER %x, %d, %x", data_t, data_t->id, obj);
        return ECORE_CALLBACK_CANCEL;
    }
    obj->determined(data_t);
    return ECORE_CALLBACK_CANCEL;
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
    m_lastStartupApp.clear();
    m_subDispApp.clear();
    m_empty.clear();
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistoryExt::CicoHSAppHistoryExt(const char* user, const char* path,
                                         const char* pathD, const char* flagpath)
    :CicoHSAppHistory(user, path, pathD, flagpath)
{
    resetCounter();
    setHandler();
    setWaitTime(DEFAULT_WAIT_TIME);
    m_lastStartupApp.clear();
    m_subDispApp.clear();
    m_empty.clear();
}

/**
 * @brief constructor
 * @param user login user name
 * @param path application list file path
 * @param flagpath control flag file path
 */
CicoHSAppHistoryExt::CicoHSAppHistoryExt(const string& user,
                                         const string& path,
                                         const string& pathD,
                                         const string& flagpath)
    :CicoHSAppHistory(user, path, pathD, flagpath)
{
    resetCounter();
    setHandler();
    setWaitTime(DEFAULT_WAIT_TIME);
    m_lastStartupApp.clear();
    m_subDispApp.clear();
    m_empty.clear();
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
int CicoHSAppHistoryExt::appLaunch(int pid, const char* appid, int aulstt)
{
    ICO_DBG("start %d, %s", pid, appid);
    m_vppa.push_back(pairPidAppid(pid, appid));
    m_aulstt.push_back(aulstt);
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
    vector<int>::iterator itas = m_aulstt.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        if ((*it).first == pid) {
            appid = (*it).second;
            m_vppa.erase(it);
            m_aulstt.erase(itas);
            bBINGO = true;
            break;  // break of while it
        }
        ++it;   // next data
        ++itas;
    }
    if (false == bBINGO) {
        ICO_DBG("end not 0");
        return -1;
    }
    bool bExist = false;
    it = m_vppa.begin(); // iterator set begin
    itas = m_aulstt.begin();
    while(it != m_vppa.end()) {  // loop to the last data
        update_appid((*it).first, (*it).second, *itas);
        if ((*it).second == appid) {    // maluti running ?
            bExist = true;   // appid exist
            ICO_DBG("exist %s is %d", appid.c_str(), (*it).first);
            break;  // break of while it
        }
        ++it;   // next data
        ++itas;   // next data
    }

    vector<pairBoolPidAppid>::iterator itvpbpa = m_vpbpa.begin();
    while (itvpbpa != m_vpbpa.end()) {
        if ((*itvpbpa).second.first == pid) {
            m_vpbpa.erase(itvpbpa);
            break;
        }
        ++itvpbpa;
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
    vector<int>::iterator itas = m_aulstt.begin(); // iterator set begin
    while(it != m_vppa.end()) {  // loop to the last data
        update_appid((*it).first, (*it).second, *itas);
        if ((*it).first == pid) {
            appid = (*it).second;
            r = true;
            ICO_DBG("pid to appid %d %s", pid, appid.c_str());
            break;  // break of while it
        }
        ++it;   // next data
        ++itas;
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
    m_waitSelApp = appid;
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
    if (true == m_waitSelApp.empty()) {
        ICO_DBG("end appid lost");
        return;
    }
    if (id == getCounter()) {
        string appid(m_waitSelApp);
        ICO_DBG("Match id");
        moveHistoryHead(m_waitSelApp);
        m_waitSelApp.clear();
        if (m_hs) {
            m_hs->requestActivationAppid(appid);
        }
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
    if (strcmp(appid, ICO_HS_PROC_DEFAULT_HOMESCREEN) == 0) {
        return;
    }
    ICO_DBG("start %s", appid);

    if (true == m_waitSelApp.empty()) {
        moveHistoryHead(appid);
        ICO_DBG("end current none");
        return;
    }
    if (m_waitSelApp == appid) { // touch operate app
        moveHistoryHead(m_waitSelApp);
        m_waitSelApp.clear();
        ICO_DBG("end touch operate app");
        return;
    }
    ICO_DBG("end no match select app(%s)", m_waitSelApp.c_str());
    if (true == moveHistoryHead(appid)) {
        m_waitSelApp.clear();
    }
    return;
}

bool CicoHSAppHistoryExt::chgChk()
{
    string tmp(m_bkTmp);
    m_bkTmp.clear();
    int i = 0;
    const char* pAppidSubD = getAppidSubDispBySystem();
    list<string>::iterator p = m_appHistoryList.begin();
    while (p != m_appHistoryList.end()) {
        if (i != 0) {
            m_bkTmp += ',';
        }
        m_bkTmp += *p;
        if (0 == (*p).compare(pAppidSubD)) {
            m_bkTmp += "(S)";
        }
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
    if (true == isStartupChecking()) {
        if (0 == m_lastStartupApp.compare(app)) {
            m_lastStartupApp.clear();
        }
        if (0 == m_subDispApp.compare(app)) {
            m_subDispApp.clear();
        }
        startupEntryFinish(app);
        if (true == isFinish()) {
            if (NULL != m_hs) {
                m_hs->finishStartup();
            }
        }
    }
    return r;
}
/**
 * @brief move history
 * @param appid
 */
bool CicoHSAppHistoryExt::moveHistoryHead(const string& app)
{
    bool r;
    r = CicoHSAppHistory::moveHistoryHead(app);
    if (true == chgChk()) {
        writeAppHistory();
    }
    return r;
}

/**
 * @breief update appid
 * @param pid
 * @param appid  update appid store string
 * @param aulstt value is AUL_R_OK then no update
 *               value is no AUL_R_OK then update
 *               rerurn value is AUL_R_OK(success) / not AUL_R_OK(fail)
 */
void CicoHSAppHistoryExt::update_appid(int pid, string& appid, int& aulstt)
{
    if (AUL_R_OK == aulstt) {
        return;
    }
    ICO_TRA("update start %d", aulstt);
    char buf[255];
    buf[0] = '\0'; // STOP CODE
    aulstt = aul_app_get_appid_bypid(pid, buf, sizeof(buf));
    if (AUL_R_OK == aulstt) {
        ICO_DBG("update appid %s -> %s", appid.c_str(), buf);
        appid = buf;
    }
    ICO_TRA("update end %d", aulstt);
    return;
}

/**
 * @brief next swipe app and appid get
 * @ret  appid
 * @retval empty is stopped
 */
const string& CicoHSAppHistoryExt::nextSwipe()
{
    if (true == isStartupChecking()) {
        ICO_DBG("Next empty! stoped");
        return m_empty;
    }
    if (m_swipeCurr == m_appHistoryList.begin()) {
        ICO_DBG("Next empty! stoped");
        return m_empty;
    }
    --m_swipeCurr;
    m_swipeStr = *m_swipeCurr;
    if (true == update_pairPidAppid(m_swipeStr)) {
        *m_swipeCurr = m_swipeStr;
    }
    const char* pAppidSubD = getAppidSubDispBySystem();
    if (0 == m_swipeStr.compare(pAppidSubD)) {
        if (m_swipeCurr == m_appHistoryList.begin()) {
            ++m_swipeCurr; // back
            m_swipeStr = *m_swipeCurr;
            ICO_DBG("begin is sub display appid");
            return m_empty;
        }
        --m_swipeCurr;  // one more
        m_swipeStr = *m_swipeCurr;
        if (true == update_pairPidAppid(m_swipeStr)) {
            *m_swipeCurr = m_swipeStr;
        }
    }
    ICO_DBG("Next %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief Previous swipe app and appid get
 * @ret  appid
 * @retval empty is stopped
 */
const string& CicoHSAppHistoryExt::prevSwipe()
{
    if (true == isStartupChecking()) {
        ICO_DBG("Next empty! stoped");
        return m_empty;
    }
    if (m_swipeCurr == m_appHistoryList.end()) {
        ICO_DBG("Next empty! stoped");
        return m_empty;
    }
    ++m_swipeCurr;
    if (m_swipeCurr == m_appHistoryList.end()) {
        --m_swipeCurr;
        ICO_DBG("Prev empty! stoped");
        return m_empty;
    }
    m_swipeStr = *m_swipeCurr;
    if (true == update_pairPidAppid(m_swipeStr)) {
        *m_swipeCurr = m_swipeStr;
    }
    const char* pAppidSubD = getAppidSubDispBySystem();
    if (0 == m_swipeStr.compare(pAppidSubD)) {
        ++m_swipeCurr;
        if (m_swipeCurr == m_appHistoryList.end()) {
            --m_swipeCurr;
            --m_swipeCurr;
            m_swipeStr = *m_swipeCurr;
            if (true == update_pairPidAppid(m_swipeStr)) {
                *m_swipeCurr = m_swipeStr;
            }
            ICO_DBG("old history appid is sub display");
            return m_empty;
        }
        m_swipeStr = *m_swipeCurr;
        if (true == update_pairPidAppid(m_swipeStr)) {
            *m_swipeCurr = m_swipeStr;
        }
    }
    ICO_DBG("Prev %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief home position set
 * @ret appid
 * @retval empty is Not running application
 */
const string& CicoHSAppHistoryExt::homeSwipe()
{
    m_swipeStr.clear();
    m_swipeCurr = m_appHistoryList.begin();
    if (m_appHistoryList.end() != m_swipeCurr) {
        m_swipeStr = *m_swipeCurr;
        if (true == update_pairPidAppid(m_swipeStr)) {
            *m_swipeCurr = m_swipeStr;
        }
        const char* pAppidSubD = getAppidSubDispBySystem();
        if (0 == m_swipeStr.compare(pAppidSubD)) {
            if (1 < m_appHistoryList.size()) {
                prevSwipe();
            }
            else {
                m_swipeStr.clear();
            }
        }
    }
    ICO_DBG("swipe home is %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief get near entry history
 * @ret appid
 */
const string& CicoHSAppHistoryExt::getNearHistory()
{
    ICO_TRA("start");
    list<string>::iterator p = m_appHistoryList.begin();
    if (p == m_appHistoryList.end()) {
        // NO history
        ICO_TRA("end no history I");
        return m_empty;
    }
    const char* pAppidSubD = getAppidSubDispBySystem();
    if (0 != (*p).compare(pAppidSubD)) {
        // on history
        ICO_TRA("end near history is %s", (*p).c_str());
        return *p;
    }
    ++p;
    if (p == m_appHistoryList.end()) {
        // NO history
        ICO_TRA("end no history II");
        return m_empty;
    }
    ICO_TRA("end near history is %s", (*p).c_str());
    return *p;
}

/**
 * @brief get swipe current appid
 * @ret appid
 */
const string& CicoHSAppHistoryExt::getSwipeCurrentAppid()
{
    m_swipeStr.clear();
    if (m_appHistoryList.end() != m_swipeCurr) {
        m_swipeStr = *m_swipeCurr;
        if (true == update_pairPidAppid(m_swipeStr)) {
            *m_swipeCurr = m_swipeStr;
        }
    }
    ICO_DBG("current %s", m_swipeStr.c_str());
    return m_swipeStr;
}

/**
 * @brief update appid by vector<pairPidAppid>
 * @param appid target appid and update appide store
 * @ret bool
 * @retval true update appid
 * @retval false no update
 */
bool CicoHSAppHistoryExt::update_pairPidAppid(string& appid)
{
    ICO_DBG("start(%s)", appid.c_str());
    bool r = false; // return value is no update
    vector<pairPidAppid>::iterator itV = m_vppa.begin();
    vector<int>::iterator itAS = m_aulstt.begin();
    while (itV != m_vppa.end()) {
        if (AUL_R_OK != *itAS) {
            if (0 == appid.compare((*itV).second)) {
                update_appid((*itV).first, (*itV).second, *itAS);
                if (AUL_R_OK == *itAS) {
                    appid = (*itV).second; // get update appid
                    r = true;  // return value is update
                }
                break;
            }
        }
        ++itV;
        ++itAS;
    }
    ICO_DBG("end(%s)", appid.c_str());
    return r;
}

/**
 * @brief update appid all
 */
void CicoHSAppHistoryExt::update_appid()
{
    ICO_DBG("start");
    vector<pairPidAppid>::iterator itV = m_vppa.begin();
    vector<int>::iterator itAS = m_aulstt.begin();
    while (itV != m_vppa.end()) {
        if (AUL_R_OK != *itAS) {
            string appid_o;
            update_appid((*itV).first, (*itV).second, *itAS);
            if (AUL_R_OK == *itAS) {
                list<string>::iterator it_h = m_appHistoryList.begin();
                while (it_h != m_appHistoryList.end()) {
                    if (0 ==appid_o.compare(*it_h)) {
                        *it_h = (*itV).second;
                    }
                    ++it_h;
                }
            }
        }
        ++itV;
        ++itAS;
    }
    ICO_DBG("end");
}

/**
 * @brief start check item add
 * @param pid
 * @param appid
 * @param display posi
 */
void CicoHSAppHistoryExt::startupCheckAdd(int pid, const std::string& appid,
                                          bool d)
{
    ICO_DBG("add pid=%d, appid=%s", pid, appid.c_str());
    m_vpbpa.push_back(pairBoolPidAppid(false, pairPidAppid(pid, appid)));
    if (true == d) {
        m_subDispApp = appid;
    }
    else {
        m_lastStartupApp = appid;
    }
}

/**
 * @brief start-up Entry Finish
 * @param pid
 */
void CicoHSAppHistoryExt::startupEntryFinish(int pid)
{
    vector<pairBoolPidAppid>::iterator it = m_vpbpa.begin();
    while (it != m_vpbpa.end()) {
        if ((*it).second.first == pid) {
            ICO_DBG("FINISH!! %d, %s", (*it).second.first,
                    (*it).second.second.c_str());
            (*it).first = true;
            break;  // break of while
        }
        it++;
    }
}

void CicoHSAppHistoryExt::startupEntryFinish(const string& appid)
{
    vector<pairBoolPidAppid>::iterator it = m_vpbpa.begin();
    while (it != m_vpbpa.end()) {
        if (0 == (*it).second.second.compare(appid)) {
            ICO_DBG("FINISH!! %d, %s", (*it).second.first,
                    (*it).second.second.c_str());
            (*it).first = true;
        }
        it++;
    }
}

/**
 * @brief is finish start-up check
 * @ret bool
 * @retval true finish
 * @retval false start-up continue
 */
bool CicoHSAppHistoryExt::isFinish()
{
    bool bR = true;
    vector<pairBoolPidAppid>::iterator it = m_vpbpa.begin();
    while (it != m_vpbpa.end()) {
        if (false == (*it).first) {
            bR = false;
            break; // brek of while
        }
        it++;
    }
    ICO_DBG("%s", bR? "true": "false");
    return bR;
}

/**
 * @brief stop startup check
 */
void CicoHSAppHistoryExt::stopStartupCheck()
{
    ICO_DBG("stop");
    m_vpbpa.clear();
    m_lastStartupApp.clear();
}
// vim: set expandtab ts=4 sw=4: