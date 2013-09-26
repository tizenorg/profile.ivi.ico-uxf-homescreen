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

#include "CicoLog.h"
#include "CicoSCAulItems.h"
#include "CicoSCAilItems.h"
#include "CicoSCConf.h"
#include "CicoSCSystemConfig.h"

using namespace std;

/**
 * @brief AUL Items class constructor
 */
CicoSCAulItems::CicoSCAulItems()
{
    ICO_TRA("CicoSCAulItems::CicoSCAulItems");
    m_appid.clear(); // appid
    m_pid = 0; // pid
    m_category = DINITm_categoryID;
    m_cpucgroup = -1;
    m_memcgroup = -1;
    m_defCgrpCpu.clear();
    m_defCgrpMem.clear();
}

/**
 * @brief AUL Items class constructor
 */
CicoSCAulItems::CicoSCAulItems(const char* appid, int pid, int ctgry,
                               const CicoSCWindow* obj)
    :m_appid(appid), m_pid(pid), m_category(ctgry)
{
    ICO_TRA("constructor %s, %d, %x", appid? appid: "(NIL)", pid, obj);
    enterWindow(obj);
    getPidCgroupInfo(pid, m_defCgrpMem, m_defCgrpCpu);
    m_cpucgroup = -1;
    m_memcgroup = -1;
    if (DINITm_categoryID != m_category) {
        CicoSCSystemConfig* conf = CicoSCSystemConfig::getInstance();
        const CicoSCCategoryConf* objX = 
            conf->getCategoryObjbyCaategoryID(m_category);
        if (NULL != objX) {
            m_cpucgroup = objX->rctrl;
            ICO_DBG("categ:%d -> cgroup: %d", m_category, m_cpucgroup);
        }
    }
}

/**
 * @brief AUL Items class constructor
 */
CicoSCAulItems::CicoSCAulItems(const CicoSCAulItems& s)
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
}

/**
 * @brief AUL Items class destructor
 */
CicoSCAulItems::~CicoSCAulItems()
{
    ICO_TRA("CicoSCAulItems::~CicoSCAulItems");
    m_CSCWptrs.clear();
}

/**
 * @brief CicoSCWindow pointer entry
 * @param obj entry pointer
 */
void CicoSCAulItems::enterWindow(const CicoSCWindow* obj)
{
    ICO_TRA("CicoSCAulItems::enterWindow %x", obj);
    if ((NULL == obj) || (0 == obj)) {
        ICO_TRA("CicoSCAulItems::enterWindow");
        return;
    }
    bool bingo = false; // Registered flag off
#if 1 // TODO mk_k
    vector<const CicoSCWindow*>::iterator it = m_CSCWptrs.begin();
    vector<const CicoSCWindow*>::iterator theEnd = m_CSCWptrs.end();
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
    ICO_TRA("CicoSCAulItems::enterWindow");
    return;
}

/**
 * @brief removw CicoSCWindow pointer
 * @param obj remove target
 */
void CicoSCAulItems::rmWindow(const CicoSCWindow* obj)
{
    ICO_TRA("CicoSCAulItems::rmWindow %x", obj);
    vector<const CicoSCWindow*>::iterator it = m_CSCWptrs.begin();
    vector<const CicoSCWindow*>::iterator theEnd = m_CSCWptrs.end();
    for(; it != theEnd; ++it) {
        if (obj == *it) {
            ICO_TRA("CicoSCAulItems::rmWindow");
            m_CSCWptrs.erase(it);
            break; // break of for
        }
    }
    ICO_TRA("CicoSCAulItems::rmWindow");
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
bool CicoSCAulItems::getPidCgroupInfo(int pid, string& m, string& c)
{
    ICO_TRA("CicoSCAulItems::getPidCgroupInfo");
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
            ICO_DBG("CicoSCAulItems::getPidCgroupInfo m=%s, c=%s", pM, pC);
            m = pM;
            c = pC;
            bR = true;
            break;  // break of while
        }
    }
    ifs.close();
    ICO_TRA("CicoSCAulItems::getPidCgroupInfo %s", bR? "true": "false");
    return bR;
}

