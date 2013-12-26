/*
 * Copyright (c) 2013, TOYOTA MOTOR CORPORATION.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 */
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <memory>
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <aul/aul.h>

#include <ico_log.h>
#include "CicoAulItems.h"
#include "CicoAilItems.h"
#include "CicoConf.h"
#include "CicoSystemConfig.h"

using namespace std;

/**
 * @brief AUL Items class constructor
 */
CicoAulItems::CicoAulItems()
{
    ICO_TRA("start");
    m_appid.clear(); // appid
    m_pid = 0; // pid
    m_category = DINITm_categoryID;
    m_cpucgroup = -1;
    m_memcgroup = -1;
    m_defCgrpCpu.clear();
    m_defCgrpMem.clear();
    m_aulstt = AUL_R_OK;
    ICO_TRA("end");
}

/**
 * @brief AUL Items class constructor
 */
CicoAulItems::CicoAulItems(const char* appid, int pid, int ctgry,
                               int aulstt, const void* obj)
    :m_appid(appid), m_pid(pid), m_category(ctgry), m_aulstt(aulstt)
{
    ICO_TRA("start %s, %d, %x", appid? appid: "(NIL)", pid, obj);
    enterWindow(obj);
    getPidCgroupInfo(pid, m_defCgrpMem, m_defCgrpCpu);
    m_cpucgroup = -1;
    m_memcgroup = -1;
    if (DINITm_categoryID != m_category) {
        CicoSystemConfig* conf = CicoSystemConfig::getInstance();
        const CicoSCCategoryConf* objX = 
            conf->getCategoryObjbyCaategoryID(m_category);
        if (NULL != objX) {
            m_cpucgroup = objX->rctrl;
            ICO_DBG("categ:%d -> cgroup: %d", m_category, m_cpucgroup);
        }
    }
    ICO_TRA("end");
}

/**
 * @brief AUL Items class constructor
 */
CicoAulItems::CicoAulItems(const CicoAulItems& s)
{
    m_appid = s.m_appid;
    m_pid = s.m_pid;
    m_category = s.m_category;
    m_cpucgroup = s.m_cpucgroup;
    m_memcgroup = s.m_memcgroup;
#if 1 // TODO mk_k
    m_CSCWptrs = s.m_CSCWptrs;
#else
    int sz = s.m_CSCWptrs.size();
    for (int i = 0; i < sz; i++) {
        m_CSCWptrs.push_back(s.m_CSCWptrs[i]);
    }
#endif
    m_defCgrpCpu = s.m_defCgrpCpu;
    m_defCgrpMem = s.m_defCgrpMem;
    m_aulstt = s.m_aulstt;
}

/**
 * @brief AUL Items class destructor
 */
CicoAulItems::~CicoAulItems()
{
    ICO_TRA("CicoAulItems::~CicoAulItems");
    m_CSCWptrs.clear();
}

/**
 * @brief window information pointer entry
 * @param obj entry pointer
 */
void CicoAulItems::enterWindow(const void* obj)
{
    ICO_TRA("CicoAulItems::enterWindow %x", obj);
    if ((NULL == obj) || (0 == obj)) {
        ICO_TRA("CicoAulItems::enterWindow");
        return;
    }
    bool bingo = false; // Registered flag off
#if 1 // TODO mk_k
    vector<const void*>::iterator it = m_CSCWptrs.begin();
    vector<const void*>::iterator theEnd = m_CSCWptrs.end();
    for(; it != theEnd; ++it) {
        if (obj == *it) { // if Registered ?
            bingo = true; // Registered flag on
            break; // break of for
        }
    }
#else
    int sz = m_CSCWptrs.size();
    for (int i = 0; i < sz; i++) {
        if (obj == m_CSCWptrs[i]) {
            bingo = true;
            break; // break of for
        }
    }
#endif
    if (false == bingo) {
        ICO_TRA("add window pointer");
        m_CSCWptrs.push_back(obj);
    }
    ICO_TRA("CicoAulItems::enterWindow");
    return;
}

/**
 * @brief remove window information pointer
 * @param obj remove target
 */
void CicoAulItems::rmWindow(const void* obj)
{
    ICO_TRA("CicoAulItems::rmWindow %x", obj);
    vector<const void*>::iterator it = m_CSCWptrs.begin();
    vector<const void*>::iterator theEnd = m_CSCWptrs.end();
    for(; it != theEnd; ++it) {
        if (obj == *it) {
            ICO_TRA("CicoAulItems::rmWindow");
            m_CSCWptrs.erase(it);
            break; // break of for
        }
    }
    ICO_TRA("CicoAulItems::rmWindow");
    return;
}

/**
 * @brief get cgroup data by /proc/[pid]/cgroup file
 * @parm pid target pid number
 * @param m store cgroup memory directory data
 * @param c store cgroup cpu,cpuacct directory data
 */
static const char* g_procPidCgroupFileFmt="/proc/%d/cgroup";
static const char* g_cpuWord = "cpuacct,cpu:";
static const char* g_memWord = "memory:";
bool CicoAulItems::getPidCgroupInfo(int pid, string& m, string& c)
{
    ICO_TRA("start");
    char fn[64];
    sprintf(fn, g_procPidCgroupFileFmt, pid);
    const size_t cpuWdSz = strlen(g_cpuWord);
    const size_t memWdSz = strlen(g_memWord);
    string tmp;
    ifstream ifs(fn);
    bool bR = false;
    const char* pC = 0;
    const char* pM = 0;
    while (ifs >> tmp) {
        if (true == tmp.empty()) {
            continue;
        }
        const char* pT = tmp.c_str();
        const char* pS = pT;
        for (;pS != '\0'; pS++) {
            if (':' == *pS) {
                pS++;
                if (0 == strncmp(pS, g_cpuWord, cpuWdSz)) { //cpu
                    pC = pS + cpuWdSz; // get cgroup cpu directory
                }
                else if (0 == strncmp(pS, g_memWord, memWdSz)) { // memory
                    pM = pS + memWdSz; // get cgroup memory directory
                }
                break; // break of for
            }
        }
        if ((0 != pC) && (0 != pM)) {
            ICO_DBG("CicoAulItems::getPidCgroupInfo m=%s, c=%s", pM, pC);
            m = pM;
            c = pC;
            bR = true;
            break;  // break of while
        }
    }
    ifs.close();
    ICO_TRA("end %s", bR? "true": "false");
    return bR;
}

/**
 * @brief appid update
 */
void CicoAulItems::update_appid()
{
    if (AUL_R_OK == m_aulstt) {
        return;
    }
    ICO_TRA("update start %d, %s", m_aulstt, m_appid.c_str());
    char buf[255];
    buf[0] = '\0'; // STOP CODE
    m_aulstt = aul_app_get_appid_bypid(m_pid, buf, sizeof(buf));
    if (AUL_R_OK == m_aulstt) {
        m_appid = buf;
    }
    ICO_TRA("update end %d, %s", m_aulstt, m_appid.c_str());
    return ;
}
// vim:set expandtab ts=4 sw=4:
